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

#include <isa.h>
#include <memory/paddr.h>

word_t vaddr_ifetch(vaddr_t addr, int len) {
  //printf("1\n");
  if(isa_mmu_check(addr, len, MEM_TYPE_IFETCH) == MMU_DIRECT){
    //printf("jiji\n");
    return paddr_read(addr, len);
  }
  else if(isa_mmu_check(addr, len, MEM_TYPE_IFETCH) == MMU_TRANSLATE){
    //printf("if11111\n");
    paddr_t paddr = isa_mmu_translate(addr, len, MEM_TYPE_IFETCH);
    //printf("paddr: %x\n",paddr);
    return paddr_read(paddr, len);
  }
  else{
    assert(0);
  }
}

word_t vaddr_read(vaddr_t addr, int len) {
  //printf("%x\n",addr);
  if(isa_mmu_check(addr, len, MEM_TYPE_READ) == MMU_DIRECT){
    //printf("111\n");
    return paddr_read(addr, len);
  }
  else if(isa_mmu_check(addr, len, MEM_TYPE_READ) == MMU_TRANSLATE){
    //printf("vr11111\n");
    paddr_t paddr = isa_mmu_translate(addr, len, MEM_TYPE_READ);
    //printf("vr111111\n"); 
    return paddr_read(paddr, len);
  }
  else{
    //printf("jijiji\n");
    assert(0);
  }
}

void vaddr_write(vaddr_t addr, int len, word_t data) {
  if (isa_mmu_check(addr, len, MEM_TYPE_WRITE) == MMU_DIRECT) {
    //printf("wr!\n");
    paddr_write(addr, len, data);
  } else {
    //printf("wr11111\n");
    paddr_t paddr = isa_mmu_translate(addr, len, MEM_TYPE_WRITE);
    paddr_write(paddr, len, data);
  }
}
