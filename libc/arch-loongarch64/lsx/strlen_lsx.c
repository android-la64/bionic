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

static inline size_t first_zero(__m128i bytes) {
  // Comparison marks zero bytes; vfrstpi returns their first lane, or 16.
  __m128i equal_zero = __lsx_vseqi_b(bytes, 0);
  __m128i position = __lsx_vfrstpi_b(equal_zero, equal_zero, 0);
  return __lsx_vpickve2gr_bu(position, 0);
}

static inline unsigned int any_zero(__m128i bytes) {
  __m128i equal_zero = __lsx_vseqi_b(bytes, 0);
  __m128i packed = __lsx_vmsknz_b(equal_zero);
  return __lsx_vpickve2gr_wu(packed, 0);
}

size_t strlen_lsx(const char* str) {
  const unsigned char* start = (const unsigned char*)str;
  const unsigned char* p = start;
  __m128i a, b, c, d;

  while (((uintptr_t)p & 15u) != 0) {
    if (*p == 0) return (size_t)(p - start);
    ++p;
  }

  // p+63 stays in one 4 KiB region, including on 16/64 KiB pages.
  if (((uintptr_t)p & 4095u) <= 4032u) {
    a = __lsx_vld(p, 0);
    b = __lsx_vld(p, 16);
    c = __lsx_vld(p, 32);
    d = __lsx_vld(p, 48);
    __m128i minimum = __lsx_vmin_bu(__lsx_vmin_bu(a, b), __lsx_vmin_bu(c, d));
    if (any_zero(minimum) != 0) goto resolve_four;
    p += 64;
  }

  for (;;) {
    // Check the next first block before considering a four-block read.
    a = __lsx_vld(p, 0);
    size_t first = first_zero(a);
    if (first != 16) return (size_t)(p - start) + first;

    if (((uintptr_t)p & 4095u) > 4032u) {
      p += 16;
      continue;
    }

    b = __lsx_vld(p, 16);
    c = __lsx_vld(p, 32);
    d = __lsx_vld(p, 48);
    __m128i minimum = __lsx_vmin_bu(__lsx_vmin_bu(b, c), d);
    if (any_zero(minimum) == 0) {
      p += 64;
      continue;
    }

  resolve_four:
    // Resolve candidate blocks in address order.
    first = first_zero(a);
    if (first != 16) return (size_t)(p - start) + first;
    first = first_zero(b);
    if (first != 16) return (size_t)(p - start) + 16 + first;
    first = first_zero(c);
    if (first != 16) return (size_t)(p - start) + 32 + first;
    first = first_zero(d);
    return (size_t)(p - start) + 48 + first;
  }
}
