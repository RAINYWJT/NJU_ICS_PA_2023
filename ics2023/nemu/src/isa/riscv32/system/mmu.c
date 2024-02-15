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
#include <memory/vaddr.h>
#include <memory/paddr.h>
#include <memory/host.h>

int isa_mmu_check(word_t vaddr,int len,int type){
    //printf("%d\n",cpu.riscv32_csr.satp);
    if (cpu.riscv32_csr.satp == 0) {
        return MMU_DIRECT;
    }else{
        //word_t valid2 = cpu.riscv32_csr.satp & 0x1;
        //printf("1:%d\n",cpu.riscv32_csr.satp >> 31);
        return MMU_TRANSLATE;
    }
    //return MMU_DIRECT;
}

//static int cnt = 0;

typedef struct {
  uint32_t valid : 1; 
  uint32_t read : 1;
  uint32_t write : 1; 
  uint32_t exec : 1; 
  uint32_t user : 1; 
  uint32_t global : 1; 
  uint32_t accessed : 1; 
  uint32_t dirty : 1; 
  uint32_t rsw : 2; 
  uint32_t ppn : 22; 
} pte_t;

pte_t word_to_pte(word_t word) {
  pte_t pte;
  pte.valid = word & 0x1;
  pte.read = (word >> 1) & 0x1;
  pte.write = (word >> 2) & 0x1;
  pte.exec = (word >> 3) & 0x1;
  pte.user = (word >> 4) & 0x1;
  pte.global = (word >> 5) & 0x1;
  pte.accessed = (word >> 6) & 0x1;
  pte.dirty = (word >> 7) & 0x1;
  pte.rsw = (word >> 8) & 0x3;
  pte.ppn = (word >> 10) & 0x3fffff;
  return pte;
}

word_t pte_to_word(pte_t pte) {
  word_t word = 0;
  word |= pte.valid;
  word |= pte.read << 1;
  word |= pte.write << 2;
  word |= pte.exec << 3;
  word |= pte.user << 4;
  word |= pte.global << 5;
  word |= pte.accessed << 6;
  word |= pte.dirty << 7;
  word |= pte.rsw << 8;
  word |= pte.ppn << 10;
  return word;
}

/**/
paddr_t isa_mmu_translate(vaddr_t vaddr, int len, int type)
{
#define v1 (vaddr & 0xffc00000)
#define v0 (vaddr & 0x003ff000)
#define vf (vaddr & 0x00000fff)
  
  //printf("\n");
  //printf("%x\n",vaddr);
  
  paddr_t add_stap = (cpu.riscv32_csr.satp & 0x003fffff);
  paddr_t first_level_pt = add_stap * PAGE_SIZE + (v1 >> 22) *4;
  
  word_t first_level_pt_word = host_read(guest_to_host(first_level_pt), 4);
  pte_t first_level_pte = word_to_pte(first_level_pt_word);

  //printf("fist: %x\n",first_level_pt_word);

  paddr_t physical_address;
  if(!first_level_pte.valid){
    assert(0);
  }
  if((first_level_pte.read | first_level_pte.write | first_level_pte.exec) != 7){
    paddr_t second_level_pt_add = first_level_pte.ppn * PAGE_SIZE + (v0 >> 12) * 4;
    word_t second_level_pt_word = host_read(guest_to_host(second_level_pt_add), 4);
    //printf("secpt: %x\n",second_level_pt_add);
    pte_t second_level_pte = word_to_pte(second_level_pt_word);
    physical_address = (second_level_pte.ppn << 12) | vf; 
  }
  else{
    //printf("jijiiji\n");
    physical_address = (first_level_pte.ppn << 22) | v0 | vf;
  }
  //printf("phy: %x\n",physical_address);
  //assert(physical_address == vaddr);
  return physical_address;
}

