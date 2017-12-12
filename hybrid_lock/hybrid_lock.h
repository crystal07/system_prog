#include <stdio.h>
#include <stdlib.h>
#include <mutex>

#define SPIN_TIME 1.0

typedef struct hybrid_lock {
	std::mutex mtx_lock;
	int lock_count = 0;
}hybrid_lock_t;

int hybrid_lock_init(hybrid_lock_t *lock);
int hybrid_lock_destroy(hybrid_lock_t *lock);
int hybrid_lock_lock(hybrid_lock_t *lock);
int hybrid_lock_unlock(hybrid_lock_t *lock);
