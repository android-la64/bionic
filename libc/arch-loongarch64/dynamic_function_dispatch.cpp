/*
 * Copyright (C) 2023 The Android Open Source Project
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

#include <fcntl.h>
#include <private/bionic_ifuncs.h>
#include <stddef.h>
#include <sys/syscall.h>
#include <unistd.h>

extern "C" {

#if 0
static inline __always_inline int ifunc_faccessat(int dir_fd, const char* path, int mode) {
  register long a0 __asm__("a0") = dir_fd;
  register long a1 __asm__("a1") = reinterpret_cast<long>(path);
  register long a2 __asm__("a2") = mode;
  register long a7 __asm__("a7") = __NR_faccessat;
  __asm__("syscall" : "=r"(a0) : "r"(a0), "r"(a1), "r"(a2), "r"(a7) : "memory");
  return a0;
}
#endif

DEFINE_IFUNC_FOR(memchr) {
  RETURN_FUNC(memchr_func_t, memchr_gc);
}
MEMCHR_SHIM()

DEFINE_IFUNC_FOR(memcmp) {
  RETURN_FUNC(memcmp_func_t, memcmp_gc);
}
MEMCMP_SHIM()

DEFINE_IFUNC_FOR(memcpy) {
  RETURN_FUNC(memcpy_func_t, memcpy_gc);
}
MEMCPY_SHIM()

DEFINE_IFUNC_FOR(memmove) {
  RETURN_FUNC(memmove_func_t, memmove_gc);
}
MEMMOVE_SHIM()

DEFINE_IFUNC_FOR(memset) {
  RETURN_FUNC(memset_func_t, memset_gc);
}
MEMSET_SHIM()

DEFINE_IFUNC_FOR(stpcpy) {
  RETURN_FUNC(stpcpy_func_t, stpcpy_gc);
}
STPCPY_SHIM()

DEFINE_IFUNC_FOR(strcat) {
  RETURN_FUNC(strcat_func_t, strcat_gc);
}
STRCAT_SHIM()

DEFINE_IFUNC_FOR(strchr) {
  RETURN_FUNC(strchr_func_t, strchr_gc);
}
STRCHR_SHIM()

DEFINE_IFUNC_FOR(strcmp) {
  RETURN_FUNC(strcmp_func_t, strcmp_gc);
}
STRCMP_SHIM()

DEFINE_IFUNC_FOR(strcpy) {
  RETURN_FUNC(strcpy_func_t, strcpy_gc);
}
STRCPY_SHIM()

DEFINE_IFUNC_FOR(strlen) {
  RETURN_FUNC(strlen_func_t, strlen_gc);
}
STRLEN_SHIM()

DEFINE_IFUNC_FOR(strncat) {
  RETURN_FUNC(strncat_func_t, strncat_gc);
}
STRNCAT_SHIM()

DEFINE_IFUNC_FOR(strncmp) {
  RETURN_FUNC(strncmp_func_t, strncmp_gc);
}
STRNCMP_SHIM()

DEFINE_IFUNC_FOR(strncpy) {
  RETURN_FUNC(strncpy_func_t, strncpy_gc);
}
STRNCPY_SHIM()

DEFINE_IFUNC_FOR(strnlen) {
  RETURN_FUNC(strnlen_func_t, strnlen_gc);
}
STRNLEN_SHIM()

}  // extern "C"
