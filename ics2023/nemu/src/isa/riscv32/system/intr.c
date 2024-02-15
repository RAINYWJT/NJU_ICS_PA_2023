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
#define IRQ_TIMER 0x80000007  // for riscv32
#define MSTATUS_MIE 0x00000008
#define MSTATUS_MPIE 0x00000080

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */
  //printf("yes1!\n");
  //printf("d:%x\n",cpu.riscv32_csr.mstatus);
  #ifdef CONFIG_ETRACE_COND
    printf("mcacuse = %x mstatus = %x mepc = %x \n",  cpu.riscv32_csr.mcause ,  cpu.riscv32_csr.mstatus ,  cpu.riscv32_csr.mepc);
  #endif
  cpu.riscv32_csr.mcause = NO;
  cpu.riscv32_csr.mepc = epc;
  if ((cpu.riscv32_csr.mstatus & MSTATUS_MIE) != 0) {
    cpu.riscv32_csr.mstatus |= MSTATUS_MPIE;
    //printf("1d:%x\n",cpu.riscv32_csr.mstatus);
  }
  else{
    cpu.riscv32_csr.mstatus &= ~MSTATUS_MPIE;
    //printf("2d:%x\n",cpu.riscv32_csr.mstatus);
  }
  cpu.riscv32_csr.mstatus &= ~MSTATUS_MIE;
  //printf("d:%x\n",cpu.riscv32_csr.mstatus);
  return cpu.riscv32_csr.mtvec;
}

word_t isa_query_intr() {
  //printf("cpu:%d\n",cpu.riscv32_csr.mstatus & MSTATUS_MIE);
  if (cpu.INTR == true && ((cpu.riscv32_csr.mstatus & MSTATUS_MIE) != 0 )) {
    //printf("jiji\n");
    cpu.INTR = false;
    return IRQ_TIMER;
  }
  return INTR_EMPTY;
}
