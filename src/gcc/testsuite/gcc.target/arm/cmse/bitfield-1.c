/* { dg-do run } */
/* { dg-require-effective-target arm_cmse_ok } */
/* { dg-options "--save-temps -mcmse -Wl,--section-start,.gnu.sgstubs=0x20400000" } */
#include <limits.h>
#include <stdlib.h>

typedef struct
{
  unsigned short  a : 6;
  unsigned char	  b : 3;
  unsigned char	  c;
  unsigned short  d : 8;
} test_st;

test_st __attribute__ ((cmse_nonsecure_entry)) foo (void)
{
  test_st t;
  t.a = 63u;
  t.b = 7u;
  t.c = UCHAR_MAX;
  t.d = 255u;
  return t;
}

int
main (void)
{
  test_st t;
  t = foo ();
  if (t.a != 63u
      || t.b != 7u
      || t.c != UCHAR_MAX
      || t.d != 255u)
    abort ();
  return 0;
}

/* { dg-final { scan-assembler "movw\tr1, #1855" } } */
/* { dg-final { scan-assembler "movt\tr1, 65535" } } */
/* { dg-final { scan-assembler "ands\tr0(, r0)?, r1" } } */
