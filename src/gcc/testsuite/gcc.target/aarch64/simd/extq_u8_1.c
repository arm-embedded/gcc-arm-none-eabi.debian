/* Test the `vextQu8' AArch64 SIMD intrinsic.  */

/* { dg-do run } */
/* { dg-options "-save-temps -O3 -fno-inline" } */

#include "arm_neon.h"
#include "extq_u8.x"

/* { dg-final { scan-assembler-times "ext\[ \t\]+\[vV\]\[0-9\]+\.16\[bB\], ?\[vV\]\[0-9\]+\.16\[bB\], ?\[vV\]\[0-9\]+\.16\[bB\], ?#?\[0-9\]+\(?:.2\)?\(?:\[ \t\]+@\[a-zA-Z0-9 \]+\)?\n" 15 } } */
/* { dg-final { cleanup-saved-temps } } */
