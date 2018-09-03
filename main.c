#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "monitor.h"

#define LAB 2

static void startothers(void);
static void mpmain(void)  __attribute__((noreturn));
extern pde_t *kpgdir;
extern char end[]; // first address after kernel loaded from ELF file

#if LAB >= 2    // ...then leave this code out.
#elif LAB >= 1
// Test the stack backtrace function (lab 1 only)
void
test_backtrace(int x)
{
  cprintf("entering test_backtrace %d\n", x);
  if (x > 0)
    test_backtrace(x-1);
  else
    mon_backtrace(0, 0, 0);
  cprintf("leaving test_backtrace %d\n", x);
}
#endif

int
test_page_free_list()
{
	//Check the page free list is not corrupted


	//Check that the pages that should not be free are not on the list of free pages


	//Assert that the first part of physical memory have been mapped to free pages

	return 0; //Success

}

int
test_page_free_list_ext()
{
	int success = test_page_free_list();
	if(!success)
		return 0;

	//Assert all unused physical memory have been mapped to free pages
	return 0;
}

int
test_page_alloc()
{
	//Count the number of free pages

	//Allocate a few pages with kalloc

	//Assert all pages are different

	//Assert that the physical addresses are within expected bounds

	//Disable the freelist by saving it to a temporary variable and set freelist to null

	//Assert kalloc returns 0 (null)

	//Free pages allocated in second commment

	//Reallocate pages, assert they are reallocated in reverse order

	//Assert that once all pages are reallocated, kalloc again returns 0

	//Set one page to known junk values

	//Free the page, reallocate it.  Assert that the page is the same one with the same junk values.

	//Restore the page free list saved to the temporary variable in fifth step.  Free the pages allocated in this test.

	//Assert the number of free pages is the same as in the beginning.

	return 0;
}

// Bootstrap processor starts running C code here.
// Allocate a real stack and switch to it, first
// doing some setup required for memory allocator to work.
int
main(void)
{
  kinit1(end, P2V(4*1024*1024)); // phys page allocator
  kvmalloc();      // kernel page table
  uartinit();      // serial port
	int success = test_page_free_list();
	if(!success)
		uartprintcstr("Test_page_free_list failed!\n");
	else
		uartprintcstr("Test_page_free_list succeeded!\n");


  mpinit();        // detect other processors
  lapicinit();     // interrupt controller
  seginit();       // segment descriptors
  cprintf("\ncpu%d: starting xv6\n\n", cpunum());
  picinit();       // another interrupt controller
  ioapicinit();    // another interrupt controller
  consoleinit();   // console hardware

  cprintf("6828 decimal is %o octal!\n", 6828);
  
  pinit();         // process table
  tvinit();        // trap vectors
  binit();         // buffer cache
  fileinit();      // file table
  ideinit();       // disk
  if(!ismp)
    timerinit();   // uniprocessor timer
  startothers();   // start other processors
  kinit2(P2V(4*1024*1024), P2V(PHYSTOP)); // must come after startothers()

	success = test_page_free_list_ext();
	if(!success)
		uartprintcstr("Test_page_free_list_ext failed!\n");
	else
		uartprintcstr("Test_page_free_list_ext failed!\n");

	success = test_page_alloc();
	if(!success)
		uartprintcstr("Test_page_alloc failed!\n");
	else
		uartprintcstr("Test_page_alloc succeeded!\n");

  userinit();      // first user process
  mpmain();        // finish this processor's setup
}

// Other CPUs jump here from entryother.S.
static void
mpenter(void)
{
  switchkvm();
  seginit();
  lapicinit();
  mpmain();
}

// Common CPU setup code.
static void
mpmain(void)
{
  cprintf("cpu%d: starting\n", cpunum());
  idtinit();       // load idt register
  xchg(&cpu->started, 1); // tell startothers() we're up
#if LAB == 1
// Test the stack back trace
  test_backtrace(5);

  while (1)
    monitor(0);
#else
   scheduler();     // start running processes
#endif
}

pde_t entrypgdir[];  // For entry.S

// Start the non-boot (AP) processors.
static void
startothers(void)
{
  extern uchar _binary_entryother_start[], _binary_entryother_size[];
  uchar *code;
  struct cpu *c;
  char *stack;

  // Write entry code to unused memory at 0x7000.
  // The linker has placed the image of entryother.S in
  // _binary_entryother_start.
  code = P2V(0x7000);
  memmove(code, _binary_entryother_start, (uint)_binary_entryother_size);

  for(c = cpus; c < cpus+ncpu; c++){
    if(c == cpus+cpunum())  // We've started already.
      continue;

    // Tell entryother.S what stack to use, where to enter, and what
    // pgdir to use. We cannot use kpgdir yet, because the AP processor
    // is running in low  memory, so we use entrypgdir for the APs too.
    stack = kalloc();
    *(void**)(code-4) = stack + KSTACKSIZE;
    *(void**)(code-8) = mpenter;
    *(int**)(code-12) = (void *) V2P(entrypgdir);

    lapicstartap(c->apicid, V2P(code));

    // wait for cpu to finish mpmain()
    while(c->started == 0)
      ;
  }
}

// The boot page table used in entry.S and entryother.S.
// Page directories (and page tables) must start on page boundaries,
// hence the __aligned__ attribute.
// PTE_PS in a page directory entry enables 4Mbyte pages.

__attribute__((__aligned__(PGSIZE)))
pde_t entrypgdir[NPDENTRIES] = {
  // Map VA's [0, 4MB) to PA's [0, 4MB)
  [0] = (0) | PTE_P | PTE_W | PTE_PS,
  // Map VA's [KERNBASE, KERNBASE+4MB) to PA's [0, 4MB)
  [KERNBASE>>PDXSHIFT] = (0) | PTE_P | PTE_W | PTE_PS,
};

//PAGEBREAK!
// Blank page.
//PAGEBREAK!
