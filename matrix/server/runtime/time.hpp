#pragma once

#include <whirl/node/time/time_service.hpp>

#include <matrix/clocks/wall.hpp>
#include <matrix/clocks/monotonic.hpp>
#include <matrix/process/scheduler.hpp>

#include <await/futures/core/future.hpp>

namespace whirl::matrix {

//////////////////////////////////////////////////////////////////////

// Time service impl

class TimeService : public node::time::ITimeService {
 public:
  TimeService(clocks::WallClock& wall_clock,
              clocks::MonotonicClock& monotonic_clock,
              process::Scheduler& scheduler)
      : wall_clock_(wall_clock),
        monotonic_clock_(monotonic_clock),
        scheduler_(scheduler) {
  }

  node::time::WallTime WallTimeNow() override {
    return wall_clock_.Now();
  }

  node::time::MonotonicTime MonotonicNow() override {
    return monotonic_clock_.Now();
  }

  await::futures::Future<void> AfterJiffies(await::time::Jiffies d) override {
    // NB: await::time::Jiffies = whirl::Jiffies
    auto after = AfterRealTime(d);

    auto [f, p] = await::futures::MakeContract<void>();

    auto cb = [timer_promise = std::move(p)]() mutable {
      std::move(timer_promise).Set();
    };
    Schedule(scheduler_, after, std::move(cb));
    return std::move(f);
  }

 private:
  Jiffies ToRealTimeDelay(Jiffies delay) const {
    return monotonic_clock_.SleepOrTimeout(delay);
  }

  TimePoint AfterRealTime(Jiffies delay) const {
    return GlobalNow() + ToRealTimeDelay(delay).Count();
  }

 private:
  clocks::WallClock& wall_clock_;
  clocks::MonotonicClock& monotonic_clock_;

  process::Scheduler& scheduler_;
};

}  // namespace whirl::matrix
