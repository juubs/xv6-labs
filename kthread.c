#include "types.h"
#include "stat.h"
#include "user.h"
#include "sched.h"

#define PGSIZE 4096

struct lock_t *lk;

/**
  @brief Creates a new thread that will run starting at start_routine with argument arg passed in.
  @param start_routine Where to begin execution of the new thread.
  @param arg Argument to pass into the start_routine.
  @return Returns the result of the clone system call made.
*/
int
thread_create(void (*start_routine)(void*), void *arg)
{
  void *stack = malloc(PGSIZE*2);
  // page align the new stack
  if ((uint)stack % PGSIZE)
    stack = stack + (PGSIZE - ((uint)stack % PGSIZE));
  return clone(stack, start_routine, arg);
}

/**
  @brief Wait for a thread of the current process to finish execution, then free the user stack used by that thread.
  @return Returns the result of the join system call made.
*/
int
thread_join()
{
  void *stack = malloc(sizeof(void*));
  int freed = join(&stack);

  struct lock_t freethread;
  initlock_t(&freethread);
  acquire_t(&freethread);
  free(stack);
  release_t(&freethread);

  return freed;
}

/**
  @brief First thread testing function. 
  Increments given integer and prints the thread's pid, what cpu it is being run on, and the new value of a.
  @param a Variable for the thread to increment to show shared memory among threads.
*/
static void
thread_test(void *a)
{
  acquire_t(lk);

  *(int*)a = *(int*)a + 1;

  printf(1, "thread: pid %d on cpu %d, a = %d\n", getpid(), getcpu(),*(int*)a);

  release_t(lk);
  exit();
}

/**
  @brief Second thread testing function.
  This thread will set itself to a FIFO schedule (a higher priority than the default) with priority of the given variable. It will then print the same info as thread_test, but sleep for a small period of time before incrementing a and exiting. This is used to show that other threads will still be running while this one sleeps, something not possible with user-level threads.
  @param a Variable for the thread to increment to show shared memory among threads.
*/
static void
thread_test2(void *a)
{
  setscheduler(SCHED_FIFO, *(int*)a);
  
  acquire_t(lk);
  printf(1, "my thread %d running and blocking on CPU %d\n", getpid(), getcpu());
  release_t(lk);

  // block here, other threads still finish
  sleep(100);
  
  *(int*)a = *(int*)a + 1;

  exit();
}

/**
  @brief Third thread testing function.
  This thread is the same as thread_test but will set itself to a Round Robin schedule. This will give the current process a lower priority than the threads given thread_test and so it will be seen how scheduling works alongside threading to control concurrent execution.
  @param a Variable for the thread to increment to show shared memory among threads.
*/
static void
thread_test3(void *a)
{
  setscheduler(SCHED_RR, *(int*)a);
  
  acquire_t(lk);
  printf(1, "my thread pid %d running on CPU %d\n", getpid(), getcpu());
  release_t(lk);
  
  *(int*)a = *(int*)a + 1;

  exit();
}

/**
  @brief Main function to split off of with clones and forks. Uses a variety of the test functions to show interactions between different schedulers alongside threading and how one thread sleeping interacts with the others.
*/
int
main(int argc, char *argv[])
{
  printf(1, "parent has pid %d and is on CPU %d\n", getpid(), getcpu());
  lk = (struct lock_t*)malloc(sizeof(struct lock_t));
  initlock_t(lk);

  int a = 1;

  // BLOCKING PROBLEM: test2 increments a but sleeps. test3 increments and exits immediately. When done with uthreads, once any of the sleep() calls were hit, the entire user program would have to wait until that one thread returned from the system call. With kthreads, you can see that the "test3" threads that are created after the first couple start blocking end up finishing before those threads return, showing that the kthreads implementation solves the blocking problem with uthreads.
  // SEPARATE CPU PROBLEM: In uthread, the threads are all user level and still actually running on one CPU kernel thread. This is seen when running uthread as the threads will all print that they are running on either CPU 0 or CPU 1, but not a mixture. kthreads, however, are actually separate processes sharing a memory space and can be picked up by either CPU. This can be seen by running it multiple times and seeing that the threads run on a mixture of CPUs and each process is not bound to any one CPU.
  // DIFFERENT SCHEDULING & PRIORITES: I had test2 assign a FIFO scheduler and test3 assign a RR scheduler. Since test2 is the one that blocks, this kind of simulates a user thread block in that the processor prioritizes finishing running through the sleep() before attending to other threads. I also set a priority based on "a" at time of calling, which can vary slightly between executions since each thread increments a at some point and other threads assigned priorities will depend on which other threads have reached the line of code to increment that value yet. 
  thread_create(thread_test2, (void*)&a);
  thread_create(thread_test2, (void*)&a);
  thread_create(thread_test3, (void*)&a);
  thread_create(thread_test2, (void*)&a);
  // This forks here makes things weird, but the process still runs consistently and without issue. 
  fork();
  thread_create(thread_test3, (void*)&a);

  // join/wait all threads to exit
  int pid;
  while ((pid = thread_join()) > 0) {
    acquire_t(lk);
    printf(1, "thread %d done\n", pid);
    release_t(lk);
  }
  while ((pid = wait()) > 0) {
    acquire_t(lk);
    printf(1, "forked child %d done\n", pid);
    release_t(lk);
  }

  printf(1, "final value a = %d\n", a); 
  exit();
}
