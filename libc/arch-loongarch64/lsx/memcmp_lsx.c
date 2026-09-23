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

static inline unsigned int memcmp_difference_mask(const unsigned char* left,
                                                   const unsigned char* right) {
  __m128i a = __lsx_vld(left, 0);
  __m128i b = __lsx_vld(right, 0);
  __m128i equal = __lsx_vseq_b(a, b);
  unsigned int equal_bits = __lsx_vpickve2gr_wu(__lsx_vmsknz_b(equal), 0);
  return ~equal_bits & 0xffffu;
}

int memcmp_lsx(const void* left, const void* right, size_t n) {
  const unsigned char* a = (const unsigned char*)left;
  const unsigned char* b = (const unsigned char*)right;

  while (n >= 16) {
    unsigned int differences = memcmp_difference_mask(a, b);
    if (differences != 0) {
      unsigned int first = (unsigned int)__builtin_ctz(differences);
      return (int)a[first] - (int)b[first];
    }
    a += 16;
    b += 16;
    n -= 16;
  }
  while (n != 0) {
    int x = *a++;
    int y = *b++;
    if (x != y) return x - y;
    --n;
  }
  return 0;
}
