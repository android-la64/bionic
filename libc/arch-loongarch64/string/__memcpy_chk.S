#include <private/bionic_asm.h>

.extern __memcpy_chk_fail
.extern memcpy

ENTRY(__memcpy_chk)
  bleu a2, a3, 1f
  la.pcrel t0, __memcpy_chk_fail
  jirl zero, t0, 0

1:
   b memcpy
END(__memcpy_chk)
