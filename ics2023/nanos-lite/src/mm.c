#include <memory.h>
#include <stdint.h>
#include <proc.h>
static void *pf = NULL;

void* new_page(size_t nr_page) {
  void *npf = pf;
  pf = (void *)((char *)pf + nr_page * PGSIZE);
  //memset(npf,0,nr_page*PGSIZE);
  return npf;
}

#ifdef HAS_VME
//#define PGSIZE 4096
static void* pg_alloc(int n) {
  void *newpage = new_page(n / PGSIZE);
  memset(newpage, 0 , n);
  return newpage;
}
#endif

void free_page(void *p) {
  panic("not implement yet");
}

extern PCB *current;
/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {
  //printf("mmbrk\n");
  // less return 0;
  if (brk <= current->max_brk) {
    //printf("1:%p\n",current->max_brk);
    return 0;
  }
  // need more
  else {
    //printf("jijijiji\n");
    //printf("cur: %d\n",current);
    current->max_brk = brk;
    uintptr_t re_brk = current->max_brk;
    while(brk > re_brk){
      //printf("jijijiji\n");
      //printf("brk: %x\n",re_brk);
      map(&(current->as), (char*)re_brk, pg_alloc(PGSIZE), 0b111);
      re_brk+=PGSIZE;
    }
    current->max_brk = re_brk;
    //printf("current->max_brk: %x\n",current->max_brk);
    return 0;
  }
  //return 0;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
