#include <am.h>
#include <nemu.h>
#include <klib.h>

static AddrSpace kas = {};
static void* (*pgalloc_usr)(int) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;

static Area segments[] = {      // Kernel memory mappings
  NEMU_PADDR_SPACE
};

#define USER_SPACE RANGE(0x40000000, 0x80000000)

static inline void set_satp(void *pdir) {
  //printf("pair in set:%p\n", pdir);
  uintptr_t mode = 1ul << (__riscv_xlen - 1);
  //printf("mode: %p\n", mode);
  asm volatile("csrw satp, %0" : : "r"(mode | ((uintptr_t)pdir >> 12)));
}

static inline uintptr_t get_satp() {
  uintptr_t satp;
  //printf("satp:%x\n", satp);
  asm volatile("csrr %0, satp" : "=r"(satp));
  return satp << 12;
}

bool vme_init(void* (*pgalloc_f)(int), void (*pgfree_f)(void*)) {
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  kas.ptr = pgalloc_f(PGSIZE);

  int i;
  for (i = 0; i < LENGTH(segments); i ++) {
    void *va = segments[i].start;
    for (; va < segments[i].end; va += PGSIZE) {
      map(&kas, va, va, 0);
    }
  }

  set_satp(kas.ptr);
  vme_enable = 1;

  return true;
}

void protect(AddrSpace *as) {
  PTE *updir = (PTE*)(pgalloc_usr(PGSIZE));
  as->ptr = updir;
  as->area = USER_SPACE;
  as->pgsize = PGSIZE;
  // map kernel space
  memcpy(updir, kas.ptr, PGSIZE);
}

void unprotect(AddrSpace *as) {
}

void __am_get_cur_as(Context *c) {
  //printf("jijiji\n");
  //printf("vme_enable:%d\n",vme_enable);
  c->pdir = (vme_enable ? (void *)get_satp() : NULL);
}

void __am_switch(Context *c) {
  if (vme_enable && c->pdir != NULL) {
    //printf("switch!\n");
    //printf("c->pdir: %p\n", c->pdir);
    set_satp(c->pdir);
  }
}

typedef struct {
  uintptr_t pte; 
  uintptr_t p0_add; 
} PTEInfo;

PTEInfo get_pte(AddrSpace *as, void *va, void *pa)
{
#define v1 ((uintptr_t)va & 0xffc00000)
#define v0 ((uintptr_t)va & 0x003ff000)
#define p1 ((uintptr_t)pa & 0xffc00000)
#define p0 ((uintptr_t)pa & 0x003ff000)

  PTEInfo pi;
  uintptr_t pdir = (uintptr_t)(as->ptr)>>12;
  uintptr_t p1_add = (pdir) *PGSIZE + (v1 >> 22) *4;
  if (((uintptr_t)va & 0xfff) != 0 || ((uintptr_t)pa & 0xfff) != 0) {
    assert(0);
  }
  if(*(uintptr_t*)p1_add == 0){
    pi.p0_add = (uintptr_t) pgalloc_usr(PGSIZE);
    *(uintptr_t*)p1_add = ((pi.p0_add & 0xfffff000) >> 2) |PTE_V;
  }
  else{
    uintptr_t pte = ((*(uintptr_t*)p1_add) & 0xfffffc00)>>10;
    pi.p0_add = pte *PGSIZE;
  }
  pi.p0_add += (v0 >> 12)*4;
  pi.pte = (p1>>2)|(p0 >>2)|PTE_X|PTE_W|PTE_R|PTE_V;
  if (pi.pte & PTE_R) {
    //printf("jiji\n");// pi.pte 可写
  }
  else{
    assert(0);
  } 
  return pi;
}
/**/
void map(AddrSpace *as, void *va, void *pa, int prot)
{
  //printf("map va pa: %p %p \n",va,pa);
  PTEInfo pi = get_pte(as, va, pa);
  *(uintptr_t*)pi.p0_add = pi.pte;
  //printf("1: %p\n", pi.p0_add);
  //printf("va: %p\n",va);
  //printf("pa: %p\n",pa);
  //if(&p0_add - 4096 <0x8216fcfc){
  //  assert(0);
  //}
  //printf("map!\n");
  //pt_2[vaddr->pte_index].prot = prot;
  //printf("jijijijibabybaby\n");
  /*
  for (uintptr_t vaddr = 0x4000c000; vaddr < 0x4000d000; vaddr += 4) {
    if (vaddr >= (uintptr_t)va && vaddr < (uintptr_t)va + PGSIZE) {
      int offset = (vaddr - (uintptr_t)va) / 4;
      uintptr_t content = ((uintptr_t*)pi.p0_add)[offset];
      printf("vaddr: %p, content: %p\n", vaddr, content);
    }
  }*/
}

//////////////////////////////////////////////////////////////////////

Context *ucontext(AddrSpace *as, Area kstack, void *entry) {
  char* end = kstack.end - sizeof(Context);
  Context* c = (Context*)end;
  //printf("sp: %p\n",sp);
  //c->gpr[2] = sp;
  c->mepc = (uintptr_t)entry;
  //printf("c->mepc:%p\n", c->mepc);
  c->mstatus = 0x1800 | 0x80 ;
  //printf("jijij\n");
  //printf("as:%p\n",as);  
  c->pdir = as->ptr;
  c->np = 1;
  //printf("jijij\n");
  //printf("c->pdir:%p\n", c->pdir);
  //printf("yes1\n");
  return c;
}
