#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  int value;
  struct watchpoint *next,*previous;
  char expr[64];
  /* TODO: Add more members if necessary */

} WP;

#endif
