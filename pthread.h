#ifndef _PTHREAD_H
#define _PTHREAD_H

#define PGSIZE 4096

typedef struct _pthread_mutex_t {

} pthread_mutex_t;

typedef struct _pthread_mutexattr_t {

} pthread_mutexattr_t;

typedef struct _pthread_cond_t {

} pthread_cond_t;

typedef struct _pthread_condattr_t {

} pthread_condattr_t;

// pthread.c
int pthread_create(void (*func)(void*), void *arg);
int pthread_join();

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
