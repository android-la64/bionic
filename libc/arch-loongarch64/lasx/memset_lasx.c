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
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <lasxintrin.h>
#include <stddef.h>

__attribute__((no_builtin("memset"), visibility("hidden"))) void* memset_lasx(void* destination,
                                                                                int c, size_t n) {
  unsigned char* p = (unsigned char*)destination;
  if (n < 32) {
    for (size_t i = 0; i < n; ++i) p[i] = (unsigned char)c;
    return destination;
  }

  unsigned char* const end = p + n;
  const __m256i fill = __lasx_xvreplgr2vr_b((unsigned char)c);

  while (n >= 128) {
    __lasx_xvst(fill, p, 0);
    __lasx_xvst(fill, p, 32);
    __lasx_xvst(fill, p, 64);
    __lasx_xvst(fill, p, 96);
    p += 128;
    n -= 128;
  }

  if (n >= 64) {
    __lasx_xvst(fill, p, 0);
    __lasx_xvst(fill, p, 32);
    __lasx_xvst(fill, end, -64);
    __lasx_xvst(fill, end, -32);
  } else if (n >= 32) {
    __lasx_xvst(fill, p, 0);
    __lasx_xvst(fill, end, -32);
  } else if (n != 0) {
    __lasx_xvst(fill, end, -32);
  }
  return destination;
}
