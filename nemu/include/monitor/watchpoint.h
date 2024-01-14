#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  //pa1: 实现监视点
  char expr[32];
  bool changed;
  uint32_t newValue, oldValue;
} WP;

//pa1: 实现监视点
bool watchpoint_monitor();  //must be declared cause it will be called in cpu_exec()

#endif
