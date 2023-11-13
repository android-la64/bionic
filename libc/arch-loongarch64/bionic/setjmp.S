/*
 * Copyright (C) 2013 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <private/bionic_asm.h>
#include <private/bionic_constants.h>

// The internal structure of a jmp_buf is totally private.
// Current layout (changes from release to release):
//
// word   name            description
// 0      sigflag/cookie  setjmp cookie in top 31 bits, signal mask flag in low bit
// 1      sigmask         64-bit signal mask
// 2      ra
// 3      sp
// 4      fp
// 5      s0
// ......
// 13     s8
// 14     fs0
// ......
// 21     fs7
// 22     checksum
// ...... reserved
// 32
// _JBLEN: defined in bionic/libc/include/setjmp.h

#define _JB_SIGFLAG   0
#define _JB_SIGMASK   1 * 8
#define _JB_RA        2 * 8
#define _JB_SP        3 * 8
#define _JB_FP        4 * 8
#define _JB_S0        5 * 8
#define _JB_S1        6 * 8
#define _JB_S2        7 * 8
#define _JB_S3        8 * 8
#define _JB_S4        9 * 8
#define _JB_S5       10 * 8
#define _JB_S6       11 * 8
#define _JB_S7       12 * 8
#define _JB_S8       13 * 8
#define _JB_FS0      14 * 8
#define _JB_FS1      15 * 8
#define _JB_FS2      16 * 8
#define _JB_FS3      17 * 8
#define _JB_FS4      18 * 8
#define _JB_FS5      19 * 8
#define _JB_FS6      20 * 8
#define _JB_FS7      21 * 8
#define _JB_CHECKSUM 22 * 8

.macro m_mangle_registers reg, sp_reg
  xor s0, s0, \reg
  xor s1, s1, \reg
  xor s2, s2, \reg
  xor s3, s3, \reg
  xor s4, s4, \reg
  xor s5, s5, \reg
  xor s6, s6, \reg
  xor s7, s7, \reg
  xor s8, s8, \reg
  xor fp, fp, \reg
  xor \sp_reg, \sp_reg, \reg
.endm

.macro m_calculate_checksum dst, src, scratch
  li.d \dst, 0
  .irp i,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21
    ld.d \scratch, \src, \i * 8
    xor  \dst, \dst, \scratch
  .endr
.endm

.macro m_unmangle_registers reg, sp_reg
  m_mangle_registers \reg, sp_reg=\sp_reg
.endm

ENTRY(setjmp)
__BIONIC_WEAK_ASM_FOR_NATIVE_BRIDGE(setjmp)
  li.d    a1, 1
  b       sigsetjmp
END(setjmp)

ENTRY(_setjmp)
__BIONIC_WEAK_ASM_FOR_NATIVE_BRIDGE(_setjmp)
  li.d    a1, 0
  b       sigsetjmp
END(_setjmp)

// int sigsetjmp(sigjmp_buf env, int save_signal_mask);
ENTRY(sigsetjmp)
__BIONIC_WEAK_ASM_FOR_NATIVE_BRIDGE(sigsetjmp)
  addi.d  sp, sp, -24
  st.d    a0, sp, 0
  st.d    a1, sp, 8
  st.d    ra, sp, 16

  // Get the cookie and store it along with the signal flag.
  move    a0, a1
  la      t0, __bionic_setjmp_cookie_get
  jirl    ra, t0, 0

  move    a1, a0
  ld.d    a0, sp, 0
  st.d    a1, a0, _JB_SIGFLAG

  // Do we need to save the signal mask?
  andi    a1, a1, 1
  beqz    a1, 1f

  // Save current signal mask.
  // The 'how'/a0 argument is ignored if set is NULL.
  li.d    a1, 0 // NULL
  addi.d  a2, a0, _JB_SIGMASK // old_mask.
  la      t0, sigprocmask
  jirl    ra, t0, 0

1:
  // Restore original a0/ra.
  ld.d    a0, sp, 0
  ld.d    ra, sp, 16
  addi.d  sp, sp, 24

  // Get the cookie.
  ld.d    a1, a0, _JB_SIGFLAG
  li.d    t0, -2
  and     a1, a1, t0

  // Save core registers.
  move    a2, sp
  m_mangle_registers a1, sp_reg=a2
  st.d    ra, a0, _JB_RA
  st.d    fp, a0, _JB_FP
  st.d    a2, a0, _JB_SP
  st.d    s0, a0, _JB_S0
  st.d    s1, a0, _JB_S1
  st.d    s2, a0, _JB_S2
  st.d    s3, a0, _JB_S3
  st.d    s4, a0, _JB_S4
  st.d    s5, a0, _JB_S5
  st.d    s6, a0, _JB_S6
  st.d    s7, a0, _JB_S7
  st.d    s8, a0, _JB_S8
  m_unmangle_registers a1, sp_reg=a2

  // Save floating point registers.
  fst.d   fs0, a0, _JB_FS0
  fst.d   fs1, a0, _JB_FS1
  fst.d   fs2, a0, _JB_FS2
  fst.d   fs3, a0, _JB_FS3
  fst.d   fs4, a0, _JB_FS4
  fst.d   fs5, a0, _JB_FS5
  fst.d   fs6, a0, _JB_FS6
  fst.d   fs7, a0, _JB_FS7

  // Calculate the checksum and save it.
  m_calculate_checksum t0, a0, t1
  st.d    t0, a0, _JB_CHECKSUM

  li.d    a0, 0
  jirl    zero, ra, 0
END(sigsetjmp)

// void siglongjmp(sigjmp_buf env, int value);
ENTRY(siglongjmp)
__BIONIC_WEAK_ASM_FOR_NATIVE_BRIDGE(siglongjmp)
  // Check the checksum before doing anything.
  m_calculate_checksum t0, a0, t1
  ld.d    t1, a0, _JB_CHECKSUM
  bne     t0, t1, 3f

  // Do we need to restore the signal mask?
  ld.d    a2, a0, _JB_SIGFLAG
  andi    a3, a2, 1
  beqz    a3, 1f

  addi.d  sp, sp, -24
  st.d    a0, sp, 0
  st.d    ra, sp, 8

  // Restore the signal mask.
  st.d    a1, sp, 16 // Save 'value'.

  move    a2, a0
  li.d    a0, 2 // SIG_SETMASK
  addi.d  a1, a2, _JB_SIGMASK // new_mask
  li.d    a2, 0 // NULL

  la      t0, sigprocmask
  jirl    ra, t0, 0

  ld.d    a1, sp, 16 // Restore 'value'.

  // Restore original a0 and ra.
  ld.d    a0, sp, 0
  ld.d    ra, sp, 8
  addi.d  sp, sp, 24

  ld.d    a2, a0, _JB_SIGFLAG
1:
  // Restore core registers.
  li.d    t0, -2
  and     a2, a2, t0
  ld.d    ra, a0, _JB_RA
  ld.d    fp, a0, _JB_FP  // Don't clobber the upper bits of gp (x3) used for SCS yet.
  ld.d    s0, a0, _JB_S0
  ld.d    s1, a0, _JB_S1
  ld.d    s2, a0, _JB_S2
  ld.d    s3, a0, _JB_S3
  ld.d    s4, a0, _JB_S4
  ld.d    s5, a0, _JB_S5
  ld.d    s6, a0, _JB_S6
  ld.d    s7, a0, _JB_S7
  ld.d    s8, a0, _JB_S8
  ld.d    a3, a0, _JB_SP
  m_unmangle_registers a2, sp_reg=a3
  move    sp, a3

  addi.d  sp, sp, -24
  st.d    ra, sp, 0
  st.d    a0, sp, 8
  st.d    a1, sp, 16
  ld.d    a0, a0, _JB_SIGFLAG

  la      t0, __bionic_setjmp_cookie_check
  jirl    ra, t0, 0

  ld.d    ra, sp, 0
  ld.d    a0, sp, 8
  ld.d    a1, sp, 16
  addi.d  sp, sp, 24

  // Restore floating point registers.
  fld.d   fs0,  a0, _JB_FS0
  fld.d   fs1,  a0, _JB_FS1
  fld.d   fs2,  a0, _JB_FS2
  fld.d   fs3,  a0, _JB_FS3
  fld.d   fs4,  a0, _JB_FS4
  fld.d   fs5,  a0, _JB_FS5
  fld.d   fs6,  a0, _JB_FS6
  fld.d   fs7,  a0, _JB_FS7

  // Set return value.
  beqz    a1, 2f
  li.d    a0, 1
2:
  move    a0, a1
  jirl    zero, ra, 0

3:
  la      t0, __bionic_setjmp_checksum_mismatch
  jirl    ra, t0, 0
END(siglongjmp)

ALIAS_SYMBOL(longjmp, siglongjmp)
__BIONIC_WEAK_ASM_FOR_NATIVE_BRIDGE(longjmp)
ALIAS_SYMBOL(_longjmp, siglongjmp)
__BIONIC_WEAK_ASM_FOR_NATIVE_BRIDGE(_longjmp)
