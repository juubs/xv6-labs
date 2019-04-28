/**
  * @file pthread.c
  * @brief Function definitions for thread creating, joining, and exiting.
  */

#include "types.h"
#include "stat.h"
#include "user.h"
#include "pthread.h"

static unsigned long next_id = 0;

/**
  * @brief Creates a new thread that begins executing at start_routine with argument arg.
  * @param thread Already alloc'd pthread_t pointer. Will be filled in with appropriate thread information and attached to the new thread process through the clone system call.
  * @param start_routine Function pointer where new thread will begin executing.
  * @param arg Parameter for start_routine.
  */
int
pthread_create(pthread_t *thread, void (*start_routine)(void *arg), void *arg) {
  // give new thread unique id
  thread->id = next_id++;

  // allocate and page align new user stack
  void *ustack = malloc(PGSIZE*2);
  if ((unsigned int)ustack % PGSIZE)
    ustack += (PGSIZE - (unsigned int)ustack % PGSIZE);
  
  return clone(thread, ustack, start_routine, arg);
}

/**
  * @brief Waits for the given thread the finish, and places the value given from that thread in pthread_exit into retval.
  * @param thread Thread created with pthread_create.
  * @param retval Pointer to the retval pointer that pthread_exit can fill upon exiting. NULL if no return value needed.
  * @return If the process does not have a pthread running with the given thread object or the process has been killed, will return -1. Otherwise, will return the pid of the exiting thread.
  */
int
pthread_join(pthread_t *thread, void **retval) {
  if (retval != NULL)
    *retval = *thread->retval;
  return thread_join(thread);
}

/**
  * @brief This is used instead of exit() to terminate an executing thread. Can return a value through retval to the joining process.
  * @param retval Pointer to return value to give to joining thread. NULL if no return value needed.
  */
void
pthread_exit(void *retval) {
  thread_exit(retval);
}
