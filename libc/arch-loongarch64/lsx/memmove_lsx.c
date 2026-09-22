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

static inline void copy_forward(unsigned char* d, const unsigned char* s, size_t n) {
  if (n < 16) {
    for (size_t i = 0; i < n; ++i) d[i] = s[i];
    return;
  }
  if (n <= 32) {
    __m128i first = __lsx_vld(s, 0);
    __m128i last = __lsx_vld(s + n - 16, 0);
    __lsx_vst(first, d, 0);
    __lsx_vst(last, d + n - 16, 0);
    return;
  }
  if (n <= 64) {
    __m128i first0 = __lsx_vld(s, 0);
    __m128i first1 = __lsx_vld(s, 16);
    __m128i last1 = __lsx_vld(s + n - 32, 0);
    __m128i last0 = __lsx_vld(s + n - 16, 0);
    __lsx_vst(first0, d, 0);
    __lsx_vst(first1, d, 16);
    __lsx_vst(last1, d + n - 32, 0);
    __lsx_vst(last0, d + n - 16, 0);
    return;
  }
  __m128i final = __lsx_vld(s + n - 16, 0);
  unsigned char* final_destination = d + n - 16;
  while (n >= 128) {
    __m128i v0 = __lsx_vld(s, 0);
    __m128i v1 = __lsx_vld(s, 16);
    __m128i v2 = __lsx_vld(s, 32);
    __m128i v3 = __lsx_vld(s, 48);
    __m128i v4 = __lsx_vld(s, 64);
    __m128i v5 = __lsx_vld(s, 80);
    __m128i v6 = __lsx_vld(s, 96);
    __m128i v7 = __lsx_vld(s, 112);
    __lsx_vst(v0, d, 0);
    __lsx_vst(v1, d, 16);
    __lsx_vst(v2, d, 32);
    __lsx_vst(v3, d, 48);
    __lsx_vst(v4, d, 64);
    __lsx_vst(v5, d, 80);
    __lsx_vst(v6, d, 96);
    __lsx_vst(v7, d, 112);
    s += 128;
    d += 128;
    n -= 128;
  }
  if (n >= 64) {
    __m128i v0 = __lsx_vld(s, 0);
    __m128i v1 = __lsx_vld(s, 16);
    __m128i v2 = __lsx_vld(s, 32);
    __m128i v3 = __lsx_vld(s, 48);
    __lsx_vst(v0, d, 0);
    __lsx_vst(v1, d, 16);
    __lsx_vst(v2, d, 32);
    __lsx_vst(v3, d, 48);
    s += 64;
    d += 64;
    n -= 64;
  }
  if (n >= 32) {
    __m128i v0 = __lsx_vld(s, 0);
    __m128i v1 = __lsx_vld(s, 16);
    __lsx_vst(v0, d, 0);
    __lsx_vst(v1, d, 16);
    s += 32;
    d += 32;
    n -= 32;
  }
  if (n >= 16) {
    __m128i value = __lsx_vld(s, 0);
    __lsx_vst(value, d, 0);
    s += 16;
    d += 16;
    n -= 16;
  }
  __lsx_vst(final, final_destination, 0);
}

static inline void copy_backward(unsigned char* d, const unsigned char* s, size_t n) {
  d += n;
  s += n;
  while (n >= 128) {
    d -= 128;
    s -= 128;
    __m128i v0 = __lsx_vld(s, 0);
    __m128i v1 = __lsx_vld(s, 16);
    __m128i v2 = __lsx_vld(s, 32);
    __m128i v3 = __lsx_vld(s, 48);
    __m128i v4 = __lsx_vld(s, 64);
    __m128i v5 = __lsx_vld(s, 80);
    __m128i v6 = __lsx_vld(s, 96);
    __m128i v7 = __lsx_vld(s, 112);
    __lsx_vst(v0, d, 0);
    __lsx_vst(v1, d, 16);
    __lsx_vst(v2, d, 32);
    __lsx_vst(v3, d, 48);
    __lsx_vst(v4, d, 64);
    __lsx_vst(v5, d, 80);
    __lsx_vst(v6, d, 96);
    __lsx_vst(v7, d, 112);
    n -= 128;
  }
  if (n >= 64) {
    d -= 64;
    s -= 64;
    __m128i v0 = __lsx_vld(s, 0);
    __m128i v1 = __lsx_vld(s, 16);
    __m128i v2 = __lsx_vld(s, 32);
    __m128i v3 = __lsx_vld(s, 48);
    __lsx_vst(v0, d, 0);
    __lsx_vst(v1, d, 16);
    __lsx_vst(v2, d, 32);
    __lsx_vst(v3, d, 48);
    n -= 64;
  }
  if (n >= 32) {
    d -= 32;
    s -= 32;
    __m128i v0 = __lsx_vld(s, 0);
    __m128i v1 = __lsx_vld(s, 16);
    __lsx_vst(v0, d, 0);
    __lsx_vst(v1, d, 16);
    n -= 32;
  }
  if (n >= 16) {
    d -= 16;
    s -= 16;
    __m128i value = __lsx_vld(s, 0);
    __lsx_vst(value, d, 0);
    n -= 16;
  }
  while (n != 0) {
    --d;
    --s;
    *d = *s;
    --n;
  }
}

__attribute__((no_builtin("memcpy", "memmove"), visibility("hidden")))
void* memmove_lsx(void* destination, const void* source, size_t n) {
  unsigned char* d = (unsigned char*)destination;
  const unsigned char* s = (const unsigned char*)source;
  uintptr_t d_address = (uintptr_t)d;
  uintptr_t s_address = (uintptr_t)s;

  if (n == 0 || d_address == s_address) return destination;
  if (d_address < s_address || d_address - s_address >= n) {
    copy_forward(d, s, n);
  } else {
    copy_backward(d, s, n);
  }
  return destination;
}
