#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

static Context* (*user_handler)(Event, Context*) = NULL;
void __am_get_cur_as(Context *c);
void __am_switch(Context* c);

Context* __am_irq_handle(Context *c) {
  //printf(" \n");
  //printf("cin : %p\n",c);
  __am_get_cur_as(c);
  //printf("1:mcacuse = %x mstatus = %x mepc = %p\n", c->mcause , c->mstatus , c->mepc);
  if (user_handler) {
    Event ev = {0};
    //printf("2:mcacuse = %x mstatus = %x mepc = %p\n", c->mcause , c->mstatus , c->mepc);
    switch (c->mcause) {
      case  1 : ev.event = EVENT_SYSCALL; break;
      case -1 : ev.event = EVENT_YIELD ; break;
      case 0X80000007: ev.event = EVENT_IRQ_TIMER ;break;
      default: ev.event = EVENT_ERROR; break;
    }
    c = user_handler(ev, c);
    assert(c != NULL);
  }
  __am_switch(c);
  //printf("c : %p\n",c);
  //printf("\n");
  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));

  // register event handler
  user_handler = handler;

  return true;
}


Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  /*??????*/
  char* start = kstack.end - sizeof(Context);
  Context* c = (Context*)start;
  c->mepc = (uintptr_t)entry;
  c->mstatus = 0x1800 | 0x80;
  //c->gpr[10]=(uintptr_t)start;
  c->GPRx=(uintptr_t)arg;
  c->gpr[2] =(uintptr_t)kstack.end;
  //printf("yes1\n");
  c->pdir = NULL;
  c->np = 0;
  return c;
}

void yield() {
#ifdef __riscv_e
  asm volatile("li a5, -1; ecall");
#else
  asm volatile("li a7, -1; ecall");
#endif
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
