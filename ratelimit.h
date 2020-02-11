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

#ifndef VIR_RATELIMIT_H_
#define VIR_RATELIMIT_H_

#include <algorithm>
#include <cassert>
#include <chrono>
#include <thread>

namespace vir
{
/**
 * Objects of type RateLimiter can be used to limit a loop to a given rate of iterations per second.
 *
 * Example:
 * \code
 * RateLimiter limit(100);  // 100 Hz
 * while (do_more_work()) {
 *   work();
 *   limit.maybe_sleep();   // this needs to be at the end of the loop for a
 *                          // correct time measurement of the first iterations
 * }
 * \endcode
 */
class RateLimiter
{
  using clock = std::chrono::steady_clock;

public:
  /**
   * Constructs a rate limiter.
   *
   * \param rate Work rate in Hz (calls to maybe_sleep per second). Values less than/equal
   *             to 0 set the rate to 1 GHz (which is impossible to achieve, even with a
   *             loop that only calls RateLimiter::maybe_sleep).
   */
  RateLimiter(float rate) : tw_req(std::chrono::seconds(1)), start_time(clock::now())
  {
    if (rate <= 0) {
      tw_req = std::chrono::nanoseconds(1);
    } else {
      tw_req = std::chrono::duration_cast<clock::duration>(tw_req / rate);
    }
    skip_check_count = std::max(1, int(std::chrono::milliseconds(5) / tw_req));
    count = skip_check_count;
  }

  /**
   * Call this function at the end of the iteration rate limited loop.
   *
   * This function might use `std::this_thread::sleep_for` to limit the iteration rate. If no sleeps
   * are necessary, the function will back off checking for the time to further allow increased
   * iteration rates (until the requested rate or 1s between rechecks is reached).
   */
  void maybe_sleep()
  {
    using namespace std::chrono;
    if (--count == 0) {
      auto now = clock::now();
      if (tw == clock::duration::zero()) {
        tw = (now - start_time) / skip_check_count;
      } else {
        tw = (1 * tw + 3 * (now - start_time) / skip_check_count) / 4;
      }
      if (tw > tw_req * 257 / 256) {
        // the time between maybe_sleep calls is ~.4% too long
        // fix it by reducing ts towards 0 and if ts = 0 doesn't suffice, increase
        // skip_check_count
        if (ts > clock::duration::zero()) {
          ts = std::max(clock::duration::zero(),
                        ts - (tw - tw_req) * skip_check_count * 1 / 2);
        } else {
          skip_check_count =
              std::min(int(seconds(1) / tw_req),  // recheck at least every second
                       (skip_check_count * 5 + 3) / 4);
        }
      } else if (tw < tw_req * 63 / 64) {
        // the time between maybe_sleep calls is ~1.6% too short
        // fix it by reducing skip_check_count towards 1 and if skip_check_count = 1
        // doesn't suffice, increase ts

        // The minimum work count is defined such that a typical sleep time is greater
        // than 1ms.
        // The user requested 1/tw_req work iterations per second. Divided by 1000, that's
        // the count per ms.
        const int min_skip_count = std::max(1, int(milliseconds(5) / tw_req));
        if (skip_check_count > min_skip_count) {
          assert(ts == clock::duration::zero());
          skip_check_count = std::max(min_skip_count, skip_check_count * 3 / 4);
        } else {
          ts += (tw_req - tw) * (skip_check_count * 7) / 8;
        }
      }

      start_time = now;
      count = skip_check_count;
      if (ts > clock::duration::zero()) {
        std::this_thread::sleep_for(ts);
      }
    }
  }

private:
  clock::duration tw{},  //! deduced duration between maybe_sleep calls
      ts{},              //! sleep duration
      tw_req;            //! requested duration between maybe_sleep calls
  clock::time_point start_time;
  int count = 1;
  int skip_check_count = 1;
};

}  // namespace vir

#endif  // VIR_RATELIMIT_H_
