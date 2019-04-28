/**
  * @file pthread.h
  * @brief A pthread (POSIX-threading) library implementation for xv6.
  */

#ifndef _PTHREAD_H
#define _PTHREAD_H

#define PGSIZE 4096
#define NULL 0

#define PTHREAD_DEFAULT_INIT (malloc(sizeof(pthread_t)))
#define PTHREAD_MUTEX_INIT (malloc(sizeof(pthread_mutex_t)))
#define PTHREAD_COND_INIT (malloc(sizeof(pthread_cond_t)))

struct _pthread_s;
struct _pthread_mutex_s;
struct _pthread_cond_s;

typedef struct _pthread_s pthread_t;
typedef struct _pthread_mutex_s pthread_mutex_t;
typedef struct _pthread_cond_s pthread_cond_t;

struct _pthread_s {
  long unsigned int id;
  pthread_cond_t *cv;
  void **retval;
};

struct _pthread_mutex_s {
  long unsigned int id;
  unsigned int locked;
  struct {
    unsigned int locked;
  } lk;
};

struct _pthread_cond_s {
  unsigned int cond;
};

// pthread.c
int pthread_create(pthread_t *thread, void (*start_routine)(void *arg), void *arg);
int pthread_join(pthread_t *thread, void **retval);
void pthread_exit(void *retval);

// mutex.c
void pthread_mutex_init(pthread_mutex_t *mutex);
int pthread_mutex_destroy(pthread_mutex_t *mutex);
void pthread_mutex_lock(pthread_mutex_t *mutex);
void pthread_mutex_unlock(pthread_mutex_t *mutex);

// cond.c
void pthread_cond_init(pthread_cond_t *cv);
int pthread_cond_destroy(pthread_cond_t *cv);
void pthread_cond_wait(pthread_cond_t *cv, pthread_mutex_t *mutex);
void pthread_cond_signal(pthread_cond_t *cv);

#endif
