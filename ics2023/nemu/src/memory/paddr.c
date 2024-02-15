/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <memory/host.h>
#include <memory/paddr.h>
#include <device/mmio.h>
#include <isa.h>

#ifdef CONFIG_MTRACE_COND
static char space[128];
#endif

#if   defined(CONFIG_PMEM_MALLOC)
static uint8_t *pmem = NULL;
#else // CONFIG_PMEM_GARRAY
static uint8_t pmem[CONFIG_MSIZE] PG_ALIGN = {};
#endif

uint8_t* guest_to_host(paddr_t paddr) { return pmem + paddr - CONFIG_MBASE; }
paddr_t host_to_guest(uint8_t *haddr) { return haddr - pmem + CONFIG_MBASE; }

static word_t pmem_read(paddr_t addr, int len) {
  word_t ret = host_read(guest_to_host(addr), len);
  //printf("ret %x\n",ret);
  return ret;
}

static void pmem_write(paddr_t addr, int len, word_t data) {
  host_write(guest_to_host(addr), len, data);
}

/*
static void out_of_bound(paddr_t addr) {
  panic("address = " FMT_PADDR " is out of bound of pmem [" FMT_PADDR ", " FMT_PADDR "] at pc = " FMT_WORD,
      addr, PMEM_LEFT, PMEM_RIGHT, cpu.pc);
      return ;
}*/

void init_mem() {
#if   defined(CONFIG_PMEM_MALLOC)
  pmem = malloc(CONFIG_MSIZE);
  assert(pmem);
#endif
#ifdef CONFIG_MEM_RANDOM
  uint32_t *p = (uint32_t *)pmem;
  int i;
  for (i = 0; i < (int) (CONFIG_MSIZE / sizeof(p[0])); i ++) {
    p[i] = rand();
  }
#endif
  Log("physical memory area [" FMT_PADDR ", " FMT_PADDR "]", PMEM_LEFT, PMEM_RIGHT);
}

#ifdef  CONFIG_MTRACE
static void m_trace(paddr_t addr,int len,word_t res){
  snprintf(space,sizeof(space),FMT_WORD"%2d",addr,len);
  uint8_t*res_p = (uint8_t*)&res;
  for (int i = len - 1; i >= 0; i--) {
    printf(" ");
    snprintf(space + strlen(space), sizeof(space) - strlen(space), " %02x", res_p[i]);
  }
  #ifdef CONFIG_MTRACE_COND
  if (MTRACE_COND) { log_write("%s\n", space); }
#endif
  puts(space);
}
#endif

word_t paddr_read(paddr_t addr, int len) {
  if (likely(in_pmem(addr))) {
    //printf("jiji\n");
  #ifdef CONFIG_MTRACE
    printf("%s",ANSI_FMT("There are read:", ANSI_FG_GREEN));
    word_t res = pmem_read(addr,len);
    m_trace(addr,len,res);
    return res;
  #else
      //printf("jiji\n");
    return pmem_read(addr, len); 
  #endif
  }
  IFDEF(CONFIG_DEVICE, return mmio_read(addr, len));
  #ifdef CONFIG_MTRACE
    m_trace(addr,len,-1);
  #endif
  printf("address = " FMT_PADDR " is out of bound of pmem [" FMT_PADDR ", " FMT_PADDR "] at pc = " FMT_WORD,
      addr, PMEM_LEFT, PMEM_RIGHT, cpu.pc);
  //out_of_bound(addr);
  printf(",please again!\n");
  return 0;
}

void paddr_write(paddr_t addr, int len, word_t data) {
  #ifdef CONFIG_MTRACE
    printf("%s",ANSI_FMT("There is write:", ANSI_FG_GREEN));
    m_trace(addr, len, data);
  #endif
  if (likely(in_pmem(addr))) { pmem_write(addr, len, data); return; }
  IFDEF(CONFIG_DEVICE, mmio_write(addr, len, data); return);
  printf("address = " FMT_PADDR " is out of bound of pmem [" FMT_PADDR ", " FMT_PADDR "] at pc = " FMT_WORD,
      addr, PMEM_LEFT, PMEM_RIGHT, cpu.pc);
  //out_of_bound(addr);
  printf(",please again!\n");
  return ;
}
