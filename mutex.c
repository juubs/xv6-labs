/**
  * @file mutex.c
  * @brief Allows the user to create mutex locks used for thread control. Only one thread can execute on a section of code that is run while the lock is being held. Other threads waiting to for the mutex to unlock will sleep until the lock becomes available.
  */

#include "types.h"
#include "stat.h"
#include "user.h"
#include "x86.h"
#include "pthread.h"

static long unsigned int next_id = 0;

// Internal use functions for mutex's anonymous spinlock.
void acquire_mutex(pthread_mutex_t *mutex);
void release_mutex(pthread_mutex_t *mutex);

/**
  * @brief Initialize a mutex object.
  * @param mutex Already alloc'd pthread_mutex_t pointer. Will be filled in with appropriate mutex information. Not attached to any threads on its own.
  */
void
pthread_mutex_init(pthread_mutex_t *mutex) {
  mutex->id = next_id++;
  mutex->locked = 0;
  mutex->lk.locked = 0;
}

int
pthread_mutex_destroy(pthread_mutex_t *mutex) {
  free(mutex);
  return 0;
}

/**
  * @brief Acquires the mutex lock. Other threads will not be able to execute the same code while this lock is being held.
  * @param mutex Mutex initialized with pthread_mutex_init.
  */
void
pthread_mutex_lock(pthread_mutex_t *mutex) {
  acquire_mutex(mutex);
  while (mutex->locked)
    mutex_sleep(mutex);
  mutex->locked = 1;
  release_mutex(mutex);
}

/**
  * @brief Release a held mutex lock. Other threads will now be able execute in the critical section.
  * @param mutex Mutex initialized with pthread_mutex_init.
  */
void
pthread_mutex_unlock(pthread_mutex_t *mutex) {
  acquire_mutex(mutex);
  mutex->locked = 0;
  mutex_wakeup(mutex);
  release_mutex(mutex);
}

// mutex internal anonymous spinlock 
void
acquire_mutex(pthread_mutex_t *mutex) {
  while (xchg(&mutex->lk.locked, 1) != 0) ; 
}

void
release_mutex(pthread_mutex_t *mutex) {
  xchg(&mutex->lk.locked, 0);
}
