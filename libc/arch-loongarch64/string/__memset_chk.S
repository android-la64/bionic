#include <private/bionic_asm.h>

ENTRY(__memset_chk)
    bleu    a2, a3, 1f
    jirl zero, __memset_chk_fail, 0

1:
    b memset
END(__memset_chk)

