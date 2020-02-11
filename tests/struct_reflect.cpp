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

#ifdef VIR_HAVE_STRUCT_REFLECT

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
  const volatile D d;
  const C &c;
};

struct F {
  int *a;
  const int *b;
  int *const c;
  const int *const d;
};

using G = int[3];

struct H1 { float a, b, c; };
struct H2 : public H1 {};
struct Empty1 {};
struct Empty2 {};
struct H3 : Empty1, Empty2 { int a, b; };
struct H4 { H1 a, b; };
struct H5 { Empty1 a, b; };
struct H6 {
  Empty1 a, b, c, d, e, f, g, h, i, j;
  Empty1 &k;
};
struct Bitfield {
  const int a : 1;
  const int b : 1;
  const int c : 1;
  const int d : 1;
  const int e : 1;
  const int f : 1;
  const int g : 1;
};

TEST(struct_size)
{
  COMPARE(vir::struct_size<Empty1>, 0u);
  COMPARE(vir::struct_size<A>, 1u);
  COMPARE(vir::struct_size<B>, 1u);
  COMPARE(vir::struct_size<C>, 1u);
  COMPARE(vir::struct_size<D>, 1u);
  COMPARE(vir::struct_size<E>, 4u);
  COMPARE(vir::struct_size<F>, 4u);
  COMPARE(vir::struct_size<G>, 3u);
  COMPARE(vir::struct_size<H1>, 3u);
  COMPARE(vir::struct_size<H2>, 3u);
  COMPARE(vir::struct_size<H3>, 2u);
  COMPARE(vir::struct_size<H4>, 2u);
  COMPARE(vir::struct_size<H5>, 2u);
  COMPARE(vir::struct_size<H6>, 11u);
  COMPARE(vir::struct_size<Bitfield>, 7u);
  COMPARE((vir::struct_size<std::pair<int, float>>), 2u);
  COMPARE((vir::struct_size<std::tuple<int, float, int>>), 3u);
}

TEST(as_tuple_t)
{
  COMPARE_TYPES(vir::as_tuple_t<Empty1>, std::tuple<>);
  COMPARE_TYPES(vir::as_tuple_t<A>, std::tuple<int>);
  COMPARE_TYPES(vir::as_tuple_t<const A>, const std::tuple<int>);
  COMPARE_TYPES(vir::as_tuple_t<B>, std::tuple<std::array<int, 2>>);
  COMPARE_TYPES(vir::as_tuple_t<C>, std::tuple<decltype(C().c)>);
  COMPARE_TYPES(vir::as_tuple_t<D>, std::tuple<decltype(D().c)>);
  COMPARE_TYPES(vir::as_tuple_t<F>, std::tuple<int *, const int *, int *const, const int *const>);
  COMPARE_TYPES(vir::as_tuple_t<const F>, const std::tuple<int *, const int *, int *const, const int *const>);
  COMPARE_TYPES(vir::as_tuple_t<Bitfield>,
                std::tuple<const int, const int, const int, const int, const int,
                           const int, const int>);
  COMPARE_TYPES(vir::as_tuple_t<std::pair<int, const int>>, std::tuple<int, const int>);
  COMPARE_TYPES(decltype(vir::to_tuple_ref(std::pair<int, const int>())), std::tuple<int&, const int&>);
}

TEST(as_pair_t)
{
  COMPARE_TYPES(vir::as_pair_t<H3>, std::pair<int, int>);
  COMPARE_TYPES(vir::as_pair_t<H4>, std::pair<H1, H1>);
  COMPARE_TYPES(vir::as_pair_t<H5>, std::pair<Empty1, Empty1>);
}

TEST(struct_get)
{
  COMPARE_TYPES(decltype(vir::struct_get<0>(A())), const int&);
  COMPARE_TYPES(decltype(vir::struct_get<0>(std::declval<A&>())), int&);
  COMPARE_TYPES(decltype(vir::struct_get<0>(std::declval<A&>())), int&);
  COMPARE_TYPES(decltype(vir::struct_get<0>(B())), const std::array<int, 2> &);
  A a;
  COMPARE_TYPES(decltype(vir::struct_get<0>(a)), int &);

  E e{{}, {}, {}, {}};
  const E &ce = e;
  COMPARE_TYPES(decltype(vir::struct_get<0>(ce)), const A &);
  COMPARE_TYPES(decltype(vir::struct_get<1>(ce)), const volatile B &);
  COMPARE_TYPES(decltype(vir::struct_get<2>(ce)), const volatile D &);
  COMPARE_TYPES(decltype(vir::struct_get<3>(ce)), const C &);

  COMPARE_TYPES(decltype(vir::struct_get<0>(e)), A &);
  COMPARE_TYPES(decltype(vir::struct_get<1>(e)), volatile B &);
  COMPARE_TYPES(decltype(vir::struct_get<2>(e)), const volatile D &);
  COMPARE_TYPES(decltype(vir::struct_get<3>(e)), const C &);

  H2 h2{};
  COMPARE_TYPES(decltype(vir::struct_get<0>(h2)), float &);

  //H3 h3{};
  //COMPARE_TYPES(decltype(vir::struct_get<0>(h3)), int &);
  //COMPARE_TYPES(decltype(vir::struct_get<1>(h3)), int &);
}

TEST(struct_element)
{
  COMPARE_TYPES(vir::struct_element_t<0, E>, A);
  COMPARE_TYPES(vir::struct_element_t<1, E>, volatile B);
  COMPARE_TYPES(vir::struct_element_t<2, E>, const volatile D);
  COMPARE_TYPES(vir::struct_element_t<3, E>, const C);

  COMPARE_TYPES(vir::struct_element_t<0, F>, int *);
  COMPARE_TYPES(vir::struct_element_t<1, F>, const int *);
  COMPARE_TYPES(vir::struct_element_t<2, F>, int *const);
  COMPARE_TYPES(vir::struct_element_t<3, F>, const int *const);

  COMPARE_TYPES(vir::struct_element_t<0, G>, int);
  COMPARE_TYPES(vir::struct_element_t<1, G>, int);
  COMPARE_TYPES(vir::struct_element_t<2, G>, int);
}

TEST(to_tuple)
{
  {
    A a{1};
    std::tuple<int> tup_a1 = vir::to_tuple(a);
    std::tuple<int &> tup_a2 = vir::to_tuple_ref(a);
    std::tuple<const int &> tup_a3 = vir::to_tuple_ref(a);
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
    std::tuple<int> tup1 = vir::to_tuple(a);
    std::tuple<const int &> tup2 = vir::to_tuple(a);
    COMPARE(std::get<0>(tup1), 2);
    COMPARE(std::get<0>(tup2), 2);
    COMPARE_TYPES(std::tuple_element_t<0, decltype(vir::to_tuple(a))>, int);
    COMPARE_TYPES(decltype(vir::to_tuple(a)), std::tuple<int>);
    COMPARE_TYPES(decltype(vir::to_tuple_ref(a)), std::tuple<const int &>);
  }

  {
    F f{0, 0, 0, 0};
    int tmp = 1;
    std::get<0>(vir::to_tuple_ref(f)) = &tmp;
    COMPARE(f.a, &tmp);
    COMPARE(std::get<0>(vir::to_tuple(f)), &tmp);
  }

  {
    G g{0, 2, 3};
    std::get<0>(vir::to_tuple_ref(g)) = 1;
    COMPARE(g[0], 1);
    const auto &[a, b, c] = vir::to_tuple_ref(g);
    COMPARE(a, 1);
    COMPARE(b, 2);
    COMPARE(c, 3);
    g[0] = 4;
    g[1] = 5;
    g[2] = 6;
    COMPARE(a, 4);
    COMPARE(b, 5);
    COMPARE(c, 6);
  }

  {
    H3 h3{{}, {}, 1, 2};
    auto [a, b] = vir::to_tuple_ref(h3);
    COMPARE(a, 1);
    a = 3;
    COMPARE(h3.a, 3);
    COMPARE(b, 2);
    COMPARE_TYPES(decltype(a), int &);
    COMPARE_TYPES(decltype(b), int &);
    const H3& cref = h3;
    auto [c, d] = vir::to_tuple_ref(cref);
    auto [e, f] = vir::to_tuple(h3);
    COMPARE(c, 3);
    COMPARE(e, 3);
    a = 1;
    COMPARE(c, 1);
    COMPARE(e, 3);
    COMPARE(d, 2);
    COMPARE(f, 2);
  }
}

TEST(to_pair)
{
  H3 h3{{}, {}, 1, 2};
  COMPARE_TYPES(decltype(vir::to_pair_ref(h3)), std::pair<int&, int&>);
  auto [a, b] = vir::to_pair_ref(h3);
  COMPARE(a, 1);
  a = 3;
  COMPARE(h3.a, 3);
  COMPARE(b, 2);
  COMPARE_TYPES(decltype(a), int &);
  COMPARE_TYPES(decltype(b), int &);
  const H3 &cref = h3;
  auto [c, d] = vir::to_pair_ref(cref);
  COMPARE_TYPES(decltype(vir::to_pair_ref(cref)), std::pair<const int&, const int&>);
  auto [e, f] = vir::to_pair(h3);
  COMPARE(c, 3);
  COMPARE(e, 3);
  a = 1;
  COMPARE(c, 1);
  COMPARE(e, 3);
  COMPARE(d, 2);
  COMPARE(f, 2);
}

#endif // VIR_HAVE_STRUCT_REFLECT
