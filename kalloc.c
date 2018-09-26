// Physical memory allocator, intended to allocate
// memory for user processes, kernel stacks, page table pages,
// and pipe buffers. Allocates 4096-byte pages.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"
#define NULL 0

void freerange(void *vstart, void *vend);
extern char end[]; // first address after kernel loaded from ELF file
int free_page_count;

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  int use_lock;
  struct run *freelist;
} kmem;

<<<<<<< HEAD
struct run **free_ptr;

struct page_info phys_page_info[PHYSPAGES];

void
kdecref(struct page_info *p) {
    if(--(p->ref_count) == 0)
        kfree(p->v);
}
=======
struct page_info {
    char *v;
    int ref_count;
    int flags;
    struct page_info *next;
};
>>>>>>> af61a49c22a93a1f384294dc8138f5f3702f4594

struct page_info ppage_info[1024];

void
kdecref(struct page_info *p) {
    if(--(p->ref_count) == 0)
        kfree(p->v);
}

int
kinsert(pde_t *pgdir, struct page_info *pp, char *va, int perm)
{

	return 0; //Placeholder so the empty function will compile
}

void
kremove(pde_t *pgdir, void *va)
{

}

struct page_info *
klookup(pde_t *pgdir, void *va, pte_t **pte_store)
{

	return 0;
}


// Initialization happens in two phases.
// 1. main() calls kinit1() while still using entrypgdir to place just
// the pages mapped by entrypgdir on free list.
// 2. main() calls kinit2() with the rest of the physical pages
// after installing a full page table that maps them on all cores.
void
kinit1(void *vstart, void *vend)
{
  free_page_count = 0;
  initlock(&kmem.lock, "kmem");
  kmem.use_lock = 0;
  freerange(vstart, vend);

  free_ptr = &kmem.freelist;
  //i/o space
  for(uint i=0; i<EXTMEM; i+=PGSIZE) {
    struct page_info p_info;
    p_info.v = P2V(i);
    p_info.ref_count = 1;
    p_info.used = 1;
    p_info.next = NULL;
    phys_page_info[i >> 12] = p_info;
  }
  //more devices
  for(uint i=DEVSPACE; i<0; i+=PGSIZE) {
    struct page_info p_info;
    p_info.v = P2V(i);
    p_info.ref_count = 1;
    p_info.used = 1;
    p_info.next = NULL;
    phys_page_info[i >> 12] = p_info;
  }
  //kernel instr and r/o data
  for(uint i=V2P(KERNLINK); i<V2P(end); i+=PGSIZE) {
    struct page_info p_info;
    p_info.v = P2V(i);
    p_info.ref_count = 1;
    p_info.used = 1;
    p_info.next = NULL;
    phys_page_info[i >> 12] = p_info;
  }

}

void
kinit2(void *vstart, void *vend)
{
  freerange(vstart, vend);
  kmem.use_lock = 1;
}

void
freerange(void *vstart, void *vend)
{
  char *p = (char*)PGROUNDUP((uint)vstart);
  struct page_info *p_info_next = NULL;
  for(; p + PGSIZE <= (char*)vend; p += PGSIZE) {
    kfree(p);

    struct page_info p_info;
    p_info.v = p;
    p_info.ref_count = 0;
    p_info.used = 0;
    p_info.next = p_info_next;
    phys_page_info[V2P(p) >> 12] = p_info;
    p_info_next = &phys_page_info[V2P(p) >> 12];
    free_page_count++;
  }
}

//PAGEBREAK: 21
// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(char *v)
{
  struct run *r;

  if((uint)v % PGSIZE || v < end || V2P(v) >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  //memset(v, 1, PGSIZE); //Commented out for the lab2 exercise

  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = (struct run*)v;
  r->next = kmem.freelist;
  kmem.freelist = r;
  if(kmem.use_lock)
    release(&kmem.lock);

  phys_page_info[V2P(v) >> 12].v = v;
  phys_page_info[V2P(v) >> 12].used = 0;
  phys_page_info[V2P(v) >> 12].next = &phys_page_info[V2P(r->next)];
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
char*
kalloc(void)
{
  struct run *r;

  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  if(kmem.use_lock)
    release(&kmem.lock);
  phys_page_info[V2P((char*)r) >> 12].used = 1;
  phys_page_info[V2P((char*)r) >> 12].next = NULL;
  return (char*)r;
}
