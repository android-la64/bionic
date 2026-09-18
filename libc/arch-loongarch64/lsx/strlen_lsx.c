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

// A set bit means that the corresponding byte in the 16-byte block is zero.
static inline unsigned int strlen_zero_mask(const unsigned char* p) {
  __m128i bytes = __lsx_vld(p, 0);
  __m128i equal_zero = __lsx_vseqi_b(bytes, 0);
  __m128i packed = __lsx_vmsknz_b(equal_zero);
  return __lsx_vpickve2gr_wu(packed, 0) & 0xffffu;
}

size_t strlen_lsx(const char* str) {
  const unsigned char* start = (const unsigned char*)str;
  const unsigned char* p = start;

  // Reach a 16-byte boundary without reading before the string starts.
  while (((uintptr_t)p & 15u) != 0) {
    if (*p == 0) return (size_t)(p - start);
    ++p;
  }

  for (;;) {
    unsigned int zero_mask = strlen_zero_mask(p);
    if (zero_mask != 0) {
      return (size_t)(p - start) + (size_t)__builtin_ctz(zero_mask);
    }
    p += 16;
  }
}
