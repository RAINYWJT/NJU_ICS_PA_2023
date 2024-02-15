#include <common.h>

static Context* do_event(Event e, Context* c) {
  //printf(" \n");
  //printf("eventnum:%d\n",e.event);
  switch (e.event) {
    //????if no break there, we need to add something//////////////////////////////
    case EVENT_YIELD: /*printf("EVENT_YIELD\n");*/return schedule(c);break;
    case EVENT_IRQ_TIMER:/* printf("jijij\n")*/Log("Recieved!\n");c=schedule(c);break;
    case EVENT_SYSCALL:/* printf("EVENT_SYSCALL\n");*/do_syscall(c); break;  
    case EVENT_ERROR:  do_syscall(c); /*printf("1\n");*/ break;
    default: panic("irqUnhandled event ID = %d", e.event);break;
  }
  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
