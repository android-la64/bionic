/*
 * Copyright (C) 2026 The Android Open Source Project
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

#include <lsxintrin.h>
#include <stddef.h>
#include <stdint.h>

static inline unsigned int strchr_match_mask(__m128i comparison) {
  __m128i packed = __lsx_vmsknz_b(comparison);
  return __lsx_vpickve2gr_wu(packed, 0) & 0xffffu;
}

char* strchr_lsx(const char* string, int c) {
  const unsigned char target_byte = (unsigned char)c;
  const unsigned char* p = (const unsigned char*)string;

  while (((uintptr_t)p & 15u) != 0) {
    unsigned char value = *p;
    if (value == target_byte) return (char*)p;
    if (value == 0) return NULL;
    ++p;
  }

  const __m128i target = __lsx_vreplgr2vr_b(target_byte);
  for (;;) {
    __m128i bytes = __lsx_vld(p, 0);
    unsigned int target_mask = strchr_match_mask(__lsx_vseq_b(bytes, target));
    unsigned int zero_mask = strchr_match_mask(__lsx_vseqi_b(bytes, 0));
    unsigned int stop_mask = target_mask | zero_mask;
    if (stop_mask != 0) {
      unsigned int index = __builtin_ctz(stop_mask);
      if ((target_mask & (1u << index)) != 0) return (char*)(p + index);
      return NULL;
    }
    p += 16;
  }
}
