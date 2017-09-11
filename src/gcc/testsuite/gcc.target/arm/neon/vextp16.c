/* Test the `vextp16' ARM Neon intrinsic.  */
/* This file was autogenerated by neon-testgen.  */

/* { dg-do assemble } */
/* { dg-require-effective-target arm_neon_ok } */
/* { dg-options "-save-temps -O0" } */
/* { dg-add-options arm_neon } */

#include "arm_neon.h"

void test_vextp16 (void)
{
  poly16x4_t out_poly16x4_t;
  poly16x4_t arg0_poly16x4_t;
  poly16x4_t arg1_poly16x4_t;

  out_poly16x4_t = vext_p16 (arg0_poly16x4_t, arg1_poly16x4_t, 0);
}

/* { dg-final { scan-assembler "vext\.16\[ 	\]+\[dD\]\[0-9\]+, \[dD\]\[0-9\]+, \[dD\]\[0-9\]+, #\[0-9\]+!?\(\[ 	\]+@\[a-zA-Z0-9 \]+\)?\n" } } */
