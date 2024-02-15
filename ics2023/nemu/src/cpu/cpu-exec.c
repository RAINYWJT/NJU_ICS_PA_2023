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

#include <cpu/cpu.h>
#include <cpu/decode.h>
#include <cpu/difftest.h>
#include <locale.h>
#include "../monitor/sdb/sdb.h"
/* The assembly code of instructions executed is only output to the screen
 * when the number of instructions executed is less than this value.
 * This is useful when you use the `si' command.
 * You can modify this value as you want.
 */
#ifdef CONFIG_ITRACE
static int p_iringbuf(int flag);
#endif

#define MAX_INST_TO_PRINT 10
#define IRINGBUF_SIZE 32
#define EVERY_BUF_LEN_START 4
static char iringbuf[IRINGBUF_SIZE][EVERY_BUF_LEN_START+128];
static size_t iringbuf_index = 0;

CPU_state cpu = {};
uint64_t g_nr_guest_inst = 0;
static uint64_t g_timer = 0; // unit: us
static bool g_print_step = false;

void device_update();
void check_wp();

static void trace_and_difftest(Decode *_this, vaddr_t dnpc) {
#ifdef CONFIG_ITRACE_COND
  if (ITRACE_COND) { log_write("%s\n", _this->logbuf); }
#endif
  if (g_print_step) { IFDEF(CONFIG_ITRACE, puts(_this->logbuf)); }
  IFDEF(CONFIG_DIFFTEST, difftest_step(_this->pc, dnpc));
  check_wp();
}

static void exec_once(Decode *s, vaddr_t pc) { //这个函数的功能就是我们在上一小节中介绍的内容: 让CPU执行当前PC指向的一条指令, 然后更新PC.
  s->pc = pc;
  s->snpc = pc;
  isa_exec_once(s);
  cpu.pc = s->dnpc;
#ifdef CONFIG_ITRACE
  char *p = s->logbuf;
  p += snprintf(p, sizeof(s->logbuf), FMT_WORD ":", s->pc);
  int ilen = s->snpc - s->pc;
  int i;
  uint8_t *inst = (uint8_t *)&s->isa.inst.val;
  for (i = ilen - 1; i >= 0; i --) {
    p += snprintf(p, 4, " %02x", inst[i]);
  }
  int ilen_max = MUXDEF(CONFIG_ISA_x86, 8, 4);
  int space_len = ilen_max - ilen;
  if (space_len < 0) space_len = 0;
  space_len = space_len * 3 + 1;
  memset(p, ' ', space_len);
  p += space_len;


#ifndef CONFIG_ISA_loongarch32r
  void disassemble(char *str, int size, uint64_t pc, uint8_t *code, int nbyte);
  disassemble(p, s->logbuf + sizeof(s->logbuf) - p,
      MUXDEF(CONFIG_ISA_x86, s->snpc, s->pc), (uint8_t *)&s->isa.inst.val, ilen); ///logbuf
#else
  p[0] = '\0'; // the upstream llvm does not support loongarch32r
#endif
  strcpy(iringbuf[iringbuf_index]+EVERY_BUF_LEN_START,s->logbuf);
  iringbuf_index = (iringbuf_index+1)%IRINGBUF_SIZE;
#endif
}

static void execute(uint64_t n) {  //模拟了CPU的工作方式: 不断执行指令
  Decode s;
  for (;n > 0; n --) {
    exec_once(&s, cpu.pc);
    g_nr_guest_inst ++;
    trace_and_difftest(&s, cpu.pc);
    if (nemu_state.state != NEMU_RUNNING) break;
    IFDEF(CONFIG_DEVICE, device_update());
    word_t intr = isa_query_intr();
    if (intr != INTR_EMPTY) {
      //printf("jijiji\n");
      cpu.pc = isa_raise_intr(intr, cpu.pc);
    }
  }
}

static void statistic() {
  IFNDEF(CONFIG_TARGET_AM, setlocale(LC_NUMERIC, ""));
#define NUMBERIC_FMT MUXDEF(CONFIG_TARGET_AM, "%", "%'") PRIu64
  Log("host time spent = " NUMBERIC_FMT " us", g_timer);
  Log("total guest instructions = " NUMBERIC_FMT, g_nr_guest_inst);
  if (g_timer > 0) Log("simulation frequency = " NUMBERIC_FMT " inst/s", g_nr_guest_inst * 1000000 / g_timer);
  else Log("Finish running in less than 1 us and can not calculate the simulation frequency");
}

#ifdef CONFIG_ITRACE
static int p_iringbuf(int flag){
  if(flag == 1){
    const char prefix[EVERY_BUF_LEN_START]="--->";
    printf("%s",ANSI_FMT("============ The follow instructions ===============\n", ANSI_FG_GREEN));
    for(int i=0;i<IRINGBUF_SIZE;i++){
      if(iringbuf[i][EVERY_BUF_LEN_START]=='\0'){
        break;
      }
      if ((i + 1) % IRINGBUF_SIZE == iringbuf_index) {
        strncpy(iringbuf[i], prefix, EVERY_BUF_LEN_START);
        iringbuf[i][EVERY_BUF_LEN_START] = '\0';
        printf("%s", iringbuf[i]);
        printf("%s","0");
        printf("%s\n",iringbuf[i]+5);
        printf("%s",ANSI_FMT("====================================================\n", ANSI_FG_GREEN));
        /*
        const char *sub_str = "ebreak";
        if(strstr(*iringbuf,sub_str)){
          Log(ANSI_FMT("PASS!", ANSI_FG_GREEN));
        }
        */
       Log(ANSI_FMT("PASS!", ANSI_FG_GREEN));
      } 
      else {
        printf("%s\n", iringbuf[i]);
        //printf("1\n");
      }
    }
    return 0;
  }
  if(flag == 0){
    const char prefix[EVERY_BUF_LEN_START]="--->";
    printf("%s",ANSI_FMT("============ The follow instructions ===============\n", ANSI_FG_RED));
    for(int i=0;i<IRINGBUF_SIZE;i++){
      if(iringbuf[i][EVERY_BUF_LEN_START]=='\0'){
        break;
      }
      if ((i + 1) % IRINGBUF_SIZE == iringbuf_index) {
        strncpy(iringbuf[i], prefix, EVERY_BUF_LEN_START);
        iringbuf[i][EVERY_BUF_LEN_START] = '\0';
        printf("%s", iringbuf[i]);
        printf("%s","0");
        printf("%s\n",iringbuf[i]+5);
        printf("%s",ANSI_FMT("====================================================\n", ANSI_FG_RED));
        Log(ANSI_FMT("BUGS DETECTED!!!", ANSI_FG_RED));
      } 
      else {
        printf("%s\n", iringbuf[i]);
        //printf("1\n");
      }
    }
    return 0;
  }
  assert(0);
}
#endif

void assert_fail_msg() {
  isa_reg_display();
  statistic();
}

//important
/* Simulate how the CPU works. */
void cpu_exec(uint64_t n) {
  //initial
  iringbuf_index=0;
  for(int i=0;i<IRINGBUF_SIZE;i++){
    memset(iringbuf[i],' ',EVERY_BUF_LEN_START);
    iringbuf[i][EVERY_BUF_LEN_START]='\0';
  }

  g_print_step = (n < MAX_INST_TO_PRINT);
  switch (nemu_state.state) {
    case NEMU_END: case NEMU_ABORT:
      printf("Program execution has ended. To restart the program, exit NEMU and run again.\n");
      return;
    default: nemu_state.state = NEMU_RUNNING;
  }

  uint64_t timer_start = get_time();

  execute(n);

  uint64_t timer_end = get_time();
  g_timer += timer_end - timer_start;

  switch (nemu_state.state) {
    case NEMU_RUNNING: nemu_state.state = NEMU_STOP; break;
    case NEMU_END: 
            #ifdef CONFIG_ITRACE
            p_iringbuf(1);
            #endif
             Log("nemu: %s at pc = " FMT_WORD,
            (nemu_state.state == NEMU_ABORT ? ANSI_FMT("ABORT", ANSI_FG_RED) :
           (nemu_state.halt_ret == 0 ? ANSI_FMT("HIT GOOD TRAP", ANSI_FG_GREEN) :
            ANSI_FMT("HIT BAD TRAP", ANSI_FG_RED))),
            nemu_state.halt_pc);
            statistic();
            break;  
    case NEMU_ABORT:
        #ifdef CONFIG_ITRACE
        p_iringbuf(0);
        #endif   
        Log("nemu: %s at pc = " FMT_WORD,
            (nemu_state.state == NEMU_ABORT ? ANSI_FMT("ABORT", ANSI_FG_RED) :
           (nemu_state.halt_ret == 0 ? ANSI_FMT("HIT GOOD TRAP", ANSI_FG_GREEN) :
            ANSI_FMT("HIT BAD TRAP", ANSI_FG_RED))),
            nemu_state.halt_pc);
        // fall through
    case NEMU_QUIT: statistic();
  }
}
