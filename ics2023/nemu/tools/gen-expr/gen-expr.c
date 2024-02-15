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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

#define MAX_INT 100

#define len 10000

int idx;

uint32_t choose(uint32_t x)
{
  return rand() % x;
}

uint32_t safe_rand()
{
  return (uint32_t)rand() % MAX_INT;
}

void gen(int x)
{
  buf[idx++]=x;
}

void gen_num()
{
  char temp[15];
  sprintf(temp,"%u",safe_rand());
  int length=strlen(temp);
  for(int i = 0;i < length;i++)
  {
    buf[idx++]=temp[i];
  }
}

void gen_rand_op(void)
{
  switch(choose(4))
  {
    case 0: buf[idx++]='+';break;
    case 1: buf[idx++]='-';break;
    case 2: buf[idx++]='*';break;
    case 3: buf[idx++]='/';break;
  }
}

void gen_space(void)
{
  int temp = choose(5);
  for(int i = 0;i < temp;i++)
  {
    buf[idx++]=' ';
  }
}

static inline void gen_rand_expr() {
  if(idx > len)
  {
    gen_num();
    return;
  }
  gen_space();
  switch (choose(3)) {
    case 0: 
      gen_num();
      break;
    case 1: 
      gen('('); 
      gen_rand_expr(); 
      gen(')');
      break;
    default: 
      gen_rand_expr(); 
      gen_rand_op();
      gen_rand_expr();
      break;
  }
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    memset(buf,0,sizeof(buf));
    memset(code_buf,0,sizeof(code_buf));
    idx = 0;

    gen_rand_expr();

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    ret = fscanf(fp, "%d", &result);
    pclose(fp);

    if(ret == 1){
      printf("%u %s\n", result, buf);     
    }
    else{
      i--;
    }
  }
  return 0;
}