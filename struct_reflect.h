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
#include <utility>

namespace vir
{
namespace detail  //{{{1
{
using std::size_t;

// struct_size implementation {{{2
struct anything {
  template <class T> operator T();
};

template <class T, size_t... Indexes, class = decltype(T{((void)Indexes, anything())...})>
constexpr size_t struct_size(std::index_sequence<Indexes...>, int)
{
  return sizeof...(Indexes);
}

template <class T, size_t... Indexes>
constexpr size_t struct_size(std::index_sequence<Indexes...>, float)
{
  static_assert(sizeof...(Indexes) > 0,
                "Increase MaxSize on your struct_size call. (Or you found a bug)");
  return struct_size<T>(std::make_index_sequence<sizeof...(Indexes) - 1>(), int());
}

// struct_get implementation {{{2
template <size_t Total> struct struct_get;

#ifdef __cpp_structured_bindings
template <> struct struct_get<1> {
  template <class T> auto as_tuple(T &&obj)
  {
    auto && [a] = std::forward<T>(obj);
    return std::tie(a);
  }
  template <size_t N, class T> auto &get(T &&obj)
  {
    auto && [a] = std::forward<T>(obj);
    auto &b = a;
    return b;
  }
};

#define VIR_STRUCT_GET_(size_, ...)                                                      \
  template <> struct struct_get<size_> {                                                 \
    template <class T> auto as_tuple(T &&obj)                                            \
    {                                                                                    \
      auto && [__VA_ARGS__] = std::forward<T>(obj);                                      \
      return std::tie(__VA_ARGS__);                                                      \
    }                                                                                    \
    template <size_t N, class T> auto &get(T &&obj)                                      \
    {                                                                                    \
      auto && [__VA_ARGS__] = std::forward<T>(obj);                                      \
      return std::get<N>(std::tie(__VA_ARGS__));                                         \
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
VIR_STRUCT_GET_( 2, a, b);
#undef VIR_STRUCT_GET_
#endif  // __cpp_structured_bindings

}  // namespace detail }}}1

/**
 * The value of struct_size is the number of non-static data members of the type \p T.
 *
 * \tparam T A standard-layout structure type.
 */
template <class T, std::size_t MaxSize = sizeof(T),
          class = typename std::enable_if<std::is_class<T>::value &&
                                          std::is_standard_layout<T>::value>::type>
constexpr std::size_t struct_size =
    detail::struct_size<T>(std::make_index_sequence<MaxSize>(), int());

/**
 * Returns a cv-qualified reference to the \p N -th non-static data member in \p obj.
 */
template <std::size_t N, class T, std::size_t Total = struct_size<std::decay_t<T>>,
          class = std::enable_if_t<(N < Total)>>
auto &struct_get(T &&obj)
{
#ifndef __cpp_structured_bindings
  static_assert(N > Total, "struct_get requires structured bindings (C++17)");
#endif
  return detail::struct_get<Total>().template get<N>(std::forward<T>(obj));
}

/**
 * An alias for the type of the \p N -th non-static data member of \p T.
 */
template <std::size_t N, class T>
using struct_element_t =
    std::remove_reference_t<decltype(struct_get<N>(std::declval<T &>()))>;

/**
 * Returns a std::tuple of lvalue references to all the non-static data members of \p obj.
 */
template <class T, std::size_t Total = struct_size<std::decay_t<T>>>
auto as_tuple(T &&obj)
{
  return detail::struct_get<Total>().as_tuple(std::forward<T>(obj));
}

}  // namespace vir

#endif  // VIR_STRUCT_SIZE_H_

// vim: foldmethod=marker
