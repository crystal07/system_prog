#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <math.h>

typedef struct hybrid_lock {
	pthread_mutex_t mutex;
	pthread_spinlock_t spin;
	int is_spin;
} hybrid_lock_t;

int hybrid_lock_init(hybrid_lock_t *hybrid_lock);
int hybrid_lock_destroy(hybrid_lock_t *hybrid_lock);
//int hybrid_lock_lock(hybrid_lock_t *hybrid_lock, float time, float per_time);
int hybrid_lock_lock(hybrid_lock_t *hybrid_lock, long long int count);
int hybrid_lock_unlock(hybrid_lock_t *hybrid_lock);
