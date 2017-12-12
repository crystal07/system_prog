#include "hybrid_lock.h"
#include <chrono>

int hybrid_lock_init(hybrid_lock* lock)
{
   return 0;
}

int hybrid_lock_destroy(hybrid_lock* lock)
{
   return 0;
}

int hybrid_lock_lock(hybrid_lock* lock)
{
   if (lock->lock_count > 2)
      lock->mtx_lock.lock();
   else
   {
      bool result = false;      

      auto start = std::chrono::high_resolution_clock::now();
      auto end = std::chrono::high_resolution_clock::now();
      while (!result && ((end - start).count() < 1000000000)) {
         end = std::chrono::high_resolution_clock::now();
         result = lock->mtx_lock.try_lock();
      }

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

int hybrid_lock_lock(hybrid_lock* lock, long long int c)
{
   if (lock->lock_count > 2)
      lock->mtx_lock.lock();
   else
   {
      bool result = false;      
      int count = 0;

      while ((!result) && (count < c)) {
         ++count;
         result = lock->mtx_lock.try_lock();
      }

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