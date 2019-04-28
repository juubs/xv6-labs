/**
  * @file pthreadmutextest.c
  * @brief Pthread library usage example of mutual exclusive (mutex) locks. Will create 5 threads all running a simple loop that periodically outputs its running status. It will do this twice, once running the loop not inside a mutex lock, and then again running the loop inside a lock. The output of the intermediate progress of each individual process shows how mutex locks can help control execution flow of user programs.\n
  * Run with 'pthreadmutextest' from the xv6 shell.
  */

#include "types.h"
#include "stat.h"
#include "user.h"
#include "pthread.h"

pthread_mutex_t mutex;

static void
thread_test(void *arg) {
  for (int i=0; i<10000; i++) {
    if (i % 1000 == 0)
      printf(1, "running in thread pid %d\n", getpid());
  }

  pthread_exit(NULL);
}

static void
thread_test_mutex(void *arg) {
  pthread_mutex_lock(&mutex);

  for (int i=0; i<10000; i++) {
    if (i % 1000 == 0)
      printf(1, "running in thread pid %d\n", getpid());
  }

  pthread_mutex_unlock(&mutex);

  pthread_exit(NULL);
}

int
main(int argc, char *argv[]) {
  printf(1, "parent pid: %d\n", getpid());

  // init 5 threads
  pthread_t *thread1 = PTHREAD_DEFAULT_INIT;
  pthread_t *thread2 = PTHREAD_DEFAULT_INIT;
  pthread_t *thread3 = PTHREAD_DEFAULT_INIT;
  pthread_t *thread4 = PTHREAD_DEFAULT_INIT;
  pthread_t *thread5 = PTHREAD_DEFAULT_INIT;

  int a = 1;

  // run threads
  printf(1, "--Running threads with no mutex--\n");
  pthread_create(thread1, thread_test, (void*)(&a));
  pthread_create(thread2, thread_test, (void*)(&a));
  pthread_create(thread3, thread_test, (void*)(&a));
  pthread_create(thread4, thread_test, (void*)(&a));
  pthread_create(thread5, thread_test, (void*)(&a));

  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);
  pthread_join(thread3, NULL);
  pthread_join(thread4, NULL);
  pthread_join(thread5, NULL);

  printf(1, "\n--Running threads with mutex locks--\n");
  pthread_create(thread1, thread_test_mutex, (void*)(&a));
  pthread_create(thread2, thread_test_mutex, (void*)(&a));
  pthread_create(thread3, thread_test_mutex, (void*)(&a));
  pthread_create(thread4, thread_test_mutex, (void*)(&a));
  pthread_create(thread5, thread_test_mutex, (void*)(&a));

  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);
  pthread_join(thread3, NULL);
  pthread_join(thread4, NULL);
  pthread_join(thread5, NULL);
  
  exit();
}
