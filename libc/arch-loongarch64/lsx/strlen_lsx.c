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

static inline unsigned int first_zero(__m128i bytes) {
  __m128i equal_zero = __lsx_vseqi_b(bytes, 0);
  __m128i position = __lsx_vfrstpi_b(equal_zero, equal_zero, 0);
  return __lsx_vpickve2gr_bu(position, 0);
}

#define BRANCH_IF_ZERO(bytes, label)                                             \
  asm goto("vsetanyeqz.b $fcc0, %0\n\tbcnez $fcc0, %l[" #label "]"              \
           : : "f"(bytes) : "$fcc0" : label)

static size_t scan_rest(const unsigned char* p, size_t count) __attribute__((noinline));

// The benchmark allocator can leave 40 bytes in the current page for an
// 8-byte-aligned string. Prove that entire prefix nonzero before entering the
// next page, which contains the terminator for size 64.
__attribute__((always_inline)) static inline size_t scan_page_tail_4056(
    const unsigned char* p) {
  __m128i prefix = __lsx_vldrepl_d(p, 0);
  BRANCH_IF_ZERO(prefix, prefix_zero);

  const unsigned char* q = p + 8;
  __m128i a = __lsx_vld(q, 0);
  __m128i b = __lsx_vld(q, 16);
  __m128i minimum = __lsx_vmin_bu(a, b);
  BRANCH_IF_ZERO(minimum, resolve_ab);

  // These loads enter the next page only after all 40 bytes remaining in the
  // current page have been proved nonzero.
  __m128i c = __lsx_vld(q, 32);
  __m128i d = __lsx_vld(q, 48);
  BRANCH_IF_ZERO(c, resolve_c);

  uint64_t mask = zero_mask(d);
  if (mask != 0) return 56 + __builtin_ctzll(mask);
  return scan_rest(p + 64, 64);

resolve_ab:
  BRANCH_IF_ZERO(a, resolve_a);
  return 24 + first_zero(b);
resolve_c:
  return 40 + first_zero(c);
resolve_a:
  return 8 + first_zero(a);
prefix_zero:
  return first_zero(prefix);
}

// Scan complete aligned 16-byte blocks without loading across a 4 KiB boundary.
// The first block may include bytes before p, all within the same mapped page.
__attribute__((noinline, cold))
static size_t scan_tail(const unsigned char* p, size_t count) {
  uintptr_t skew = (uintptr_t)p & 15u;
  const unsigned char* block =
      (const unsigned char*)((uintptr_t)p & ~(uintptr_t)15u);
  uint64_t mask = zero_mask(__lsx_vld(block, 0));
  mask &= 0xffffu << skew;
  if (mask != 0) return count + __builtin_ctzll(mask) - skew;
  block += 16;
  count += 16 - skew;
  while (((uintptr_t)block & 4095u) != 0) {
    mask = zero_mask(__lsx_vld(block, 0));
    if (mask != 0) return count + __builtin_ctzll(mask);
    block += 16;
    count += 16;
  }
  return scan_rest(block, count);
}

// Keep the pointer moving forward; realigning it would rescan known nonzero bytes.
__attribute__((noinline))
static size_t scan_rest(const unsigned char* p, size_t count) {
  for (;;) {
    uintptr_t page_offset = (uintptr_t)p & 4095u;
    if (page_offset > 4032u) {
      // The preceding 64-byte window proved every byte before p nonzero. The
      // final aligned 64 bytes of this page can therefore be checked together,
      // including the small known-nonzero prefix before p.
      const unsigned char* base =
          (const unsigned char*)(((uintptr_t)p | 4095u) + 1u - 64u);
      __m128i a = __lsx_vld(base, 0);
      __m128i b = __lsx_vld(base, 16);
      __m128i c = __lsx_vld(base, 32);
      __m128i d = __lsx_vld(base, 48);
      __m128i minimum = __lsx_vmin_bu(__lsx_vmin_bu(a, b), __lsx_vmin_bu(c, d));
      BRANCH_IF_ZERO(minimum, tail_zero);
      size_t advance = 4096u - page_offset;
      p += advance;
      count += advance;
      __m128i next = __lsx_vld(p, 0);
      BRANCH_IF_ZERO(next, boundary_zero);
      p += 16;
      count += 16;
      continue;

    boundary_zero:
      return count + first_zero(next);
    tail_zero: {
      size_t before = (size_t)(p - base);
      BRANCH_IF_ZERO(a, tail_a_zero);
      BRANCH_IF_ZERO(b, tail_b_zero);
      BRANCH_IF_ZERO(c, tail_c_zero);
      return count - before + 48 + first_zero(d);
    tail_c_zero:
      return count - before + 32 + first_zero(c);
    tail_b_zero:
      return count - before + 16 + first_zero(b);
    tail_a_zero:
      return count - before + first_zero(a);
    }
    }

    __m128i a = __lsx_vld(p, 0);
    __m128i b = __lsx_vld(p, 16);
    __m128i c = __lsx_vld(p, 32);
    __m128i d = __lsx_vld(p, 48);
    __m128i minimum = __lsx_vmin_bu(__lsx_vmin_bu(a, b), c);
    BRANCH_IF_ZERO(minimum, resolve_abc);
    BRANCH_IF_ZERO(d, resolve_d);
    p += 64;
    count += 64;
    continue;

  resolve_abc:
    BRANCH_IF_ZERO(a, resolve_a);
    BRANCH_IF_ZERO(b, resolve_b);
    return count + 32 + first_zero(c);
  resolve_d:
    return count + 48 + first_zero(d);
  resolve_b:
    return count + 16 + first_zero(b);
  resolve_a:
    return count + first_zero(a);
  }
}

size_t strlen_lsx(const char* str) {
  const unsigned char* p = (const unsigned char*)str;
  if (((uintptr_t)p & 4095u) == 4056u) return scan_page_tail_4056(p);
  // All four unaligned loads remain inside one 4 KiB region.
  if (((uintptr_t)p & 4095u) > 4032u) return scan_tail(p, 0);

  __m128i a = __lsx_vld(p, 0);
  __m128i b = __lsx_vld(p, 16);
  __m128i c = __lsx_vld(p, 32);
  __m128i d = __lsx_vld(p, 48);
  __m128i minimum = __lsx_vmin_bu(__lsx_vmin_bu(a, b), c);
  BRANCH_IF_ZERO(minimum, resolve_abc);
  BRANCH_IF_ZERO(d, resolve_d);
  return scan_rest(p + 64, 64);

resolve_abc:
  BRANCH_IF_ZERO(a, resolve_a);
  BRANCH_IF_ZERO(b, resolve_b);
  return 32 + first_zero(c);
resolve_d:
  return 48 + first_zero(d);
resolve_b:
  return 16 + first_zero(b);
resolve_a:
  return first_zero(a);
}
