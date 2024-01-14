#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

//pa1: 设置监视点
//function: add a new watchpoint in the pool
WP *new_wp(char *expr)  //can't use expr as a parameter, because expr is a function in expr.c
{
  //fail case1: pool is full
  if(free_ == NULL)
  {
    printf("No free watchpoint!\n");
    return NULL;
  }

  //fail case2: expr is invalid
  bool success = true;
  uint32_t result = expr(expr, &success);
  if(success == false)
  {
    printf("Expression evaluation failed!\n");
    return NULL;
  }

  //success case
  //free linklist moveing direction: left->right; -1 node
  WP *wp = free_;
  free_ = free_->next;
  //busy linklist moving direction: right->left; +1`node
  wp->next = head;
  head = wp;
  strcpy(wp->expr, expr);
  wp->changed = false;
  wp->newValue = wp->oldValue = result;
  return wp;
}

//pa1: 实现监视点
//function: check if the watchpoint's value is changed
//return: false means no watchpoint is changed, true means there is a watchpoint(or more) is changed
bool watchpoint_monitor()
{
  WP *wp = head;
  if(wp == NULL)
    return false;
  bool success = true;
  uint32_t result = expr(wp->expr, &success);
  if(success == false)
    panic("new watchpoint's expr is invalid!\n");
  if(result != wp->newValue)
  {
    wp->changed = true;
    wp->oldValue = wp->newValue;
    wp->newValue = result;
    printf("watchpoint changed! Please use 'info w' to check!\n");
    return true;
  }
  return false;
}

//pa1: 打印监视点信息
void watchpoint_display()
{
  WP *wp = head;
  if(wp == NULL)
    printf("No watchpoint!\n");
  else
  {
    printf("Num\tExpr\t\tOldValue\tNewValue\n");
    while(wp != NULL)
    {
      printf("%d\t%s\t\t%d\t\t%d\n", wp->NO, wp->expr, wp->oldValue, wp->newValue);
      wp = wp->next;
    }
  }
}

//pa1: 删除监视点
//function: use the NO to get the watchpoint
WP *get_wp(int NO)
{
  WP *wp = head;
  while(wp != NULL)
  {
    if(wp->NO == NO)
      return wp;
    wp = wp->next;
  }
  return NULL;
}

//pa1: 删除监视点
//function: find the watchpoint's previous node
WP *get_prev_wp(int NO)
{
  WP *wp = head;
  if(wp == NULL)
    return NULL;
  if(wp->NO == NO)
    return NULL;
  while(wp->next != NULL)
  {
    if(wp->next->NO == NO)
      return wp;
    wp = wp->next;
  }
  return NULL;
}

//pa1: 删除监视点
//function: delete the watchpoint
bool free_wp(int NO)
{
  WP *wp = get_wp(NO);
  if(wp == NULL)
    return false;
  WP *prevWp = get_prev_wp(NO);
  if(prevWp == NULL)
  {
    head = wp->next;
    //insert wp into free linklist's head(which is free_ pointer)
    wp->next = free_;
    free_ = wp;
  }
  else
  {
    prevWp->next = wp->next;
    wp->next = free_;
    free_ = wp;
  }
  return true;
}

