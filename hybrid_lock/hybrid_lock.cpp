#include "hybrid_lock.h"

int hybrid_lock_init(hybrid_lock* lock)
{
	return 0;
}

int hybrid_lock_destroy(hybrid_lock* lock)
{
	return 0;
}

int hybrid_lock_lock(hybrid_lock* lock, long long int count)
{
	if (lock->lock_count > 2)
		lock->mtx_lock.lock();
	else
	{
		bool result = lock->mtx_lock.try_lock();
		if (result)
		{
			lock->lock_count++;
			return 0;
		}

		time_t spinStartTime = time(0);
		while ((time(0) - spinStartTime) < SPIN_TIME) {}

		result = lock->mtx_lock.try_lock();
		if (result)
		{
			lock->lock_count++;
			return 0;
		}

		lock->mtx_lock.lock();
	}
	lock->lock_count++;

	return 0;
}


int hybrid_lock_unlock(hybrid_lock* lock)
{
	lock->mtx_lock.unlock();
	lock->lock_count--;

	return 0;
}
