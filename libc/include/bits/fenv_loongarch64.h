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
/* Type of the control word.  */
typedef unsigned int fpu_control_t __attribute__ ((__mode__ (__SI__)));

/* Type representing floating-point environment.  This function corresponds
   to the layout of the block written by the `fstenv'.  */
typedef struct
{
  unsigned int __fp_control_register;
} fenv_t;
/* If the default argument is used we use this value.  */
#define FE_DFL_ENV ((const fenv_t *) -1)
/* Floating-point environment where none of the exception is masked.  */
#define FE_NOMASK_ENV ((const fenv_t *) -257)

/* Type representing floating-point control modes.  */
typedef unsigned int femode_t;
/* Default floating-point control modes.  */
#define FE_DFL_MODE ((const femode_t *) -1L)

/* Masks for interrupts.  */
#define _FPU_MASK_V 0x10 /* Invalid operation */
#define _FPU_MASK_Z 0x08 /* Division by zero  */
#define _FPU_MASK_O 0x04 /* Overflow */
#define _FPU_MASK_U 0x02 /* Underflow */
#define _FPU_MASK_I 0x01 /* Inexact operation */
/* Flush denormalized numbers to zero.  */
#define _FPU_FLUSH_TZ 0x1000000
/* Rounding control.  */
#define _FPU_RC_NEAREST 0x000 /* RECOMMENDED */
#define _FPU_RC_ZERO 0x100
#define _FPU_RC_UP 0x200
#define _FPU_RC_DOWN 0x300
/* Mask for rounding control.  */
#define _FPU_RC_MASK 0x300
#define _FPU_RESERVED 0x0
#define _FPU_DEFAULT 0x0
#define _FPU_IEEE 0x1F

/* Macros for accessing the hardware control word.  */
#define _FPU_GETCW(cw) __asm__ volatile ("movfcsr2gr %0,$fcsr0" : "=r"(cw))
#define _FPU_SETCW(cw) __asm__ volatile ("movgr2fcsr $fcsr0,%0" : : "r"(cw))

#define _FCLASS_SNAN     (1 << 0)
#define _FCLASS_QNAN     (1 << 1)
#define _FCLASS_MINF     (1 << 2)
#define _FCLASS_MNORM    (1 << 3)
#define _FCLASS_MSUBNORM (1 << 4)
#define _FCLASS_MZERO    (1 << 5)
#define _FCLASS_PINF     (1 << 6)
#define _FCLASS_PNORM    (1 << 7)
#define _FCLASS_PSUBNORM (1 << 8)
#define _FCLASS_PZERO    (1 << 9)

#define _FCLASS_ZERO     (_FCLASS_MZERO | _FCLASS_PZERO)
#define _FCLASS_SUBNORM  (_FCLASS_MSUBNORM | _FCLASS_PSUBNORM)
#define _FCLASS_NORM     (_FCLASS_MNORM | _FCLASS_PNORM)
#define _FCLASS_INF      (_FCLASS_MINF | _FCLASS_PINF)
#define _FCLASS_NAN      (_FCLASS_SNAN | _FCLASS_QNAN)

/* Mask for enabling exceptions and for the CAUSE bits.  */
#define ENABLE_MASK 0x0000001FU
#define CAUSE_MASK 0x1F000000U

/* Shift for FE_* flags to get up to the ENABLE bits and the CAUSE bits.  */
#define ENABLE_SHIFT 16
#define CAUSE_SHIFT 8

__END_DECLS
