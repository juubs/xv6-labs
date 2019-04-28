/**
  * @file pthreadtest.c
  * @brief Pthread test using all pieces of library.\n
  * Run with 'pthreadtest' in the xv6 shell.
  */ 

#include "types.h"
#include "stat.h"
#include "user.h"
#include "pthread.h"

pthread_mutex_t mutex;
pthread_mutex_t cond_mutex;
pthread_cond_t cv;

static void
thread_test(void *arg) {
  pthread_mutex_lock(&mutex);

  for(int i=0; i<100000; i++) {
    if (i % 10000 == 0)
      printf(1, "working on test in pid %d\n", getpid());
  }
  
  *(int*)arg = *(int*)arg + 10;
  printf(1, "thread pid %d done, returning %d\n", getpid(), *(int*)arg);
  pthread_mutex_unlock(&mutex);
  
  pthread_mutex_lock(&cond_mutex);
  if (*(int*)arg > 20) {
    printf(1, "signalling cv...\n");
    pthread_cond_signal(&cv);
  }
  pthread_mutex_unlock(&cond_mutex);
  
  pthread_exit(arg);
}

static void
thread_cond_test(void *arg) {
  pthread_mutex_lock(&mutex);
  printf(1, "arg on sleeping cond_wait: %d\n", *(int*)arg);
  pthread_mutex_unlock(&mutex);

  pthread_mutex_lock(&cond_mutex);
    while (*(int*)arg < 20) {
      printf(1, "sleeping...\n");
      pthread_cond_wait(&cv, &cond_mutex);
      printf(1, "cv woken up with arg = %d\n", *(int*)arg);
    }
  pthread_mutex_unlock(&cond_mutex);

  pthread_exit(NULL);
}

int
main(int argc, char *argv[]) {
  printf(1, "pid of parent: %d\n", getpid());

  int a = 13;
  void **ret3 = malloc(sizeof(void*));
  
  pthread_mutex_init(&mutex);
  pthread_mutex_init(&cond_mutex);
  pthread_cond_init(&cv);

  pthread_t *thread1 = PTHREAD_DEFAULT_INIT;
  pthread_t *thread2 = PTHREAD_DEFAULT_INIT;  
  pthread_t *thread3 = PTHREAD_DEFAULT_INIT;

  pthread_create(thread1, thread_cond_test, (void*)(&a));
  pthread_create(thread2, thread_test, (void*)(&a));
  pthread_create(thread3, thread_test, (void*)(&a));

  pthread_join(thread2, NULL);
  pthread_join(thread3, ret3);
  pthread_join(thread1, NULL);

  printf(1, "all threads joined\n");
  printf(1, "thread 3 returned %d\n", **((int**)(ret3)));
 
  exit();
}
