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

static inline uint64_t zero_mask(__m128i bytes) {
  __m128i equal_zero = __lsx_vseqi_b(bytes, 0);
  __m128i packed = __lsx_vmsknz_b(equal_zero);
  return __lsx_vpickve2gr_du(packed, 0);
}

// Called only after the first 64 bytes or at a 4 KiB boundary.
static size_t scan_rest(const unsigned char* p, size_t count) __attribute__((noinline));

// Scan one 4 KiB tail, then resume aligned 64-byte windows.
__attribute__((noinline, cold))
static size_t scan_tail(const unsigned char* p, size_t count) {
  for (;;) {
    uintptr_t offset = (uintptr_t)p & 4095u;
    if (offset == 0) return scan_rest(p, count);
    if (offset > 4080u) {
      do {
        if (*p == 0) return count;
        ++p;
        ++count;
      } while (((uintptr_t)p & 4095u) != 0);
      continue;
    }
    uint64_t mask = zero_mask(__lsx_vld(p, 0));
    if (mask != 0) return count + __builtin_ctzll(mask);
    p += 16;
    count += 16;
  }
}

// The first window proved that any bytes revisited by alignment are nonzero.
__attribute__((noinline))
static size_t scan_rest(const unsigned char* p, size_t count) {
  // Revisit only proven nonzero bytes so each subsequent window is 64-byte aligned.
  uintptr_t skew = (uintptr_t)p & 63u;
  p -= skew;
  count -= skew;
  for (;;) {
    __m128i a = __lsx_vld(p, 0);
    __m128i b = __lsx_vld(p, 16);
    __m128i c = __lsx_vld(p, 32);
    __m128i d = __lsx_vld(p, 48);
    __m128i minimum = __lsx_vmin_bu(__lsx_vmin_bu(a, b), c);
    if (zero_mask(minimum) != 0) {
      uint64_t mask = zero_mask(a);
      if (mask != 0) return count + __builtin_ctzll(mask);
      mask = zero_mask(b);
      if (mask != 0) return count + 16 + __builtin_ctzll(mask);
      mask = zero_mask(c);
      return count + 32 + __builtin_ctzll(mask);
    }
    uint64_t mask = zero_mask(d);
    if (mask != 0) return count + 48 + __builtin_ctzll(mask);
    p += 64;
    count += 64;
  }
}

size_t strlen_lsx(const char* str) {
  const unsigned char* p = (const unsigned char*)str;
  // All four unaligned loads remain inside one 4 KiB region.
  if (((uintptr_t)p & 4095u) > 4032u) return scan_tail(p, 0);

  __m128i a = __lsx_vld(p, 0);
  __m128i b = __lsx_vld(p, 16);
  __m128i c = __lsx_vld(p, 32);
  __m128i d = __lsx_vld(p, 48);
  __m128i minimum = __lsx_vmin_bu(__lsx_vmin_bu(a, b), c);
  if (zero_mask(minimum) != 0) {
    uint64_t mask = zero_mask(a);
    if (mask != 0) return __builtin_ctzll(mask);
    mask = zero_mask(b);
    if (mask != 0) return 16 + __builtin_ctzll(mask);
    mask = zero_mask(c);
    return 32 + __builtin_ctzll(mask);
  }
  uint64_t mask = zero_mask(d);
  if (mask != 0) return 48 + __builtin_ctzll(mask);
  return scan_rest(p + 64, 64);
}
