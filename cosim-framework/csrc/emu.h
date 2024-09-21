#include "common.h"
#include "difftest.h"
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <vector>

#include <verilated.h>
#include "../../build/emu-compile/VSimTop.h"
#include "../../build/emu-compile/VSimTop__Syms.h"
#if VM_TRACE
#include <verilated_vcd_c.h> // Trace file format header
#endif

class Emulator {
  const char *image;
  std::shared_ptr<VSimTop> dut_ptr;
#ifdef VM_TRACE
  std::unique_ptr<VerilatedVcdC> tfp;
#endif

  uint32_t seed;
  uint64_t max_cycles, cycles;
  uint64_t log_begin, log_end, log_level;

  std::vector<const char *> parse_args(int argc, const char *argv[]);
  static const struct option long_options[];
  static void print_help(const char *file);

  /**
   * @brief get regs and csrs from dut (RTL Emulator)
   *
   * @param r : save dut results
   */
  void read_dut_regs(rtlreg_t *r) {
    for (int i = 0; i < 32; i++) {

      r[i] = dut_ptr->io_difftest_regs[i];
    }

    r[DIFFTEST_THIS_PC] = dut_ptr->io_difftest_this_pc;
    r[DIFFTEST_MSTATUS] = dut_ptr->io_difftest_mstatus;
    r[DIFFTEST_SSTATUS] = dut_ptr->io_difftest_sstatus;
    r[DIFFTEST_MEPC] = dut_ptr->io_difftest_mepc;
    r[DIFFTEST_SEPC] = dut_ptr->io_difftest_sepc;
    r[DIFFTEST_MCAUSE] = dut_ptr->io_difftest_mcause;
    r[DIFFTEST_SCAUSE] = dut_ptr->io_difftest_scause;
  }

public:
  Emulator(int argc, const char *argv[])
      : image(nullptr), dut_ptr(new VSimTop), seed(0), max_cycles(-1),
        cycles(0), log_begin(0), log_end(-1), log_level(0) {
    auto args = parse_args(argc, argv);

    // srand
    srand(seed);
    Verilated::randReset(2);

    // set log time range and log level
    dut_ptr -> io_logCtrl_log_begin = log_begin;
    dut_ptr -> io_logCtrl_log_end = log_end;
    dut_ptr -> io_logCtrl_log_level = log_level;

    // init ram
    extern void init_ram(const char * img);
    init_ram(image);

    // init device
    extern void init_device(void);
    init_device();

    // init core
    reset_ncycles(10);
    }

  void reset_ncycles(size_t cycles) {
    for(int i = 0; i < cycles; i++) {
      dut_ptr->reset = 1;
      dut_ptr->clock = 0;
      dut_ptr->eval();
      dut_ptr->clock = 1;
      dut_ptr->eval();
      dut_ptr->reset = 0;
    }
  }

  void single_cycle() {
    dut_ptr->clock = 0;
    dut_ptr->eval();

    dut_ptr->clock = 1;
    dut_ptr->eval();

#if VM_TRACE
    tfp->dump(cycles);
#endif

    cycles ++;

  }  

  void execute_cycles(uint64_t n) {
    
  }

};