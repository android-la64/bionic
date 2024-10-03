#include <stdint.h>

extern void* memcpy(void*, void *, size_t);
extern void* __memcpy_chk_fail(void*, void*, size_t, size_t);

void* __memcpy_chk(void* dst, void *src, size_t n, size_t dst_len) {
  if (dst_len < n) __memcpy_chk_fail(dst, src, n, dst_len);
  return memcpy(dst, src, n);
}
