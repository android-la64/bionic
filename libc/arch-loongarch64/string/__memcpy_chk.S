#include <private/bionic_asm.h>

.extern __memcpy_chk_fail
.extern memcpy

ENTRY(__memcpy_chk)
  bleu a2, a3, 1f
  jirl zero,  __memcpy_chk_fail, 0

1:
   b memcpy
END(__memcpy_chk)
