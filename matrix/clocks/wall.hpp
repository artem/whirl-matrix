#pragma once

#include <whirl/node/time/wall_time.hpp>

#include <matrix/world/global/time.hpp>

#include <matrix/time_model/time_model.hpp>

namespace whirl::matrix::clocks {

class WallClock {
 public:
  WallClock() = default;

  void Init() {
    offset_ = InitOffset();
  }

  void AdjustOffset() {
    // This action does not affect active timers:
    // they rely on monotonic clock
    offset_ = InitOffset();
  }

  node::time::WallTime Now() const {
    return GlobalNow() + offset_;
  }

 private:
  static Jiffies InitOffset() {
    return ThisServerTimeModel()->InitWallClockOffset();
  }

 private:
  Jiffies offset_{0};
};

}  // namespace whirl::matrix::clocks
