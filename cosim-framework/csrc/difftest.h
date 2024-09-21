#ifndef __DIFFTEST_H__
#define __DIFFTEST_H__

#include <stdint.h>

typedef uint64_t rtlreg_t;
typedef uint64_t paddr_t;

enum {
  DIFFTEST_THIS_PC = 32,
  DIFFTEST_MSTATUS,
  DIFFTEST_MCAUSE,
  DIFFTEST_MEPC,
  DIFFTEST_SSTATUS,
  DIFFTEST_SCAUSE,
  DIFFTEST_SEPC,
  DIFFTEST_NR_REG
};

#endif