/**
  * @file cond.c
  * @brief Condition variables used for controlling flow of execution of pthreads. 
  */

#include "types.h"
#include "stat.h"
#include "user.h"
#include "pthread.h"

static unsigned int next_cond = 0;

/**
  * @brief Initialize a condition variable object.
  * @param cv Already alloc'd pthread_cond_t pointer. Will be filled in with appropriate condition variable information. 
  */
void
pthread_cond_init(pthread_cond_t *cv) {
  cv->cond = next_cond++;
}

int
pthread_cond_destroy(pthread_cond_t *cv) {
  free(cv);
  return 0;
}

/**
  * @brief Will cause the calling process to sleep until the given condition variable is signalled. This will attach the given condition variable to the process. Must be holding the given mutex when called, and will atomically unlock and go to sleep.
  * @param cv Condition variable to wait for.
  * @param mutex Calling thread must hold this lock.
  */
void
pthread_cond_wait(pthread_cond_t *cv, pthread_mutex_t *mutex) {
  cond_sleep(cv, mutex);    
}

/**
  * @brief Will wake up a process waiting on the given condition variable.
  * @param cv The condition variable to signal.
  */
void
pthread_cond_signal(pthread_cond_t *cv) {
  cond_wakeup(cv);
}
