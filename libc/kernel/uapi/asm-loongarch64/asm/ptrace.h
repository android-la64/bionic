/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ***   To edit the content of this header, modify the corresponding
 ***   source file (e.g. under external/kernel-headers/original/) then
 ***   run bionic/libc/kernel/tools/update_all.py
 ***
 ***   Any manual change here will be lost the next time this script will
 ***   be run. You've been warned!
 ***
 ****************************************************************************
 ****************************************************************************/
#ifndef _UAPI_ASM_PTRACE_H
#define _UAPI_ASM_PTRACE_H
#include <linux/types.h>
#include <stdint.h>
#define GPR_BASE 0
#define GPR_NUM 32
#define GPR_END (GPR_BASE + GPR_NUM - 1)
#define PC (GPR_END + 1)
#define BADVADDR (GPR_END + 2)
#define NUM_FPU_REGS 32
struct user_pt_regs {
  unsigned long regs[32];
  unsigned long csr_era;
  unsigned long csr_badv;
  unsigned long reserved[11];
} __attribute__((aligned(8)));
struct user_fp_state {
  uint64_t fpr[32];
  uint64_t fcc;
  uint32_t fcsr;
};
struct user_lsx_state {
  uint64_t vregs[32 * 2];
};
struct user_lasx_state {
  uint64_t vregs[32 * 4];
};
struct user_lbt_state {
  uint64_t regs[6];
};
#define NUM_WATCH_REGS 16
enum pt_watch_style {
  pt_watch_style_la32,
  pt_watch_style_la64
};
struct la32_watch_regs {
  uint32_t addr;
  uint32_t mask;
  uint8_t irw;
  uint8_t irwstat;
  uint8_t irwmask;
} __attribute__((aligned(8)));
struct la64_watch_regs {
  uint64_t addr;
  uint64_t mask;
  uint8_t irw;
  uint8_t irwstat;
  uint8_t irwmask;
} __attribute__((aligned(8)));
struct pt_watch_regs {
  int16_t max_valid;
  int16_t num_valid;
  enum pt_watch_style style;
  union {
    struct la32_watch_regs la32[NUM_WATCH_REGS];
    struct la64_watch_regs la64[NUM_WATCH_REGS];
  };
};
#define PTRACE_SYSEMU 0x1f
#define PTRACE_SYSEMU_SINGLESTEP 0x20
#define PTRACE_GET_WATCH_REGS 0xd0
#define PTRACE_SET_WATCH_REGS 0xd1
#define LA_WATCH_W (1 << 0)
#define LA_WATCH_R (1 << 1)
#define LA_WATCH_I (1 << 2)
#define LA_WATCH_IRW (LA_WATCH_W | LA_WATCH_R | LA_WATCH_I)
#endif
