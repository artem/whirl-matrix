#pragma once

namespace whirl::matrix::clocks {

/* Value \in [-99, +inf]
 * Effect:
 * 0 - world time rate
 * -75 - x0.25
 * -50 - x0.5
 * +100 - x2
 * +200 - x3
 */

class Drift {
 public:
  Drift(int value) : drift_(value) {
  }

  // For now
  // Real time duration -> user duration
  Jiffies Elapsed(Jiffies real) const {
    return (real.Count() * (100 + drift_)) / 100;
  }

  // For sleeps/timeouts
  // User duration -> real time duration
  Jiffies SleepOrTimeout(Jiffies user) const {
    if (user.Count() == 0) {
      return 0;
    }
    uint64_t real = (user.Count() * 100) / (100 + drift_);
    return std::max<uint64_t>(real, 1);
  }

 private:
  int drift_;
};

}  // namespace whirl::matrix::clocks
