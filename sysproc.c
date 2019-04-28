#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void) {
  return fork();
}

int
sys_exit(void) {
  exit();
  return 0;  // not reached
}

int
sys_clone(void) {
  void *thread, *stack, *func, *arg;

  if (argptr(0, (void*)&thread, sizeof(thread)) < 0)
    return -1;
  if (argptr(1, (void*)&stack, sizeof(stack)) < 0)
    return -1;
  if (argptr(2, (void*)&func, sizeof(func)) < 0)
    return -1;
  if (argptr(3, (void*)&arg, sizeof(arg)) < 0)
    return -1;

  return clone(thread, stack, func, arg);
}

int
sys_thread_join(void) {
  void *thread;

  if (argptr(0, (void*)&thread, sizeof(thread)) < 0)
    return -1;

  return thread_join(thread);
}

int
sys_thread_exit(void) {
  void *retval;

  if (argptr(0, (void*)&retval, sizeof(retval)) < 0)
    return -1;
  
  thread_exit(retval);
  return 0;
}

int
sys_mutex_sleep(void) {
  void *mutex;

  if (argptr(0, (void*)&mutex, sizeof(mutex)) < 0)
    return -1;

  mutex_sleep(mutex);
  return 0;
}

int
sys_mutex_wakeup(void) {
  void *mutex;

  if (argptr(0, (void*)&mutex, sizeof(mutex)) < 0)
    return -1;

  mutex_wakeup(mutex);
  return 0;
}

int
sys_cond_sleep(void) {
  void *cv, *mutex;

  if (argptr(0, (void*)&cv, sizeof(cv)) < 0)
    return -1;
  if (argptr(1, (void*)&mutex, sizeof(mutex)) < 0)
    return -1;

  cond_sleep(cv, mutex);
  return 0;
}

int
sys_cond_wakeup(void) {
  void *cv;

  if (argptr(0, (void*)&cv, sizeof(cv)) < 0)
    return -1;

  cond_wakeup(cv);
  return 0;
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
