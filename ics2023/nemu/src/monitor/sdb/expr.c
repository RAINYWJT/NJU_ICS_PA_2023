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
#include <regex.h>
#include <math.h>
#include <memory/paddr.h>
#include "sdb.h"
#include <cpu/cpu.h>
enum {
    TK_NOTYPE = 256,TK_DEFER,TK_NEGTIVE,HEX,TK_EQ,TK_NEQ,TK_AND,TK_NUM,TK_PC,TK_REG,TK_OR,TK_GEQ,TK_LEQ,TK_G,TK_L
};
static struct rule {
    const char *regex;
    int token_type;
} rules[] = {
    {"0x[0-9a-f]+",HEX},
    {" +", TK_NOTYPE},    // spaces
    {"\\+", '+'},         // plus
    {"\\-", '-'},         //sub
    {"\\*",'*'},          //multiply
    {"/",'/'},            //divide

    {"\\(",'('},          
    {"\\)",')'},          

    {"==", TK_EQ},        // equal
    {"!=", TK_NEQ},
    {"<=", TK_LEQ},
    {">=", TK_GEQ},
    {"<", TK_L},
    {">", TK_G},

    {"[0-9]+",TK_NUM},      
    {"\\$\\p\\c",TK_PC},
    {"\\$ *[a-z|A-Z|$|0-9]+",TK_REG},
    {"&&",TK_AND}, 
    {"\\|\\|",TK_OR}

};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[1024] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
    int position = 0;
    int i;
    regmatch_t pmatch;

    nr_token = 0;

    while (e[position] != '\0') {
        for (i = 0; i < NR_REGEX; i ++) {
            if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
                char *substr_start = e + position;//pian yi xun zhi
                int substr_len = pmatch.rm_eo;
                position += substr_len;
                Assert(substr_len < sizeof(tokens[nr_token].str)/sizeof(tokens[nr_token].str[0]), "The Token length is out of bound!");
                switch (rules[i].token_type) 
                {
                    case 256: 
                        break;
                    default: 
                        tokens[nr_token].type=rules[i].token_type;
                        strncpy(tokens[nr_token].str,substr_start,substr_len);
                        tokens[nr_token].str[substr_len]='\0';
                        nr_token++;
                        break;
               }

                break;
            }
        }

        if (i == NR_REGEX) {
            printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
            return false;
        }
    }
    //for(int i=0;i<nr_token;i++){
          //printf("len:%s\n",tokens[i].str);
    //}
    int array[nr_token];
    for (int i=0;i<nr_token;i++){
      int temp_len_str=0;
      int length=0;
      while(tokens[i].str[length]!='\0'){
        length++;
        temp_len_str++;
      }
      array[i]=temp_len_str;
      //printf("array:%d\n",array[i]);
    }
    /////////////init the NO_TYPE:
    for(int i=0;i<nr_token;i++){
      int point1=0;
      int point2=0;
      for(int j=0;j<array[i];j++){
        if(tokens[i].str[j]!=' '){
          tokens[i].str[point2]=tokens[i].str[j];
          point2++;
        }
        point1++;
      }
      tokens[i].str[point2]='\0';
    }
    //for(int i=0;i<nr_token;i++){
          //printf("len:%s\n",tokens[i].str);
    //}
    //printf("nr_token:%d\n",nr_token);
    return true;
}


bool check_parentheses(int p, int q)
{
  if(tokens[p].type != '('  || tokens[q].type != ')')
    return false;
  int cnt=0;
  int idx=p;
  while(p<=q)
  {    
      //printf("cnt:%d\n",cnt); 
    if(tokens[idx].type == '('){
      cnt++;}
    else if(tokens[idx].type == ')'){
      //printf("11!\n");
      cnt--;}
    if(cnt == 0 && idx == q){
      return true;
    }
    if(cnt == 0 && idx != q){
      return false;
    }
    idx++;
  }
  return false;
}

int main_operator(int p, int q){
    int add_index = 0;
    int mul_index = 0;
    int count_bracket = 0;
    int eq_index = 0;
    int neq_index = 0;
    int leq_index = 0;
    int geq_index = 0;
    int l_index = 0;
    int g_index = 0;
    int and_index = 0;
    int or_index = 0;
    for (int i=p;i<q;i++){
      if(tokens[i].type == '('){
        count_bracket++;
      }
      if(tokens[i].type == ')' ){
        count_bracket--;
      }
      if(count_bracket == 0 && (tokens[i].type == '+' || tokens[i].type == '-')){
        add_index = i ;
      }
      if(count_bracket == 0 && (tokens[i].type == '*' || tokens[i].type == '/')){
        mul_index = i ;
      }
      if(tokens[i].type == TK_EQ){
        eq_index = i;
      }
      if(tokens[i].type == TK_NEQ){
        neq_index = i;
      }
      if(tokens[i].type == TK_LEQ){
        leq_index = i;
      }
      if(tokens[i].type == TK_GEQ){
        geq_index = i;
      }
      if(tokens[i].type == TK_L){
        l_index = i;
      }
      if(tokens[i].type == TK_G){
        g_index = i;
      }


      if(tokens[i].type == TK_AND){
        and_index = i;
      }
      if(tokens[i].type == TK_OR){
        or_index = i;
        //printf("i:%d\n",i);
      }
    }
    if(add_index!=0){
      return add_index;
    }
    else if (mul_index!= 0){
      return mul_index;
    }
    else if(eq_index!= 0){
      return eq_index;
    }
    else if(neq_index!= 0){
      return neq_index;
    }
    else if(leq_index!= 0){
      return leq_index;
    }
    else if(geq_index!= 0){
      return geq_index;
    }
    else if(l_index!= 0){
      return l_index;
    }
    else if(g_index!= 0){
      return g_index;
    }
    else if(and_index!= 0){
      return and_index;
    }
    else if(or_index!= 0){
      return or_index;
    }
    assert(0);
    return 0;
} 

unsigned int eval(u_int32_t p,u_int32_t q){
    ///////////////////////////////////defer
    /*
    printf("%d %d\n",p,q);
    for (int i = p; i < q; i++) {
      if (tokens[i].type == TK_DEFER) {
        uint32_t address = eval(p + 1, q);
        return paddr_read(address,4);
      }
    }*/
    ///////////////////////////////////
    ////////////////////////////negative judge
    /*
    bool flg = true;
    bool flg2= false;
    for(int i=p;i<q;i++){
        if(tokens[i].type == ')' || tokens[i].type == '('){
          flg = false;
        }
    }
    for(int i=p;i<q;i++){
      if(tokens[i].type == TK_NEGTIVE){
          flg2 = true;
          break;
        }
    }
      int temp0 = p;
      int temp1=0;
      while(tokens[temp0].type== TK_NEGTIVE && flg == true && flg2 ==true){
        //printf("yes1\n");   
        if(tokens[temp0+1].type!= TK_NEGTIVE && temp1%2==0 ){
          tokens[temp0].type = TK_NOTYPE;
          int cnt = 0;
          while (tokens[temp0+1].str[cnt]!= '\0')
          {
            cnt++;
          }
          for(int j=cnt;j>0;j--){
            tokens[temp0+1].str[j]=tokens[temp0+1].str[j-1];
          }
          tokens[temp0+1].str[0]='-'; 
          printf("yes1\n");   
          return eval(temp0+1,q);
        }
        else if(tokens[temp0+1].type!= TK_NEGTIVE && temp1%2==1){
          return eval(temp0+1,q);
        }
        else{
          temp0++;
          temp1++;
        } 
    }*/

    if(tokens[p].type==TK_NEGTIVE || tokens[p].type==TK_DEFER){
      word_t value =0;
      int pointer = p;
      while (tokens[pointer].type == TK_DEFER || tokens[pointer].type == TK_NEGTIVE)
      {
        pointer++;
      }
      if(((tokens[pointer].type == TK_NUM || tokens[pointer].type==TK_REG || tokens[pointer].type== TK_PC || tokens[pointer].type == HEX) && pointer==q) || check_parentheses(pointer,q)){
        value = eval(p+1,q);
        if(tokens[p].type==TK_DEFER){
          return paddr_read(value,4); 
        }
        else if(tokens[p].type == TK_NEGTIVE){
          return 0-value;
        }
      }    
    }
    ///////////////////////////////////end negative
    if (p > q) {
        assert(0);
        return -1;
    }
    else if (p == q) {
          bool success ;
          switch (tokens[p].type) {
            case TK_NUM:
                return strtoul(tokens[p].str, NULL, 10);
            case HEX:
            //printf("222!\n");
                return strtoul(tokens[p].str+2, NULL, 16);
            case TK_REG:
            //printf("111\n");
                success = false;
                return isa_reg_str2val(tokens[p].str,&success);
            case TK_PC:
            //printf("yes!\n");
                return cpu.pc;
            default:
                assert(0);
                break;
            }
    }
    else if (check_parentheses(p, q) == true) {
        return eval(p + 1, q - 1);
    }
    else {
        u_int32_t op = main_operator(p,q);
        //printf("op:%d\n",op);
        u_int32_t val1 = eval(p, op - 1);
        u_int32_t val2 = eval(op + 1, q);
        u_int32_t op_type = tokens[op].type;
        switch (op_type) {
            case '+': return  val1 + val2;
            case '-': return val1 - val2;
            case '*': return val1 * val2;
            case '/': 
                if (val2 == 0){
                    printf("Not to divide zero!\n");
                    return 0;
                }
                return val1 / val2;
            case TK_EQ: return val1 == val2;
            case TK_NEQ: return val1 != val2;
            case TK_LEQ: return val1 <= val2;
            case TK_GEQ: return val1 >= val2;
            case TK_L:return val1 < val2;
            case TK_G:return val1 > val2;
            case TK_AND: return val1 && val2;
            case TK_OR: return val1 || val2;
            default: assert(0);
    }
  }
}



word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  for (int i=0;i<nr_token;i++){
    if(tokens[i].type == '-' &&(i==0 || (tokens[i-1].type!= TK_NUM&& tokens[i-1].type!= TK_REG  && tokens[i-1].type!= TK_PC&& tokens[i-1].type!=')' && tokens[i-1].type!=HEX))){
      tokens[i].type = TK_NEGTIVE;
      //printf("i:%d\n",i);
      //printf("yes\n");
    }
    if(tokens[i].type == '*' &&(i==0 || (tokens[i-1].type!= TK_NUM && tokens[i-1].type!= TK_REG && tokens[i-1].type!= TK_PC &&  tokens[i-1].type!=')' && tokens[i-1].type!=HEX))){
      tokens[i].type = TK_DEFER;
      //printf("i:%d\n",i);
    }
    if(tokens[i].str[0] == '0' && tokens[i].str[1] == 'x'){
        tokens[i].type = HEX;
      }
    if(tokens[i].str[0] == '$' || (tokens[i].str[0]=='$' && tokens[i].str[1]=='$')){
      //printf("ji:%d %d %d\n",tokens[i].str[0],tokens[i].str[1],tokens[i].str[2]);
        tokens[i].type = TK_REG;
      }
    if(tokens[i].str[0] == '$' && tokens[i].str[1]=='p'){
        //printf("ji:%d %d %d\n",tokens[i].str[0],tokens[i].str[1],tokens[i].str[2]);
      
        tokens[i].type = TK_PC;
      }
    if(tokens[i].str[0] == '=' && tokens[i].str[1] == '='){
        tokens[i].type = TK_EQ;
      }
    if(tokens[i].str[0] == '!' && tokens[i].str[1] == '='){
        tokens[i].type = TK_NEQ;
      }
    if(tokens[i].str[0] == '<' && tokens[i].str[1] == '='){
        tokens[i].type = TK_LEQ;
      }
    if(tokens[i].str[0] == '>' && tokens[i].str[1] == '='){
        tokens[i].type = TK_GEQ;
      }
    if(tokens[i].str[0] == '<'){
        tokens[i].type = TK_L;
      }
    if(tokens[i].str[0] == '>' ){
        tokens[i].type = TK_G;
      }
    if(tokens[i].str[0] == '&' && tokens[i].str[1] == '&'){
        tokens[i].type = TK_AND;
      }
    if(tokens[i].str[0] == '|' && tokens[i].str[1] == '|'){
        tokens[i].type = TK_OR;
      }
    }
    
  //printf("len:%d\n",nr_token);
  u_int32_t result =eval(0,nr_token-1);
  return result;
}
