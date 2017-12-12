#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "hybrid_lock.h"

long g_count = 0;
pthread_mutex_t g_mutex;
hybrid_lock_t hybrid;
float while_time;
long long int while_count;

float get_while_time (){
	int i = 1;
	struct timeval start, current;
	float operating_time;
	pthread_spinlock_t spin;
	int is_spin; 
	int result;

	pthread_spin_init(&spin, PTHREAD_PROCESS_PRIVATE);

	gettimeofday(&start, NULL);
	while (i > 0) {
		if (!(result = pthread_spin_trylock(&spin))) {
			is_spin = 1;
			break;
		}
		i--;
	}
	gettimeofday(&current, NULL);
	pthread_spin_destroy(&spin);
	return (double)(current.tv_sec)+(double)(current.tv_usec)/1000000.0-(double)(start.tv_sec)-(double)(start.tv_usec)/1000000.0;
}

long long int get_count_while_per_sec() {
	long long int i = 0;
	struct timeval start, current;
	float operating_time = 0;
	pthread_spinlock_t spin;
	int is_spin; 
	int result;

	pthread_spin_init(&spin, PTHREAD_PROCESS_PRIVATE);

	gettimeofday(&start, NULL);
	while (operating_time < (float)1) {
		i++;
		//printf("i %d, operating_time %f\n", i, operating_time);
		if (!(result = pthread_spin_trylock(&spin))) {
			is_spin = 1;
			//continue;
		}
		gettimeofday(&current, NULL);
		operating_time = (double)(current.tv_sec)+(double)(current.tv_usec)/1000000.0-(double)(start.tv_sec)-(double)(start.tv_usec)/1000000.0;
	}
	pthread_spin_destroy(&spin);
	return i;
}

void *thread_func(void *arg)
{
	long i, count = (long)arg;

	/*
	 * Increase the global variable, g_count.
	 * This code should be protected by
	 * some locks, e.g. spin lock, and the lock that 
	 * you implemented for assignment,
	 * because g_count is shared by other threads.
	 */
	//pthread_mutex_lock(&g_mutex);
	for (i = 0; i<count; i++) {
		hybrid_lock_lock(&hybrid, while_count);
		//pthread_mutex_unlock(&g_mutex);
		printf("%d thead : %d count\n", pthread_self(), g_count);
		/************ Critical Section ************/
		g_count++;
		/******************************************/
		hybrid_lock_unlock(&hybrid);
	}
}

int main(int argc, char *argv[])
{
	pthread_t *tid;
	long i, thread_count, value, v;
	int rc;
	//while_time = get_while_time();
	while_count = get_count_while_per_sec();
	//printf("cout %d\n", count);
	//scanf("%ld", &v);

	/*
	 * Check that the program has three arguments
	 * If the number of arguments is not 3, terminate the process.
	 */
	if (3 != argc) {
		fprintf(stderr, "usage: %s <thread count> <value>\n", argv[0]);
		exit(0);
	}

	if (pthread_mutex_init(&g_mutex, NULL) != 0) {
		fprintf(stderr, "g_mutex init error\n");
		exit(-1);
	}

	if (hybrid_lock_init(&hybrid) != 0) {
		fprintf(stderr, "hybrid init error\n");
		exit(-1);
	}

	/*
	 * Get the values of the each arguments as a long type.
	 * It is better that use long type instead of int type,
	 * because sizeof(long) is same with sizeof(void*).
	 * It will be better when we pass argument to thread
	 * that will be created by this thread.
	 */
	thread_count = atol(argv[1]);
	value = atol(argv[2]);

	/*
	 * Create array to get tids of each threads that will
	 * be created by this thread.
	 */
	tid = (pthread_t*)malloc(sizeof(pthread_t)*thread_count);
	if (!tid) {
		fprintf(stderr, "malloc() error\n");
		exit(0);
	}

	/*
	 * Create a threads by the thread_count value received as
	 * an argument. Each threads will increase g_count for
	 * value times.
	 */
	for (i = 0; i<thread_count; i++) {
		rc = pthread_create(&tid[i], NULL, thread_func, (void*)value);
		if (rc) {
			fprintf(stderr, "pthread_create() error\n");
			free(tid);
			pthread_mutex_destroy(&g_mutex);
			hybrid_lock_destroy(&hybrid);
			exit(0);
		}
	}

	/*
	 * Wait until all the threads you created above are finished.
	 */
	for (i = 0; i<thread_count; i++) {
		rc = pthread_join(tid[i], NULL);
		if (rc) {
			fprintf(stderr, "pthread_join() error\n");
			free(tid);
			pthread_mutex_destroy(&g_mutex);
			hybrid_lock_destroy(&hybrid);
			exit(0);
		}
	}

	pthread_mutex_destroy(&g_mutex);
	hybrid_lock_destroy(&hybrid);

	/*
	 * Print the value of g_count.
	 * It must be (thread_count * value)
	 */ 
	printf("value: %ld\n", g_count);

	free(tid);
}
