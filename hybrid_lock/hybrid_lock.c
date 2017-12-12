#include "hybrid_lock.h"

int hybrid_lock_init(hybrid_lock_t *hybrid_lock)
{
	if (pthread_mutex_init(&hybrid_lock->mutex, NULL) != 0) {
		fprintf(stderr, "mutex init error\n");
		exit(-1);
	}

	if (pthread_spin_init(&hybrid_lock->spin, PTHREAD_PROCESS_PRIVATE) != 0) {
		fprintf(stderr, "spin init error\n");
		exit(-1);
	}
	hybrid_lock->is_spin = 0;
	return 0;
}

int hybrid_lock_destroy(hybrid_lock_t *hybrid_lock)
{
	pthread_mutex_destroy(&hybrid_lock->mutex);
	pthread_spin_destroy(&hybrid_lock->spin);
}

/*
int hybrid_lock_lock(hybrid_lock_t *hybrid_lock, float time, float per_time)
{
	struct timeval start, current;
	int result;
	float operating_time;
	
	gettimeofday(&start, NULL);
	
	float x = time / per_time;
	long long int count = (long long int)ceil(x) * 50000;
	long long int i = count;

	while (i > 0) {
		if (!(result = pthread_spin_trylock(&hybrid_lock->spin))) {
			hybrid_lock->is_spin = 1;
			printf("thread : %lld, spin lock\n", pthread_self());
			return result;
		}
		i--;
	}

	gettimeofday(&current, NULL);
	operating_time = (double)(current.tv_sec)+(double)(current.tv_usec)/1000000.0-(double)(start.tv_sec)-(double)(start.tv_usec)/1000000.0;
	if (operating_time >= (double)time) {
		printf("thread L %lld, mutex lock\n", pthread_self());
		result = pthread_mutex_lock(&hybrid_lock->mutex);
		hybrid_lock->is_spin = 0;
		return result;
	}
	else {
		printf("call recursive func\n");
		printf("thread : %lld. remained time : %f\n", pthread_self(), time - operating_time);
		return hybrid_lock_lock(hybrid_lock, time - operating_time, per_time);
	}
}
*/

int hybrid_lock_lock(hybrid_lock_t *hybrid_lock, long long int count)
{
	struct timeval start, current;
	int result;
	float operating_time;
	
	gettimeofday(&start, NULL);
	
	long long int i = count;

	while (i > 0) {
		if ((result = pthread_spin_trylock(&hybrid_lock->spin)) == 0) {
			hybrid_lock->is_spin = 1;
			//printf("thread : %lld, spin lock\n", pthread_self());
			return result;
		}
		i--;
	}

	gettimeofday(&current, NULL);
	operating_time = (double)(current.tv_sec)+(double)(current.tv_usec)/1000000.0-(double)(start.tv_sec)-(double)(start.tv_usec)/1000000.0;
	
	if (operating_time >= (double)1) {
		//printf("thread L %lld, mutex lock\n", pthread_self());
		result = pthread_mutex_lock(&hybrid_lock->mutex);
		hybrid_lock->is_spin = 0;
		return result;
	}
	else {
		//printf("thread %lld, call recursive func\n", pthread_self());
		return hybrid_lock_lock(hybrid_lock, count);
	}
}

int hybrid_lock_unlock(hybrid_lock_t *hybrid_lock)
{
	int result;
	if (hybrid_lock->is_spin) result = pthread_spin_unlock(&hybrid_lock->spin);
	else result = pthread_mutex_unlock(&hybrid_lock->mutex);
	hybrid_lock->is_spin = 0;
	return result;
}
