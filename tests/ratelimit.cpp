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

#include "../ratelimit.h"
#include <atomic>
#include <iostream>
#include <thread>
#ifdef _MSC_VER
#include <intrin.h>
#endif

int main() {
  for (int tw : {0, 10, 1000, 100000}) {
    std::atomic<unsigned> count = {};
    std::atomic<bool> quit = false;

    std::thread rate_checker([&count, &quit]() {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      while (true) {
        const unsigned lastcount = count;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        const unsigned elapsed = count - lastcount;
        if (quit) {
          return;
        }
        std::cout << elapsed << " Hz\n" << std::flush;
      }
    });

    for (float rate : {0.1f, 1.f, 1.5f, 10.f, 100.f, 1000.f, 10000.f, 100000.f, 1000000.f,
                       10000000.f}) {
      // consider tw as ~ns
      if (tw >= 1'000'000'000ll / rate) {
        continue;
      }
      std::cerr << "requested rate: " << rate << " Hz, work loop: " << tw << '\n';

      vir::RateLimiter limiter(rate);
      while (count < rate * 20) {
        ++count;
        for (int i = tw; i; --i) {
#ifdef _MSC_VER
          __nop();
#else
          asm volatile("nop");
#endif
        }
        limiter.maybe_sleep();
      }
    }

    quit = true;
    rate_checker.join();
  }
  return 0;
}
