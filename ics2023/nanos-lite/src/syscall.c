#include <common.h>
#include "syscall.h"
#include "proc.h"

//#define SYSCALL_TRACE
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_RESET "\x1b[0m"

size_t fs_write(int fd, const void *buf, size_t len);
int mm_brk(uintptr_t brk);

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  //printf("a2: %p\n",c->GPR2);
  switch (a[0]) {                                                                                        
    case SYS_yield: 
      #ifdef SYSCALL_TRACE
        printf(ANSI_COLOR_BLUE "====== Do SYS_yield now! ======\n" ANSI_COLOR_RESET);
      #endif
      //printf("ttttt\n");
      yield(); 
      c->GPRx = 0; 
      break;
    
    case SYS_exit: 
      #ifdef SYSCALL_TRACE
        printf(ANSI_COLOR_BLUE "====== Do SYS_exit now! ======\n" ANSI_COLOR_RESET);
      #endif
      //printf("sys-exit\n");
      //halt(c->gpr[17]);
      //c->GPRx = execve("/bin/menu", NULL, NULL);
      halt(a[1]);
      break;
    
    case SYS_write:
      #ifdef SYSCALL_TRACE
        printf(ANSI_COLOR_BLUE "====== Do SYS_write now! ======\n" ANSI_COLOR_RESET);
      #endif
      c->GPRx = fs_write(c->GPR2, (void *)c->GPR3, c->GPR4);
      break;

    case SYS_brk:
      #ifdef SYSCALL_TRACE
        printf(ANSI_COLOR_BLUE "====== Do SYS_brk now! ======\n" ANSI_COLOR_RESET);
      #endif
      //printf("sysbrk\n");
      //printf("gpr2: %x\n",(uintptr_t)c->GPR2);
      c->GPRx = mm_brk((uintptr_t)c->GPR2);
      c->GPRx = 0;
      break;

    case SYS_open:
      #ifdef SYSCALL_TRACE
        printf(ANSI_COLOR_BLUE "====== Do SYS_open now! ======\n" ANSI_COLOR_RESET);
      #endif
      c->GPRx = fs_open((const char *)c->GPR2, c->GPR3, c->GPR4);
      break;

    case SYS_read:
      #ifdef SYSCALL_TRACE
        printf(ANSI_COLOR_BLUE "====== Do SYS_read now! ======\n" ANSI_COLOR_RESET);
      #endif
      c->GPRx = fs_read(c->GPR2, (void *)c->GPR3, c->GPR4);
      break;

    case SYS_lseek:
      #ifdef SYSCALL_TRACE
        printf(ANSI_COLOR_BLUE "====== Do SYS_lseek now! ======\n" ANSI_COLOR_RESET);
      #endif
      c->GPRx = fs_lseek(c->GPR2, c->GPR3, c->GPR4);
      break;

    case SYS_close:
      #ifdef SYSCALL_TRACE
        printf(ANSI_COLOR_BLUE "====== Do SYS_close now! ======\n" ANSI_COLOR_RESET);
      #endif
      c->GPRx = fs_close(c->GPR2);
      break;

    case SYS_gettimeofday:
      #ifdef SYSCALL_TRACE
        printf(ANSI_COLOR_BLUE "====== Do SYS_gettimeofday now! ======\n" ANSI_COLOR_RESET);
      #endif
      AM_TIMER_UPTIME_T time = io_read(AM_TIMER_UPTIME); 
      c->GPRx = time.us;
      break;

    case SYS_execve:
      #ifdef SYSCALL_TRACE
        printf(ANSI_COLOR_BLUE "====== Do SYS_execve now! ======\n" ANSI_COLOR_RESET);
      #endif
      //printf("yes!\n");
      c->GPRx=execve((char*)c->GPR2,(char**)c->GPR3,(char**)c->GPR4);
      break;

    default: 
      panic("1111Unhandled syscall ID = %d", a[0]);
  }
}
