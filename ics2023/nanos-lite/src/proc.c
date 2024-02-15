#include <proc.h>

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite with arg '%p' for the %dth time!\n", (uintptr_t)arg, j);
    j ++;
    yield();
  }
}

int execve(const char *filename, char *const argv[], char *const envp[]){
  int fp = fs_open(filename,0 ,0);
  if( fp == -1){
    panic("NO FP WORKS!");
    return -1;
  }
  fs_close(fp);
  context_uload(current, (char*)filename, argv, envp);
  //printf("jijiijbaybay!\n");
  switch_boot_pcb();
  //printf("cur11:%p\n",current);
  //printf("jijiji\n");
  yield(); // wrong
  //printf("jiji\n");
  return 0;
}

void init_proc() {
  switch_boot_pcb();

  Log("Initializing processes...");

  context_kload(&pcb[0], hello_fun, "hello world");
  //printf("ji\n");
  
  //char *argv[] = {"--skip", NULL};
  //printf("argv1:%s\n",argv[0]);
  //context_uload(&pcb[1], "/bin/pal", argv , NULL);
  char *argv[] = {NULL,"--skip"};
  char *envp[] = {NULL};
  //context_uload(&pcb[0], "/bin/exec-test", argv, envp);
  //context_uload(&pcb[0], "/bin/hello", argv, envp);
  context_uload(&pcb[1], "/bin/pal", argv, envp);
  switch_boot_pcb();
  //assert(0);
  // load program here
  //naive_uload(NULL,"/bin/event-test");
  //naive_uload(NULL,"/bin/bird");
  //naive_uload(NULL,"/bin/nterm");
}

Context* schedule(Context *prev) {
  // save the context pointer
  //printf("sch!\n");
  current->cp = prev;

  // switch between pcb[0] and pcb[1]
  current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);
  //current = &pcb[0];
  //printf("jijij\n");
  // then return the new context
  //printf("cur->cp: %p\n",current->cp);
  return current->cp;
}
