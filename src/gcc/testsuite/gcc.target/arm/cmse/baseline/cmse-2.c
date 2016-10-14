/* { dg-do compile } */
/* { dg-require-effective-target arm_arch_v8m_base_ok } */
/* { dg-add-options arm_arch_v8m_base } */
/* { dg-options "-mcmse" }  */

extern float bar (void);

float __attribute__ ((cmse_nonsecure_entry))
foo (void)
{
  return bar ();
}
/* { dg-final { scan-assembler "mov\tr1, lr" } } */
/* { dg-final { scan-assembler "mov\tr2, lr" } } */
/* { dg-final { scan-assembler "mov\tr3, lr" } } */
/* { dg-final { scan-assembler "mov\tip, lr" } } */
/* { dg-final { scan-assembler "msr\tAPSR_nzcvq, r1" } } */
/* { dg-final { scan-assembler "bxns\tr1" } } */
