#include <proc.h>
#include <elf.h>
#include <common.h>
#include <fs.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

#define PAGE_MASK (~(PGSIZE - 1))

int strlen_instead(char *args[]) {
    int num = 0;
    char **p = args; 
    if (p) {
        while (*p) { 
            num++; 
            p++;
        }
    }
    return num;
}


static uintptr_t loader(PCB *pcb, const char *filename) {
  #ifndef HAS_VME
  Elf_Ehdr ehdr;
  int filed = fs_open(filename, 0, 0);
  fs_read(filed, &ehdr, sizeof(Elf_Ehdr));
  assert(*(uint32_t *)ehdr.e_ident == 0x464c457f);
  Elf_Phdr phdr[ehdr.e_phnum];
  fs_lseek(filed, ehdr.e_phoff, SEEK_SET);
  fs_read(filed, phdr, sizeof(Elf_Phdr) * ehdr.e_phnum);
  for(int i = 0; i < ehdr.e_phnum; i++){
    if(phdr[i].p_type == PT_LOAD){
      fs_lseek(filed, phdr[i].p_offset, SEEK_SET);
      fs_read(filed, (void*)phdr[i].p_vaddr, phdr[i].p_memsz);
      memset((void*)(phdr[i].p_vaddr + phdr[i].p_filesz), 0, phdr[i].p_memsz - phdr[i].p_filesz);
    }
  }
  fs_close(filed);
  //printf("1111\n");
  //printf("entry: %p\n",ehdr.e_entry);
  return ehdr.e_entry;
  #else
  /* version 3 */
  Elf_Ehdr ehdr;
  char *va,*pg;
  uintptr_t pg_str;
  size_t filed = fs_open(filename, 0, 0);
  //printf("sizeehdr:%d \n",sizeof(Elf_Ehdr));
  fs_read(filed , &ehdr, sizeof(Elf_Ehdr));
  assert(*(uint32_t *)ehdr.e_ident == 0x464c457f);
  Elf_Phdr phdr;
  //printf("%d\n",ehdr.e_phnum);
  for(int i = 0; i < ehdr.e_phnum; i++){
    fs_lseek(filed, ehdr.e_phoff+ i*ehdr.e_phentsize, 0);
    fs_read(filed, &phdr, ehdr.e_phentsize);
    if(phdr.p_type == PT_LOAD){
      va = (char*)(phdr.p_vaddr & 0xfffff000);
      pg_str = 0;
      //printf("phdr.p_offset: %p \n",phdr.p_offset);
      fs_lseek(filed , phdr.p_offset, 0);
        //printf("pcb->as: %p\n",(pcb->as));
        while((uintptr_t)va <= phdr.p_vaddr+phdr.p_memsz){
          pg = new_page(1);
          //assert(&(pcb->as));
          //printf("jijijijijiji\n");
          //printf("loader va pg: %p %p\n",va, pg);
          map(&(pcb->as), va, pg, 0b111);
          //printf("jijijijijijijijijjijijijijiji\n");
          fs_read(filed, pg+ (phdr.p_vaddr & 0xfff), PGSIZE);
          va += PGSIZE;
          if(!pg_str){
            pg_str = (uintptr_t)pg;
            //printf("pgstr: %p\n",pg_str);
          }
        }
        //printf("loader 1: %p %p\n",(char*)(pg_str+(phdr.p_vaddr&0xfff)+phdr.p_filesz), phdr.p_memsz-phdr.p_filesz);
        memset((char*)(pg_str+(phdr.p_vaddr&0xfff)+phdr.p_filesz),0, phdr.p_memsz-phdr.p_filesz);
        pcb->max_brk = (uintptr_t)va;
    }
  }
  fs_close(filed);
  //printf("1111\n");
  //printf("entry: %p\n",ehdr.e_entry);
  return ehdr.e_entry;
  #endif
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void (*entry)(void *),void* args){
  /*调用kcontext()来创建上下文, 并把返回的指针记录到PCB的cp中*/
  Area kspace=RANGE(&pcb->cp, &pcb->cp+STACK_SIZE);
  //printf("kjiji\n");
  pcb->cp = kcontext(kspace, entry, args);
  //printf("kjijiijijii\n");
} 

#define PGSIZE 4096

void context_uload(PCB *proc, const char *filename, char *const argv[], char *const envp[]) {
  uint32_t heap_base, stack_top;
  heap_base = (uint32_t)new_page(8);  
  protect(&proc->as);
  for(int i = 0; i < 8; i++) {
    void* paddr = (void*)(heap_base + i * PGSIZE);
    map(&proc->as, (void *)((uint32_t)proc->as.area.end - (8 - i) * PGSIZE), paddr, 0);
  }
  stack_top = heap_base + STACK_SIZE;               
  if(argv != NULL && envp != NULL) {                         
    int num_env, num_arg;
    char *env_ptr[8], *arg_ptr[8];
    // copy envp and argv to stack
    for(num_env = 0; envp[num_env] != 0; num_env++) {
      env_ptr[num_env] = (char *)stack_top;
      stack_top -= strlen(envp[num_env]) + 1;
      memcpy((void *)stack_top, (void *)(envp[num_env]), strlen(envp[num_env]) + 1);
    }
    env_ptr[num_env] = 0;                   
    for(num_arg = 0; argv[num_arg] != 0; num_arg++) {
      arg_ptr[num_arg] = (char *)stack_top;
      stack_top -= strlen(argv[num_arg]) + 1;
      memcpy((void *)stack_top, (void *)(argv[num_arg]), strlen(argv[num_arg]) + 1);
    }
    arg_ptr[num_arg] = 0;                  
    // align stack pointer
    stack_top -= stack_top % 4;                      
    // copy env_ptr and arg_ptr to stack
    stack_top -= (num_env + 1) * sizeof(uint32_t);
    memcpy((void *)stack_top, (void *)env_ptr, sizeof(uint32_t) * (num_env + 1));
    stack_top -= (num_arg + 1) * sizeof(uint32_t);
    memcpy((void *)stack_top, (void *)arg_ptr, sizeof(uint32_t) * (num_arg + 1));
    // copy num_arg to stack
    stack_top -= sizeof(uint32_t);
    memcpy((void *)stack_top, (void *)&num_arg, sizeof(uint32_t));
  }
  AddrSpace *as = &proc->as;
  Area stack_area;
  stack_area.start = proc->stack;
  stack_area.end = stack_area.start + sizeof(proc->stack);
  assert(filename != NULL);
  uintptr_t entry_point = loader(proc, filename);
  proc->cp = ucontext(as, stack_area, (void *)entry_point);   
  proc->cp->gpr[10] = (uintptr_t)stack_top;
}


/*
void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]){
  uintptr_t entry = loader(pcb, filename);
  Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);
  pcb->cp = ucontext(&pcb->as, stack, (void *)entry);
}
*/ 
  /*
  Elf_Ehdr ehdr;
  init_ramdisk();
  //printf("yes!\n");
  ramdisk_read(&ehdr , 0 ,get_ramdisk_size());
  //printf("yes1!\n");
  assert(*(uint32_t *)ehdr.e_ident == 0x464c457f);
  //printf("yes2!\n");
  for(int i=0; i<ehdr.e_phnum; i++){
    Elf_Phdr phdr;
    ramdisk_read(&phdr, ehdr.e_phoff + i * sizeof(Elf_Phdr), sizeof(Elf_Phdr));
    if(phdr.p_type == PT_LOAD){
      ramdisk_read((void*)phdr.p_vaddr, phdr.p_offset, phdr.p_filesz);
      memset((void*)(phdr.p_vaddr + phdr.p_filesz), 0 ,phdr.p_memsz - phdr.p_filesz);
    }
  }
  return ehdr.e_entry;
  */
