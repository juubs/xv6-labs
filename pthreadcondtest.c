/** 
  * @file pthreadcondtest.c
  * @brief Pthread library usage example of condition variables. Creates 3 threads, one that increments a global counter, and 2 that wait until that counter hits a certain number. The incrementing thread will signal the condition variable on every increment, but the other threads will go back to sleep until the counter reaches their given numbers. Once it reaches that, those threads will print that they have done so and exit.\
  * Run with 'pthreadcondtest' from the xv6 shell.
  */

#include "types.h"
#include "stat.h"
#include "user.h"
#include "pthread.h"

pthread_mutex_t cond_mutex;
pthread_cond_t cv;
static unsigned int counter = 0;

static void 
cond_thread(void *arg) {
  pthread_mutex_lock(&cond_mutex);
  // wait on cv while global counter is less than given arg
    while (counter < *(int*)arg)
      pthread_cond_wait(&cv, &cond_mutex);
  printf(1, "counter reached %d, exiting thread\n", *(int*)arg);
  pthread_mutex_unlock(&cond_mutex);

  pthread_exit(NULL);
}

static void
signal_thread(void *arg) {
  for (int i=0; i<1000000; i++) {
    if (i % 10000 == 0) {
      // every 1000 cycles, increment arg and signal to check for wakeup of other threads
      counter++;
      printf(1, "counter at: %d\n", counter);
      pthread_mutex_lock(&cond_mutex);
      pthread_cond_signal(&cv);
      pthread_mutex_unlock(&cond_mutex);
    }
  }

  pthread_exit(NULL);
}

int
main(int argc, char *argv[]) {
  pthread_t *thread1 = PTHREAD_DEFAULT_INIT;
  pthread_t *thread2 = PTHREAD_DEFAULT_INIT;
  pthread_t *thread3 = PTHREAD_DEFAULT_INIT;

  int a = 50;
  int b = 100;
 
  pthread_create(thread1, cond_thread, (void*)(&a));
  pthread_create(thread2, cond_thread, (void*)(&b));
  pthread_create(thread3, signal_thread, NULL);

  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);
  pthread_join(thread3, NULL);

  exit();
}
