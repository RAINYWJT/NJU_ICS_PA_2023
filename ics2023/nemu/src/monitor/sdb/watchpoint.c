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

#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  /* TODO: Add more members if necessary */
  char expr[128];
  bool use_or_not;
  uint32_t value;
} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;
static int count = 0;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
void check_wp(){
  for(int i=0;i<NR_WP;i++){
    if(wp_pool[i].use_or_not==true){
      bool success = false;
      uint32_t temp = expr(wp_pool[i].expr,&success);
      if(success){
        if(temp != wp_pool[i].value){
          nemu_state.state = NEMU_STOP;
          printf("Not the same!\n");
          return;
        }
      }
      else{
        printf("Wrong expr!\n");
      }
    }
  }
}

WP* new_wp() {
    WP* p = (WP*)malloc(sizeof(WP));
    if (p == NULL) {
        printf("Failed to allocate memory.\n");
        assert(0);
    }
    
    p->next = NULL;
    p->use_or_not = true;
    
    if (head == NULL) {
        head = p;
    } else {
        WP* cur = head;
        while (cur->next != NULL) {
            cur = cur->next;
        }
        cur->next = p;
    }
    
    return p;
}

void free_wp(WP* wp) {
    if (wp == NULL) {
        printf("Watchpoint not found.\n");
        return;
    }
    
    wp->use_or_not = false;
    
    if (wp == head) {
        head = head->next;
    } else {
        WP* cur = head;
        while (cur != NULL && cur->next != wp) {
            cur = cur->next;
        }
        if (cur == NULL) {
            printf("Watchpoint not found.\n");
            return;
        }
        cur->next = wp->next;
    }
    
    free(wp);
    printf("Deleted watchpoint successfully!.\n");
}

void display_watchpoint() {
    if (head == NULL) {
        printf("No watchpoint now.\n");
        return;
    }
    
    WP* cur = head;
    while (cur != NULL) {
        printf("Watchpoint.No: %d, expr = \"%s\", value = %u\n", cur->NO, cur->expr, cur->value);
        cur = cur->next;
    }
}

void create_watchpoint(char* args) {
    WP* p = new_wp();
    strcpy(p->expr, args);
    
    bool success = true;
    uint32_t tmp = expr(p->expr, &success);
    if (success) {
        p->NO = count++;
        p->value = tmp;
        printf("Create watchpoint No.%d success.\n", p->NO);
    } else {
        printf("Problem with creating the watchpoint!\n");
        free_wp(p);
    }
}

void delete_watchpoint(int no) {
    WP* cur = head;
    while (cur != NULL) {
        if (cur->NO == no) {
            free_wp(cur);
            return;
        }
        cur = cur->next;
    }
    printf("Watchpoint not found.\n");
}
