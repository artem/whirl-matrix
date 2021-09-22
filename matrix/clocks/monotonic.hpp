#pragma once

#include <whirl/node/time/monotonic_time.hpp>

#include <matrix/clocks/drift.hpp>

#include <matrix/world/global/time.hpp>
#include <matrix/world/global/time_model.hpp>

namespace whirl::matrix::clocks {

class MonotonicClock {
 public:
  MonotonicClock() = default;

  void Init() {
    drift_ = ThisServerTimeModel()->InitClockDrift();
    Reset();
  }

  void Reset() {
    init_ = ThisServerTimeModel()->ResetMonotonicClock();
    last_reset_ = GlobalNow();
  }

  node::time::MonotonicTime Now() const {
    return drift_.Elapsed(ElapsedSinceLastReset()) + init_;
  }

  // For timeouts and sleeps
  Jiffies SleepOrTimeout(Jiffies d) const {
    return drift_.SleepOrTimeout(d);
  }

 private:
  // Global time
  Jiffies ElapsedSinceLastReset() const {
    return GlobalNow() - last_reset_;
  }

 private:
  Drift drift_{0};
  TimePoint last_reset_{0};
  TimePoint init_{0};
};

}  // namespace whirl::matrix::clocks
