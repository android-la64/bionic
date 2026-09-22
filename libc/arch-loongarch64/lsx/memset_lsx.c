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

void* memset_lsx(void* destination, int c, size_t n) {
  unsigned char* p = (unsigned char*)destination;
  if (n < 16) {
    for (size_t i = 0; i < n; ++i) p[i] = (unsigned char)c;
    return destination;
  }

  unsigned char* end = p + n;
  const __m128i fill = __lsx_vreplgr2vr_b((unsigned char)c);

  while (n >= 128) {
    __lsx_vst(fill, p, 0);
    __lsx_vst(fill, p, 16);
    __lsx_vst(fill, p, 32);
    __lsx_vst(fill, p, 48);
    __lsx_vst(fill, p, 64);
    __lsx_vst(fill, p, 80);
    __lsx_vst(fill, p, 96);
    __lsx_vst(fill, p, 112);
    p += 128;
    n -= 128;
  }
  if (n >= 64) {
    __lsx_vst(fill, p, 0);
    __lsx_vst(fill, p, 16);
    __lsx_vst(fill, p, 32);
    __lsx_vst(fill, p, 48);
    p += 64;
    n -= 64;
  }
  if (n >= 32) {
    __lsx_vst(fill, p, 0);
    __lsx_vst(fill, p, 16);
    p += 32;
    n -= 32;
  }
  if (n >= 16) {
    __lsx_vst(fill, p, 0);
    p += 16;
    n -= 16;
  }
  if (n != 0) __lsx_vst(fill, end, -16);
  return destination;
}
