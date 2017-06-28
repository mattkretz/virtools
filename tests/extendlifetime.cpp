/*{{{
Copyright © 2017 Matthias Kretz <kretz@kde.org>

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
#include "../extendlifetime.h"
#include <vir/metahelpers.h>

static int ndctor = 0;
static int ncctor = 0;
static int nmctor = 0;
static int ncass = 0;
static int nmass = 0;
static int ndtor = 0;

struct LifetimeChecker {
  LifetimeChecker() { ++ndctor; }
  LifetimeChecker(const LifetimeChecker &) { ++ncctor; }
  LifetimeChecker(LifetimeChecker &&) { ++nmctor; }
  LifetimeChecker &operator=(const LifetimeChecker &) { ++ncass; return *this; }
  LifetimeChecker &operator=(LifetimeChecker &&) { ++nmass; return *this; }
  ~LifetimeChecker() { ++ndtor; }
};

inline void check(int dctor, int cctor, int mctor, int cass, int mass, int dtor)
{
  COMPARE(ndctor, dctor);
  COMPARE(ncctor, cctor);
  COMPARE(nmctor, mctor);
  COMPARE(nmass, mass);
  COMPARE(ncass, cass);
  COMPARE(ndtor, dtor);
}

using vir::ExtendLifetime;

TEST(foo)
{
  {
    std::vector<ExtendLifetime> zombies;
    {
      LifetimeChecker c1;
      check(1, 0, 0, 0, 0, 0);
      zombies.emplace_back(std::move(c1));
      check(1, 0, 1, 0, 0, 0);
    }
    check(1, 0, 1, 0, 0, 1);
  }
  check(1, 0, 1, 0, 0, 2);
}

TEST(traits)
{
  VERIFY(!std::is_copy_constructible<ExtendLifetime>::value);
  VERIFY( std::is_move_constructible<ExtendLifetime>::value);
  VERIFY( std::is_nothrow_move_constructible<ExtendLifetime>::value);
  VERIFY( std::is_nothrow_default_constructible<ExtendLifetime>::value);

  VERIFY(vir::test::sfinae_is_callable<LifetimeChecker &&>(
      [](auto &&x) -> decltype(ExtendLifetime(std::forward<decltype(x)>(x))) {
        return {};
      }));
  VERIFY(!vir::test::sfinae_is_callable<LifetimeChecker &>(
      [](auto &&x) -> decltype(ExtendLifetime(std::forward<decltype(x)>(x))) {
        return {};
      }));
  VERIFY(vir::test::sfinae_is_callable<LifetimeChecker>(
      [](auto &&x) -> decltype(ExtendLifetime(std::forward<decltype(x)>(x))) {
        return {};
      }));
}
