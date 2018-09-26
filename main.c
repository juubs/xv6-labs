#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "monitor.h"

#define LAB 2
#define NULL 0

static void startothers(void);
static void mpmain(void)  __attribute__((noreturn));
extern pde_t *kpgdir;
extern struct run *freelist;
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
    struct page_info *pp;
    for(char *v = (char*)end; v <= (char*)P2V(4*1024*1024); v += PGSIZE) {
        pp = &phys_page_info[V2P(v) >> 12];
        //cprintf("%02x\n", pp->v);
    }

	//Check that the pages that should not be free are not on the list of free pages
    for(uint p = 0; p < EXTMEM; p += PGSIZE) {
        if (phys_page_info[p >> 12].used == 0) {
            cprintf("Forbidden space not set properly.\n");
            return 0; 
        }
    }
	
    //Assert that the first part of physical memory have been mapped to free pages
    for(char *p = (char*)end; p < (char*)(4*1024*1024); p += PGSIZE) {
        if (phys_page_info[(uint)p >> 12].used != 0) {
            cprintf("Pages not freed properly.\n");
            return 0; 
        }
    }

	return 1; //Success
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
    int free_page_count = 0;
    for(uint p=0; p<PHYSTOP; p+=PGSIZE) {
        if (!phys_page_info[p >> 12].used) free_page_count++;
    }
    cprintf("%d free pages\n", free_page_count);
        
	//Allocate a few pages with kalloc
    char *p1 = kalloc();
    char *p2 = kalloc();
    char *p3 = kalloc();
    cprintf("Virtual addresses from kalloc:\np1: 0x%08x\np2: 0x%08x\np3: 0x%08x\n", p1, p2, p3);

	//Assert all pages are different
    if (p1 == p2 || p2 == p3 || p1 == p3) return 0;

	//Assert that the physical addresses are within expected bounds
    cprintf("Physical addresses of p1-p3:\np1: 0x%08x\np2: 0x%08x\np3: 0x%08x\n", V2P(p1), V2P(p2), V2P(p3));

	//Disable the freelist by saving it to a temporary variable and set freelist to null
    //struct run **temp_free = free_ptr;
    uint fp = (uint)*free_ptr;
    cprintf("Freelist before disable:\ntemp: 0x%08x\nreal: 0x%08x\n", fp, *free_ptr);
    *free_ptr = NULL;
    
    free_page_count = 0;
    for(uint p=0; p<PHYSTOP; p+=PGSIZE) {
        if (!phys_page_info[p >> 12].used) free_page_count++;
    }
    cprintf("%d free pages\n", free_page_count);

	//Assert kalloc returns 0 (null)
    char *p4 = kalloc();
    cprintf("freelist disabled\np4: 0x%08x\n", p4);

	//Free pages allocated in second commment
    kfree(p1);
    kfree(p2);
    kfree(p3);
    cprintf("freed p1-p3\n");

	//Reallocate pages, assert they are reallocated in reverse order
    p1 = kalloc();
    p2 = kalloc();
    p3 = kalloc();
    cprintf("Re-kalloc'd p1-p3:\np1: 0x%08x\np2: 0x%08x\np3: 0x%08x\n", p1, p2, p3);

	//Assert that once all pages are reallocated, kalloc again returns 0
    char *p5 = kalloc();
    cprintf("New kalloc\np5: 0x%08x\n", p5);

	//Set one page to known junk values
    *p1 = (char)7234432772459894985;
    cprintf("junk: %d\n", *p1);

	//Free the page, reallocate it.  Assert that the page is the same one with the same junk values.
    kfree(p1);
    cprintf("freed junk\n");
    p1 = kalloc();
    cprintf("p1 realloc'd: 0x%08x\n", p1);
    cprintf("realloced junk: %d\n", *p1);

	//Restore the page free list saved to the temporary variable in fifth step.  Free the pages allocated in this test.
    *free_ptr = (struct run *)fp;
    cprintf("Temp freelist: 0x%08x\n", fp);
    cprintf("Restored freelist: 0x%08x\n", *free_ptr);
    kfree(p1);
    kfree(p2);
    kfree(p3);
	
    //Assert the number of free pages is the same as in the beginning.
    free_page_count = 0;
    for(uint p=0; p<PHYSTOP; p+=PGSIZE) {
        if (!phys_page_info[p >> 12].used) free_page_count++;
    }
    cprintf("%d free pages\n", free_page_count);

	return 1;
}

int
test_page()
{
	//Allocate a few pages p1, p2, p3

	//Assert that they are all non-zero and different from one another

	//Save the free page list to a temporary variable.  Set free page list to zero.

	//Assert that kookup(0x0) == 0

	//Assert that you can not allocate a page table with kinsert

	//Free page p1, kinsert the physical page p2 at 0x0. Assert the operation succeeded.

	//Assert that p1 is the page table from the previous step.  Assert p2 is in that page table.

	//Asset that p1 and p2 have a ref count of 1.

	//Kinsert p3 at 0x1000.

	//Assert that p3 is also in the page table stored at p1.  Assert p3 has a ref count of 1.

	//Reinsert p3 at 0x1000.

	//Assert that p3 is still in the page table stored at p1.  Assert p3 still has a reference count of 1.

	//Assert you can no longer allocate any more pages.

	//Change the permissions on the pages with kinsert. Assert permissions were changed correctly.

	//Do a remap with fewer permissions on the pages with kinsert.  Assert permissions were changed correctly.

	//Try to remap at a place where kinsert will fail because it will need to allocate another page table.  

	//Insert a different page, e.g. p2 at 0x1000.

	//Check that physical page p2 is mapped in two places and verify that the reference count for p2 is 2 and for p3 is 0

	//Assert that kalloc returns p3.

	//kremove the reference to p2 at 0x0. Assert p2 is still mapped at 0x1000.

	//Assert that the reference count to p2 has been decremented to 1.

	//Reinsert p2 at 0x1000 and assert that the reference count is still 1.

	//Remove the mapping of p2, verify that it is freed.  Assert that when you kallocate you get it back.

	//Free the pages and restore the free list.

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
	success ? uartprintcstr("Test_page_free_list succeeded!\n") : uartprintcstr("Test_page_free_list failed!\n");

  mpinit();        // detect other processors
  lapicinit();     // interrupt controller
  seginit();       // segment descriptors
  cprintf("\ncpu%d: starting xv6\n\n", cpunum());
  picinit();       // another interrupt controller
  ioapicinit();    // another interrupt controller
  consoleinit();   // console hardware
  uartinit();      // serial port (Have to call it twice to get interrupt output)

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
	success ? uartprintcstr("Test_page_free_list_ext succeded!\n") : uartprintcstr("Test_page_free_list_ext failed!\n");

	success = test_page_alloc();
	success ? uartprintcstr("Test_page_alloc succeeded!\n") : uartprintcstr("Test_page_alloc failed!\n");

	success = test_page();
	success ? uartprintcstr("Test_page succeeded!\n") : uartprintcstr("Test_page failed!\n");

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
