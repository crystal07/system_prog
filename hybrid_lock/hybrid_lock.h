#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct hybrid_lock {
	/* ... */
};

int hybrid_lock_init(/* ... */);
int hybrid_lock_destroy(/* ... */);
int hybrid_lock_lock(/* ... */);
int hybrid_lock_unlock(/* ... */);
