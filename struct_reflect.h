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

#ifndef VIR_STRUCT_SIZE_H_
#define VIR_STRUCT_SIZE_H_
#if defined __cpp_structured_bindings && defined __cpp_concepts && __cpp_concepts >= 201907
#define VIR_HAVE_STRUCT_REFLECT 1
#include <utility>
#include <tuple>

namespace vir
{
namespace detail  //{{{1
{
using std::remove_cvref_t;
using std::size_t;

// struct_size implementation {{{2
template <class Struct> struct anything_but_base_of {
  template <class T>
  requires(!std::is_base_of_v<T, Struct>)
  operator T();
};

template <class Struct> struct any_empty_base_of {
  template <class T>
  requires(std::is_base_of_v<T, Struct> && std::is_empty_v<T>)
  operator T();
};

#define VIR_DEFINE_TEST_FUNCTIONS_(size_, ...)                                           \
  template <class T> constexpr auto test_##size_(const T *obj = nullptr)                 \
  {                                                                                      \
    [[maybe_unused]] const auto &[__VA_ARGS__] = *obj;                                   \
  }
VIR_DEFINE_TEST_FUNCTIONS_(26, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z);
VIR_DEFINE_TEST_FUNCTIONS_(25, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y);
VIR_DEFINE_TEST_FUNCTIONS_(24, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x);
VIR_DEFINE_TEST_FUNCTIONS_(23, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w);
VIR_DEFINE_TEST_FUNCTIONS_(22, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v);
VIR_DEFINE_TEST_FUNCTIONS_(21, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u);
VIR_DEFINE_TEST_FUNCTIONS_(20, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t);
VIR_DEFINE_TEST_FUNCTIONS_(19, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s);
VIR_DEFINE_TEST_FUNCTIONS_(18, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r);
VIR_DEFINE_TEST_FUNCTIONS_(17, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q);
VIR_DEFINE_TEST_FUNCTIONS_(16, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p);
VIR_DEFINE_TEST_FUNCTIONS_(15, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o);
VIR_DEFINE_TEST_FUNCTIONS_(14, a, b, c, d, e, f, g, h, i, j, k, l, m, n);
VIR_DEFINE_TEST_FUNCTIONS_(13, a, b, c, d, e, f, g, h, i, j, k, l, m);
VIR_DEFINE_TEST_FUNCTIONS_(12, a, b, c, d, e, f, g, h, i, j, k, l);
VIR_DEFINE_TEST_FUNCTIONS_(11, a, b, c, d, e, f, g, h, i, j, k);
VIR_DEFINE_TEST_FUNCTIONS_(10, a, b, c, d, e, f, g, h, i, j);
VIR_DEFINE_TEST_FUNCTIONS_( 9, a, b, c, d, e, f, g, h, i);
VIR_DEFINE_TEST_FUNCTIONS_( 8, a, b, c, d, e, f, g, h);
VIR_DEFINE_TEST_FUNCTIONS_( 7, a, b, c, d, e, f, g);
VIR_DEFINE_TEST_FUNCTIONS_( 6, a, b, c, d, e, f);
VIR_DEFINE_TEST_FUNCTIONS_( 5, a, b, c, d, e);
VIR_DEFINE_TEST_FUNCTIONS_( 4, a, b, c, d);
VIR_DEFINE_TEST_FUNCTIONS_( 3, a, b, c);
VIR_DEFINE_TEST_FUNCTIONS_( 2, a, b);
VIR_DEFINE_TEST_FUNCTIONS_( 1, a);
#undef VIR_DEFINE_TEST_FUNCTIONS_

template <class T, size_t... Indexes>
concept brace_constructible =
(
   requires { T{((void)Indexes, anything_but_base_of<T>())...}; }
|| requires { T{any_empty_base_of<T>(), ((void)Indexes, anything_but_base_of<T>())...}; }
|| requires { T{any_empty_base_of<T>(), any_empty_base_of<T>(), ((void)Indexes, anything_but_base_of<T>())...}; }
|| requires { T{any_empty_base_of<T>(), any_empty_base_of<T>(), any_empty_base_of<T>(), ((void)Indexes, anything_but_base_of<T>())...}; }
) && (
   (sizeof...(Indexes) ==  1 && requires { test_1 <T>(); })
|| (sizeof...(Indexes) ==  2 && requires { test_2 <T>(); }) 
|| (sizeof...(Indexes) ==  3 && requires { test_3 <T>(); }) 
|| (sizeof...(Indexes) ==  4 && requires { test_4 <T>(); }) 
|| (sizeof...(Indexes) ==  5 && requires { test_5 <T>(); }) 
|| (sizeof...(Indexes) ==  6 && requires { test_6 <T>(); }) 
|| (sizeof...(Indexes) ==  7 && requires { test_7 <T>(); }) 
|| (sizeof...(Indexes) ==  8 && requires { test_8 <T>(); }) 
|| (sizeof...(Indexes) ==  9 && requires { test_9 <T>(); }) 
|| (sizeof...(Indexes) == 10 && requires { test_10<T>(); }) 
|| (sizeof...(Indexes) == 11 && requires { test_11<T>(); }) 
|| (sizeof...(Indexes) == 12 && requires { test_12<T>(); }) 
|| (sizeof...(Indexes) == 13 && requires { test_13<T>(); }) 
|| (sizeof...(Indexes) == 14 && requires { test_14<T>(); }) 
|| (sizeof...(Indexes) == 15 && requires { test_15<T>(); }) 
|| (sizeof...(Indexes) == 16 && requires { test_16<T>(); }) 
|| (sizeof...(Indexes) == 17 && requires { test_17<T>(); }) 
|| (sizeof...(Indexes) == 18 && requires { test_18<T>(); }) 
|| (sizeof...(Indexes) == 19 && requires { test_19<T>(); }) 
|| (sizeof...(Indexes) == 20 && requires { test_20<T>(); }) 
|| (sizeof...(Indexes) == 21 && requires { test_21<T>(); }) 
|| (sizeof...(Indexes) == 22 && requires { test_22<T>(); }) 
|| (sizeof...(Indexes) == 23 && requires { test_23<T>(); })
|| (sizeof...(Indexes) == 24 && requires { test_24<T>(); })
|| (sizeof...(Indexes) == 25 && requires { test_25<T>(); })
|| (sizeof...(Indexes) == 26 && requires { test_26<T>(); })
);

template <class T, size_t... Indexes>
requires brace_constructible<T, Indexes...>
constexpr size_t struct_size(std::index_sequence<Indexes...>)
{
  return sizeof...(Indexes);
}

template <class T>
requires requires { T{}; }
constexpr size_t struct_size(std::index_sequence<>)
{
  static_assert(std::is_empty_v<T>,
                "Increase MaxSize on your struct_size call. (Or you found a bug)");
  return 0;
}

template <class T, size_t I0, size_t... Indexes>
requires(!brace_constructible<T, I0, Indexes...>)
constexpr size_t struct_size(std::index_sequence<I0, Indexes...>)
{
  return struct_size<T>(std::index_sequence<Indexes...>());
}

// struct_get implementation {{{2
template <size_t Total> struct struct_get;

template <typename T> struct remove_ref_in_tuple;
template <typename... Ts> struct remove_ref_in_tuple<std::tuple<Ts...>> {
  using type = std::tuple<std::remove_reference_t<Ts>...>;
};
template <typename T1, typename T2> struct remove_ref_in_tuple<std::pair<T1, T2>> {
  using type = std::pair<std::remove_reference_t<T1>, std::remove_reference_t<T2>>;
};

template <> struct struct_get<1> {
  template <class T> auto to_tuple_ref(T &&obj)
  {
    auto && [a] = obj;
    return std::forward_as_tuple(a);
  }
  template <class T>
  auto to_tuple(const T &obj) ->
      typename remove_ref_in_tuple<decltype(to_tuple_ref(std::declval<T>()))>::type
  {
    const auto &[a] = obj;
    return {a};
  }
  template <size_t N, class T> const auto &get(const T &obj)
  {
    auto && [a] = obj;
    return a;
  }
  template <size_t N, class T> auto &get(T &obj)
  {
    auto && [a] = obj;
return a;
  }
};

template <> struct struct_get<2> {
  template <class T> auto to_tuple_ref(T &&obj)
  {
    auto &&[a, b] = obj;
    return std::forward_as_tuple(a, b);
  }
  template <class T>
  auto to_tuple(const T &obj) ->
      typename remove_ref_in_tuple<decltype(to_tuple_ref(std::declval<T>()))>::type
  {
    const auto &[a, b] = obj;
    return {a, b};
  }
  template <class T> auto to_pair_ref(T &&obj)
  {
    auto &&[a, b] = obj;
    return std::pair<decltype((a)), decltype((b))>(a, b);
  }
  template <class T>
  auto to_pair(const T &obj) ->
      typename remove_ref_in_tuple<decltype(to_pair_ref(std::declval<T>()))>::type
  {
    const auto &[a, b] = obj;
    return {a, b};
  }
  template <size_t N, class T> auto &get(T &&obj)
  {
    auto &&[a, b] = obj;
    return std::get<N>(std::forward_as_tuple(a, b));
  }
};

#define VIR_STRUCT_GET_(size_, ...)                                                      \
  template <> struct struct_get<size_> {                                                 \
    template <class T> auto to_tuple_ref(T &&obj)                                        \
    {                                                                                    \
      auto &&[__VA_ARGS__] = obj;                                                        \
      return std::forward_as_tuple(__VA_ARGS__);                                         \
    }                                                                                    \
    template <class T>                                                                   \
    auto to_tuple(const T &obj) ->                                                       \
        typename remove_ref_in_tuple<decltype(to_tuple_ref(std::declval<T>()))>::type    \
    {                                                                                    \
      const auto &[__VA_ARGS__] = obj;                                                   \
      return {__VA_ARGS__};                                                              \
    }                                                                                    \
    template <size_t N, class T> auto &get(T &&obj)                                      \
    {                                                                                    \
      auto &&[__VA_ARGS__] = obj;                                                        \
      return std::get<N>(std::forward_as_tuple(__VA_ARGS__));                            \
    }                                                                                    \
  }
VIR_STRUCT_GET_(26, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z);
VIR_STRUCT_GET_(25, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y);
VIR_STRUCT_GET_(24, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x);
VIR_STRUCT_GET_(23, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w);
VIR_STRUCT_GET_(22, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v);
VIR_STRUCT_GET_(21, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u);
VIR_STRUCT_GET_(20, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t);
VIR_STRUCT_GET_(19, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s);
VIR_STRUCT_GET_(18, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r);
VIR_STRUCT_GET_(17, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q);
VIR_STRUCT_GET_(16, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p);
VIR_STRUCT_GET_(15, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o);
VIR_STRUCT_GET_(14, a, b, c, d, e, f, g, h, i, j, k, l, m, n);
VIR_STRUCT_GET_(13, a, b, c, d, e, f, g, h, i, j, k, l, m);
VIR_STRUCT_GET_(12, a, b, c, d, e, f, g, h, i, j, k, l);
VIR_STRUCT_GET_(11, a, b, c, d, e, f, g, h, i, j, k);
VIR_STRUCT_GET_(10, a, b, c, d, e, f, g, h, i, j);
VIR_STRUCT_GET_( 9, a, b, c, d, e, f, g, h, i);
VIR_STRUCT_GET_( 8, a, b, c, d, e, f, g, h);
VIR_STRUCT_GET_( 7, a, b, c, d, e, f, g);
VIR_STRUCT_GET_( 6, a, b, c, d, e, f);
VIR_STRUCT_GET_( 5, a, b, c, d, e);
VIR_STRUCT_GET_( 4, a, b, c, d);
VIR_STRUCT_GET_( 3, a, b, c);
#undef VIR_STRUCT_GET_

// concept definitions {{{2
template <typename T> concept has_tuple_size = requires(T x, std::tuple_size<T> ts)
{
  {static_cast<int>(ts)} -> std::same_as<int>;
  struct_get<std::tuple_size_v<T>>().to_tuple(x);
};
template <typename T>
concept aggregate_without_tuple_size = std::is_aggregate_v<T> && !has_tuple_size<T>;

// traits {{{2
template <typename From, typename To> struct copy_cvref {
  using From2 = std::remove_reference_t<From>;
  using with_const = std::conditional_t<std::is_const_v<From2>, const To, To>;
  using with_volatile =
      std::conditional_t<std::is_volatile_v<From2>, volatile with_const, with_const>;
  using type = std::conditional_t<std::is_lvalue_reference_v<From>, with_volatile &,
                                  std::conditional_t<std::is_rvalue_reference_v<From>,
                                                     with_volatile &&, with_volatile>>;
};
}  // namespace detail }}}1

/**
 * The value of struct_size is the number of non-static data members of the type \p T.
 * More precisely, struct_size is the number of elements in the identifier-list of a
 * structured binding declaration.
 *
 * \tparam T An aggregate type or a type specializing `std::tuple_size` that can be
 *           destructured via a structured binding. This implies that either \p T has only
 *           empty bases or \p T has no non-static data members and a single base class
 *           with non-static data members. The uses of aggregate types that do not support
 *           destructuring is ill-formed. The uses of non-aggregate types that do not
 *           support destructuring results in a substitution failure.
 *
 * \tparam MaxSize The maximum number of non-static data members to expect. The
 *                 implementation uses an upper limit of 26.
 */
template <typename T, std::size_t MaxSize = sizeof(T)>
requires detail::aggregate_without_tuple_size<T> || detail::has_tuple_size<T>
constexpr inline std::size_t struct_size;

template <detail::aggregate_without_tuple_size T, std::size_t MaxSize>
constexpr inline std::size_t struct_size<T, MaxSize> =
    detail::struct_size<T>(std::make_index_sequence<std::min(std::size_t(26), MaxSize)>());

template <detail::has_tuple_size T, std::size_t MaxSize>
constexpr inline std::size_t struct_size<T, MaxSize> = std::tuple_size_v<T>;

/**
 * Returns a cv-qualified reference to the \p N -th non-static data member in \p obj.
 */
template <std::size_t N, class T,
          std::size_t Total = struct_size<std::remove_cvref_t<T>>,
          class = std::enable_if_t<(N < Total)>>
decltype(auto) struct_get(T &&obj)
{
  return detail::struct_get<Total>().template get<N>(std::forward<T>(obj));
}

/**
 * `struct_element_t` is an alias for the type of the \p N -th non-static data member of
 * \p T.
 */
template <std::size_t N, class T>
using struct_element_t =
    std::remove_reference_t<decltype(struct_get<N>(std::declval<T &>()))>;

/**
 * Returns a std::tuple with a copy of all the non-static data members of \p obj.
 */
template <class T, std::size_t Total = struct_size<std::remove_cvref_t<T>>>
auto to_tuple(T &&obj)
{
  return detail::struct_get<Total>().to_tuple(std::forward<T>(obj));
}

/**
 * Returns a std::tuple of lvalue references to all the non-static data members of \p obj.
 */
template <class T, std::size_t Total = struct_size<std::remove_cvref_t<T>>>
auto to_tuple_ref(T &&obj)
{
  return detail::struct_get<Total>().to_tuple_ref(std::forward<T>(obj));
}

/**
 * Defines the member type \p type to a std::tuple specialization matching the non-static
 * data members of \p T.
 */
template <class T>
struct as_tuple
    : detail::copy_cvref<
          T, decltype(detail::struct_get<struct_size<std::remove_cvref_t<T>>>().to_tuple(
                 std::declval<T>()))> {
};

/**
 * Alias for a std::tuple specialization matching the non-static data members of \p T.
 */
template <class T> using as_tuple_t = typename as_tuple<T>::type;

/**
 * Returns a std::pair with a copy of all the non-static data members of \p obj.
 */
template <class T>
requires(struct_size<std::remove_cvref_t<T>> == 2)
auto to_pair(T &&obj)
{
  return detail::struct_get<2>().to_pair(std::forward<T>(obj));
}

/**
 * Returns a std::pair of lvalue references to all the non-static data members of \p obj.
 */
template <class T>
requires(struct_size<std::remove_cvref_t<T>> == 2)
auto to_pair_ref(T &&obj)
{
  return detail::struct_get<2>().to_pair_ref(std::forward<T>(obj));
}

/**
 * Defines the member type \p type to a std::pair specialization matching the non-static
 * data members of \p T.
 */
template <class T>
requires (struct_size<std::remove_cvref_t<T>> == 2)
struct as_pair
    : detail::copy_cvref<
          T, decltype(detail::struct_get<2>().to_pair(
                 std::declval<T>()))> {
};

/**
 * Alias for a std::pair specialization matching the non-static data members of \p T.
 */
template <class T> using as_pair_t = typename as_pair<T>::type;

}  // namespace vir

#endif // structured bindings & concepts
#endif  // VIR_STRUCT_SIZE_H_

// vim: foldmethod=marker
