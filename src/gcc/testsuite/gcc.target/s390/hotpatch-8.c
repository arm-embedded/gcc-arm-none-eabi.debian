/* Functional tests for the function hotpatching feature.  */

/* { dg-do compile { target { ! lp64 } } } */
/* { dg-options "-O3 -mesa -march=g5 -mhotpatch=0,3" } */

#include <stdio.h>

void hp1(void)
{
  printf("hello, world!\n");
}

/* Check number of occurences of certain instructions.  */
/* { dg-final { scan-assembler-not "pre-label NOPs" } } */
/* { dg-final { scan-assembler "post-label.*(3 halfwords)" } } */
/* { dg-final { scan-assembler-times "nopr\t%r7" 1 } } */
/* { dg-final { scan-assembler-times "nop\t0" 1 } } */
/* { dg-final { scan-assembler-not "brcl\t0, 0" } } */
/* { dg-final { scan-assembler-not "alignment for hotpatch" } } */
