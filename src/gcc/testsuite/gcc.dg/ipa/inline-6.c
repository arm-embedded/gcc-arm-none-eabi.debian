/* Check statements that are eliminated by inlining.  */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-ipa-inline-details -fno-early-inlining -fno-partial-inlining -fno-ipa-cp"  } */

void foo (void);
void bar (void);
void bagr (void);

static int t(int a)
{
  if (a==1)
    {
      foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo();
      foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo();
      foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo();
      foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo();
      foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo();
      foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo();
      foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo(); foo();
    }
  else if (a==2)
    {
      bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar();
      bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar();
      bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar();
      bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar();
      bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar();
      bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar(); bar();
    }
  else
   {
      bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr();
      bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr();
      bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr();
      bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr();
      bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr();
      bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr(); bagr();
   }
}
int
main()
{
  t(1);
  t(2);
} 
/* Even if function is huge, inlining it will save code.  */
/* { dg-final { scan-ipa-dump-times "Inlined into" 2 "inline"  } } */
/* { dg-final { cleanup-ipa-dump "inline" } } */
