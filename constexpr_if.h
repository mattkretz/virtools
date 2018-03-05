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

#ifndef VIR_CONSTEXPR_IF_H_
#define VIR_CONSTEXPR_IF_H_

#include <utility>
#include <type_traits>

namespace vir
{
namespace detail
{
template <class IfFun, class ElseFun>
inline auto impl_or_fallback_dispatch(std::true_type, IfFun &&true_fun, ElseFun &&)
{
  return true_fun(0);
}

template <class IfFun, class ElseFun>
inline auto impl_or_fallback_dispatch(std::false_type, IfFun &&, ElseFun &&else_fun)
{
  return else_fun(0);
}
}  // namespace detail

template <bool Condition, class IfFun, class ElseFun>
inline auto constexpr_if(IfFun &&if_fun, ElseFun &&else_fun)
{
  return ::vir::detail::impl_or_fallback_dispatch(
      std::integral_constant<bool, Condition>(), if_fun, else_fun);
}

template <bool Condition, class IfFun>
inline auto constexpr_if(IfFun &&if_fun)
{
  return ::vir::detail::impl_or_fallback_dispatch(
      std::integral_constant<bool, Condition>(), if_fun, [](int) {});
}

template <bool Condition, bool Condition2, class IfFun, class... Remainder>
inline auto constexpr_if(IfFun &&if_fun, std::integral_constant<bool, Condition2>,
                         Remainder &&... rem)
{
  return ::vir::detail::impl_or_fallback_dispatch(
      std::integral_constant<bool, Condition>(), if_fun, [&](auto tmp_) {
        return ::vir::constexpr_if<(std::is_same<decltype(tmp_), int>::value &&
                                    Condition2)>(rem...);
      });
}

}  // namespace vir

#ifdef __cpp_if_constexpr
#define Vir_CONSTEXPR_IF_RETURNING(condition_) if constexpr (condition_) {
#define Vir_CONSTEXPR_IF(condition_) if constexpr (condition_) {
#define Vir_CONSTEXPR_ELSE_IF(condition_) } else if constexpr (condition_) {
#define Vir_CONSTEXPR_ELSE } else {
#define Vir_CONSTEXPR_ENDIF }
#else
#define Vir_CONSTEXPR_IF_RETURNING(condition_) return vir::constexpr_if<(condition_)>([&](auto) {
#define Vir_CONSTEXPR_IF(condition_) vir::constexpr_if<(condition_)>([&](auto) {
#define Vir_CONSTEXPR_ELSE_IF(condition_) }, std::integral_constant<bool, (condition_)>(), [&](auto) {
#define Vir_CONSTEXPR_ELSE }, [&](auto) {
#define Vir_CONSTEXPR_ENDIF });
#endif

#endif  // VIR_CONSTEXPR_IF_H_
