#pragma once

#include <matrix/time/time_point.hpp>

#include <wheels/support/assert.hpp>

namespace whirl::matrix {

// Virtual time

class Time {
 public:
  Time() = default;

  // Non-copyable
  Time(const Time&) = delete;
  Time& operator=(const Time&) = delete;

  // Non-movable
  Time(Time&&) = delete;
  Time& operator=(Time&&) = delete;

  TimePoint Now() const {
    return now_;
  }

  void FastForwardTo(TimePoint future) {
    WHEELS_VERIFY(future >= now_, "Cannot fast-forward time: now = "
                                      << now_ << " -> " << future);
    now_ = future;
  }

 private:
  TimePoint now_{0};
};

}  // namespace whirl::matrix
