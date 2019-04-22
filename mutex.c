#include "pthread.h"

int
pthread_mutex_init(pthread_mutex_t *mutex, pthread_mutexattr_t *mutexattr) {
  mutex = malloc(sizeof(pthread_mutex_t));
  mutex->locked = 0;
  if (mutexattr == NULL) {
    mutex->attr = NULL; 
  } else {
    mutex->attr.name = mutexattr->name;
    mutex->attr.cpu = mutexattr->cpu;
  }
  return 0;
}

int
pthread_mutex_destroy(pthread_mutex_t *mutex) {
  if (mutex->locked)
    panic("trying to destroy locked mutex");
}

int
pthread_mutex_lock(pthread_mutex_t *mutex) {

}

int
pthread_mutex_unlock(pthread_mutex_t *mutex) {

}
