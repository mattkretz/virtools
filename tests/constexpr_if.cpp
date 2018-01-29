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
#include "../constexpr_if.h"

template <class T> int foo()
{
  Vir_CONSTEXPR_IF_RETURNING(T::value == 1) { return 1; }
  Vir_CONSTEXPR_ELSE_IF(T::value == 2) { return 2; }
  Vir_CONSTEXPR_ELSE_IF(T::value == 1) { return 3; }
  Vir_CONSTEXPR_ELSE_IF(T::value == 0) { return 0; }
  Vir_CONSTEXPR_ELSE { return 4; }
  Vir_CONSTEXPR_ENDIF
}

TEST_TYPES(T, foo, std::integral_constant<int, 0>, std::integral_constant<int, 1>,
           std::integral_constant<int, 2>)
{
  Vir_CONSTEXPR_IF(T::value == 1)
  {
      COMPARE(T::value, 1);
      static_assert(T::value == 1, "");
  }
  Vir_CONSTEXPR_ENDIF

  Vir_CONSTEXPR_IF(T::value == 1)
  {
      COMPARE(T::value, 1);
      static_assert(T::value == 1, "");
  }
  Vir_CONSTEXPR_ELSE_IF(T::value == 2)
  {
      COMPARE(T::value, 2);
      static_assert(T::value == 2, "");
  }
  Vir_CONSTEXPR_ELSE_IF(T::value == 1)
  {
      FAIL() << "this must be unreachable";
      static_assert(T::value == 2, "this must be unreachable at compile time");
  }
  Vir_CONSTEXPR_ELSE
  {
      COMPARE(T::value, 0);
      static_assert(T::value == 0, "");
  }
  Vir_CONSTEXPR_ENDIF

  COMPARE(foo<T>(), T::value);
}
