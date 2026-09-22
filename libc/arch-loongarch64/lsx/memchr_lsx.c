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

static inline unsigned int memchr_match_mask(__m128i bytes, __m128i target) {
  __m128i equal = __lsx_vseq_b(bytes, target);
  __m128i packed = __lsx_vmsknz_b(equal);
  return __lsx_vpickve2gr_wu(packed, 0) & 0xffffu;
}

void* memchr_lsx(const void* source, int c, size_t n) {
  if (n == 0) return NULL;

  const unsigned char target_byte = (unsigned char)c;
  const unsigned char* p = (const unsigned char*)source;

  const __m128i target = __lsx_vreplgr2vr_b(target_byte);
  while (n >= 64) {
    unsigned int mask0 = memchr_match_mask(__lsx_vld(p, 0), target);
    unsigned int mask1 = memchr_match_mask(__lsx_vld(p, 16), target);
    unsigned int mask2 = memchr_match_mask(__lsx_vld(p, 32), target);
    unsigned int mask3 = memchr_match_mask(__lsx_vld(p, 48), target);
    if ((mask0 | mask1 | mask2 | mask3) != 0) {
      if (mask0 != 0) return (void*)(p + __builtin_ctz(mask0));
      if (mask1 != 0) return (void*)(p + 16 + __builtin_ctz(mask1));
      if (mask2 != 0) return (void*)(p + 32 + __builtin_ctz(mask2));
      return (void*)(p + 48 + __builtin_ctz(mask3));
    }
    p += 64;
    n -= 64;
  }

  while (n >= 16) {
    unsigned int mask = memchr_match_mask(__lsx_vld(p, 0), target);
    if (mask != 0) return (void*)(p + __builtin_ctz(mask));
    p += 16;
    n -= 16;
  }

  while (n != 0) {
    if (*p == target_byte) return (void*)p;
    ++p;
    --n;
  }
  return NULL;
}
