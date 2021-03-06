// RUN: %clang_cc1 -fsyntax-only -verify %s

char *const_cast_test(const char *var)
{
  return const_cast<char*>(var);
}

struct A {
  virtual ~A() {}
};

struct B : public A {
};

struct B *dynamic_cast_test(struct A *a)
{
  return dynamic_cast<struct B*>(a);
}

char *reinterpret_cast_test()
{
  return reinterpret_cast<char*>(0xdeadbeef);
}

double static_cast_test(int i)
{
  return static_cast<double>(i);
}

char postfix_expr_test()
{
  return reinterpret_cast<char*>(0xdeadbeef)[0];
}

// This was being incorrectly tentatively parsed.
namespace test1 {
  template <class T> class A {}; // expected-note 2{{here}}
  void foo() { A<int>(*(A<int>*)0); }
}

typedef char* c;
typedef A* a;
void test2(char x, struct B * b) {
  (void)const_cast<::c>(&x);  // expected-error{{found '<::' after a const_cast which forms the digraph '<:' (aka '[') and a ':', did you mean '< ::'?}}
  (void)dynamic_cast<::a>(b);  // expected-error{{found '<::' after a dynamic_cast which forms the digraph '<:' (aka '[') and a ':', did you mean '< ::'?}}
  (void)reinterpret_cast<::c>(x);  // expected-error{{found '<::' after a reinterpret_cast which forms the digraph '<:' (aka '[') and a ':', did you mean '< ::'?}}
  (void)static_cast<::c>(&x);  // expected-error{{found '<::' after a static_cast which forms the digraph '<:' (aka '[') and a ':', did you mean '< ::'?}}

  // Do not do digraph correction.
  (void)static_cast<: :c>(&x); //\
       expected-error {{expected '<' after 'static_cast'}} \
       expected-error {{expected expression}}\
       expected-error {{expected ']'}}\
       expected-note {{to match this '['}}
  (void)static_cast<: // expected-error {{expected '<' after 'static_cast'}} \
                         expected-note {{to match this '['}}
  :c>(&x); // expected-error {{expected expression}} \
              expected-error {{expected ']'}}
#define LC <:
#define C :
  test1::A LC:B> c; // expected-error {{cannot refer to class template 'A' without a template argument list}} expected-error 2{{}} expected-note{{}}
  (void)static_cast LC:c>(&x); // expected-error {{expected '<' after 'static_cast'}} expected-error 2{{}} expected-note{{}}
  test1::A<:C B> d; // expected-error {{cannot refer to class template 'A' without a template argument list}} expected-error 2{{}} expected-note{{}}
  (void)static_cast<:C c>(&x); // expected-error {{expected '<' after 'static_cast'}} expected-error 2{{}} expected-note{{}}

#define LCC <::
  test1::A LCC B> e; // expected-error{{found '<::' after a template name which forms the digraph '<:' (aka '[') and a ':', did you mean '< ::'?}}
  (void)static_cast LCC c>(&x); // expected-error{{found '<::' after a static_cast which forms the digraph '<:' (aka '[') and a ':', did you mean '< ::'?}}
}
