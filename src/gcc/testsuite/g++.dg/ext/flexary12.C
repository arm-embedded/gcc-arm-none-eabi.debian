// PR c++/69290 - [6 Regression] g++ ICE on invalid initialization
//     of a flexible array member
// { dg-do compile }

// Suppress pedantic errors about initialization of a flexible array member.
// { dg-options "-Wno-pedantic" }

struct A {
  int a [];  // { dg-error "flexible array member .A::a. in an otherwise empty .struct A." }
};

void f1 ()
{
  // This is the meat of the test from c++/69290:
  struct A a
    = { "c" };   // { dg-error "invalid conversion from .const char\\*. to .int." }

  (void)&a;
}


// Exercise other forms of invalid initialization besides the one in the bug.
struct B {
  int n;
  int a [];
};

void f2 ()
{
  struct B b1
    = { 0, "c" };   // { dg-error "invalid conversion from .const char\\*. to .int." }

  (void)&b1;

  const char s[] = "c";
  struct B b2
    = { 0, s };   // { dg-error "invalid conversion from .const char\\*. to .int." }

  (void)&b2;
}

struct D {
  int a [];  // { dg-error "flexible array member .D::a. in an otherwise empty .struct D." }
  D ();
};

D::D ():    // { dg-error "member initializer for flexible array member" }
  a ("c")   // { dg-error "incompatible types in assignment of .const char \\\[2\\\]. to .int \\\[\\\]." }
{ }


template <class T>
struct C {
  T a [];  // { dg-error "flexible array member" }
};

void f3 ()
{
  struct C<double> cd
    = { "c" };   // { dg-error "cannot convert .const char\\*. to .double." }

  (void)&cd;
}
