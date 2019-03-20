#include "types.h"
#include "stat.h"
#include "user.h"
#include "sched.h"

void run_child();

int
main(void) 
{
  printf(1, "%d\n", getcpu());
  // set parent to very high priority to finish forking before running children
  setscheduler(SCHED_FIFO, 99);

  // test laid out in assignment description
  for(int i=0; i<6; i++) {
    if (fork() == 0) {
      switch (i) {
        // FIFO w/ priority 3
        case 0:
          setscheduler(SCHED_FIFO, 3);
          break;
        // FIFO w/ priority 4
        case 1:
          setscheduler(SCHED_FIFO, 4);
          break;
        // RR w/ priority 3
        case 2:
        case 3:
          setscheduler(SCHED_RR, 3);
          break;
        // RR w/ priority 2
        case 4:
        case 5:
          setscheduler(SCHED_RR, 2);
          break;
        default:
          // how?
          break;
      }
      run_child();
      printf(1, "proc %d done\n", getpid());
      exit();
    }
  }
  
  // general test
  /*
  for(int i=0; i<20; i++) {
    if (fork() == 0) {
      // set every other child to fifo with increasing priority
      if (i % 2 == 0)
        setscheduler(SCHED_FIFO, i);
      run_child();
      printf(1, "proc %d done\n", getpid());
      exit();
    }
  }
  */
  
  // wait for each child to finish
  while (wait() > 0) ;

  printf(1, "all children done\n");
  exit();
}

void
run_child() {
  //spin for a bit
  for(int i=0; i<100000000; i++) {
    // print proc progress every tenth slice of loop
    if (i % 10000000 == 0)
      printf(1, "proc %d running...\n", getpid());
  }
}
