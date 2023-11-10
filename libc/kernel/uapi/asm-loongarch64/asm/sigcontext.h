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
#ifndef _UAPI_ASM_SIGCONTEXT_H
#define _UAPI_ASM_SIGCONTEXT_H
#include <linux/types.h>
#include <linux/posix_types.h>
#define USED_FP (1 << 0)
#define ADRERR_RD (1 << 30)
#define ADRERR_WR (1 << 31)
#define FPU_REG_WIDTH 256
#define FPU_ALIGN __attribute__((aligned(32)))
struct sigcontext {
  __u64 sc_pc;
  __u64 sc_regs[32];
  __u32 sc_flags;
  __u32 sc_fcsr;
  __u32 sc_vcsr;
  __u64 sc_fcc;
  __u64 sc_scr[4];
  union {
    __u32 val32[FPU_REG_WIDTH / 32];
    __u64 val64[FPU_REG_WIDTH / 64];
  } sc_fpregs[32] FPU_ALIGN;
  __u8 sc_reserved[4096] __attribute__((__aligned__(16)));
};
#endif
