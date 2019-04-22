#ifndef _PTHREAD_H
#define _PTHREAD_H

#define PGSIZE 4096

typedef struct _pthread_attr_t {
  // fill attributes here
} pthread_attr_t;

typedef struct _pthread_t {
  uint id;
  pthread_attr_t attr;
} pthread_t;

typedef struct _pthread_mutexattr_t {
  char *name;
  struct cpu cpu;
} pthread_mutexattr_t;

typedef struct _pthread_mutex_t {
  uint locked;
  pthread_mutexattr_t attr;
} pthread_mutex_t;

typedef struct _pthread_condattr_t {
  // fill attributes here
} pthread_condattr_t;

typedef struct _pthread_cond_t {

  pthread_condattr_t attr;
} pthread_cond_t;

// pthread.c
int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void *arg), void *arg);
int pthread_join(pthread thread, void **retval);
void pthread_exit(void *retval);

// mutex.c
int pthread_mutex_init(pthread_mutex_t *mutex, pthread_mutexattr_t *mutexattr);
int pthread_mutex_destroy(pthread_mutex_t *mutex);
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);

int pthread_cond_init(pthread_cond_t *cond, pthread_condattr_t *condattr);
int pthread_cond_destroy(pthread_cond_t *cond);

int pthread_condattr_init(pthread_condattr_t *condattr);
int pthread_condattr_destroy(pthread_condattr_t *condattr);

#endif
