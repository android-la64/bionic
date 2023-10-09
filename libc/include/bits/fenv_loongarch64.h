#pragma once

#include <sys/types.h>

__BEGIN_DECLS

#define FE_INEXACT 0x010000
#define FE_UNDERFLOW 0x020000
#define FE_OVERFLOW 0x040000
#define FE_DIVBYZERO 0x080000
#define FE_INVALID 0x100000

#define FE_ALL_EXCEPT \
  (FE_INEXACT | FE_DIVBYZERO | FE_UNDERFLOW | FE_OVERFLOW | FE_INVALID)

/* The LoongArch FPU supports all of the four defined rounding modes.  We
   use again the bit positions in the FPU control word as the values
   for the appropriate macros.  */
#define FE_TONEAREST 0x000
#define FE_TOWARDZERO 0x100
#define FE_UPWARD 0x200
#define FE_DOWNWARD 0x300

/* Type representing exception flags.  */
typedef unsigned int fexcept_t;

/* Type representing floating-point environment.  This function corresponds
   to the layout of the block written by the `fstenv'.  */
typedef struct
{
  unsigned int __fp_control_register;
} fenv_t;

__END_DECLS
