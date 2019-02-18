	.text

/* Switch from current_thread to next_thread. Make next_thread
 * the current_thread, and set next_thread to 0.
 * Use eax as a temporary register; it is caller saved.
 */
	.globl thread_switch
thread_switch:
  pushal

  // save %esp to current_thread->sp
  movl current_thread, %eax
  movl %esp, (%eax)

  // load next_thread->sp to %esp
  movl next_thread, %eax
  movl (%eax), %esp

  // current thread = next thread
  movl %eax, current_thread
  
  // next thread = 0
  movl $0, next_thread

  popal
  ret				/* pop return address from stack */
