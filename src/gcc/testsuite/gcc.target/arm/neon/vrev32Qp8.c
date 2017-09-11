/* Test the `vrev32Qp8' ARM Neon intrinsic.  */
/* This file was autogenerated by neon-testgen.  */

/* { dg-do assemble } */
/* { dg-require-effective-target arm_neon_ok } */
/* { dg-options "-save-temps -O0" } */
/* { dg-add-options arm_neon } */

#include "arm_neon.h"

void test_vrev32Qp8 (void)
{
  poly8x16_t out_poly8x16_t;
  poly8x16_t arg0_poly8x16_t;

  out_poly8x16_t = vrev32q_p8 (arg0_poly8x16_t);
}

/* { dg-final { scan-assembler "vrev32\.8\[ 	\]+\[qQ\]\[0-9\]+, \[qQ\]\[0-9\]+!?\(\[ 	\]+@\[a-zA-Z0-9 \]+\)?\n" } } */
