// RUN: %clang_cc1 -fsyntax-only -verify -std=c++0x %s 

int f();

struct S 
{
  int a = f(); // ok
  int b = g(); // expected-error {{use of undeclared identifier 'g'}}
};
