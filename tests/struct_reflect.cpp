/*{{{
Copyright © 2018 Matthias Kretz <kretz@kde.org>

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the names of contributing organizations nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

}}}*/

#include <vir/test.h>
#include "../struct_reflect.h"
#include <vir/metahelpers.h>

struct A {
    int a;
};

struct B {
    std::array<int, 2> a;
};

struct C {
  struct {
    int a, b;
  } c;
};

struct D {
  union {
    int a, b;
  } c;
};

struct E {
  A a;
  volatile B b;
  const C c;
  const volatile D d;
};

struct F {
  int *a;
  const int *b;
  int *const c;
  const int *const d;
};

TEST(struct_size)
{
  COMPARE(vir::struct_size<A>, 1u);
  COMPARE(vir::struct_size<B>, 1u);
  COMPARE(vir::struct_size<C>, 1u);
  COMPARE(vir::struct_size<D>, 1u);
  COMPARE(vir::struct_size<E>, 4u);
  COMPARE(vir::struct_size<F>, 4u);
  COMPARE((vir::struct_size<std::array<int, 4>>), 4u);
}

#ifdef __cpp_structured_bindings
TEST(struct_get)
{
  COMPARE(typeid(vir::struct_get<0>(A())), typeid(const int &));
  COMPARE(typeid(vir::struct_get<0>(B())), typeid(const std::array<int, 2> &));
  A a;
  COMPARE(typeid(vir::struct_get<0>(a)), typeid(int &));

  E e{{}, {}, {}, {}};
  const E &ce = e;
  COMPARE(typeid(vir::struct_get<0>(ce)), typeid(const A &));
  COMPARE(typeid(vir::struct_get<1>(ce)), typeid(const volatile B &));
  COMPARE(typeid(vir::struct_get<2>(ce)), typeid(const C &));
  COMPARE(typeid(vir::struct_get<3>(ce)), typeid(const volatile D &));

  COMPARE(typeid(vir::struct_get<0>(e)), typeid(A &));
  COMPARE(typeid(vir::struct_get<1>(e)), typeid(volatile B &));
  COMPARE(typeid(vir::struct_get<2>(e)), typeid(const C &));
  COMPARE(typeid(vir::struct_get<3>(e)), typeid(const volatile D &));
}

TEST(struct_element)
{
  COMPARE(typeid(vir::struct_element_t<0, E>), typeid(A));
  COMPARE(typeid(vir::struct_element_t<1, E>), typeid(volatile B));
  COMPARE(typeid(vir::struct_element_t<2, E>), typeid(const C));
  COMPARE(typeid(vir::struct_element_t<3, E>), typeid(const volatile D));

  COMPARE(typeid(vir::struct_element_t<0, F>), typeid(int *));
  COMPARE(typeid(vir::struct_element_t<1, F>), typeid(const int *));
  COMPARE(typeid(vir::struct_element_t<2, F>), typeid(int *const));
  COMPARE(typeid(vir::struct_element_t<3, F>), typeid(const int *const));
}

TEST(as_tuple)
{
  {
    A a{1};
    std::tuple<int> tup_a1 = vir::as_tuple(a);
    std::tuple<int &> tup_a2 = vir::as_tuple(a);
    std::tuple<const int &> tup_a3 = vir::as_tuple(a);
    COMPARE(a.a, 1);
    COMPARE(std::get<0>(tup_a1), 1);
    COMPARE(std::get<0>(tup_a2), 1);
    COMPARE(std::get<0>(tup_a3), 1);
    a.a = 2;
    COMPARE(a.a, 2);
    COMPARE(std::get<0>(tup_a1), 1);
    COMPARE(std::get<0>(tup_a2), 2);
    COMPARE(std::get<0>(tup_a3), 2);
    std::get<0>(tup_a2) = 3;
    COMPARE(a.a, 3);
    COMPARE(std::get<0>(tup_a1), 1);
    COMPARE(std::get<0>(tup_a2), 3);
    COMPARE(std::get<0>(tup_a3), 3);
  }

  {
    const A a{2};
    std::tuple<int> tup1 = vir::as_tuple(a);
    std::tuple<const int &> tup2 = vir::as_tuple(a);
    COMPARE(std::get<0>(tup1), 2);
    COMPARE(std::get<0>(tup2), 2);
    COMPARE(typeid(std::tuple_element_t<0, decltype(vir::as_tuple(a))>), typeid(int));
    COMPARE(typeid(vir::as_tuple(a)), typeid(std::tuple<const int &>));
  }

  {
    F f{0, 0, 0, 0};
    int tmp = 1;
    std::get<0>(vir::as_tuple(f)) = &tmp;
    COMPARE(f.a, &tmp);
    COMPARE(std::get<0>(vir::as_tuple(f)), &tmp);
  }
}
#endif  // __cpp_structured_bindings
