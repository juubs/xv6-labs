#include "types.h"
#include "stat.h"
#include "user.h"
#include "pthread.h"

int
pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *arg), void *arg) {

}

int
pthread_join(pthread thread, void **retval) {

}

void pthread_exit(void *retval) {

}
