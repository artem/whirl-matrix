#pragma once

#include <whirl/node/time/true_time_service.hpp>

#include <matrix/world/global/time.hpp>
#include <matrix/world/global/time_model.hpp>

namespace whirl::matrix {

class TrueTimeService : public node::time::ITrueTimeService {
 public:
  node::time::TTInterval Now() const override {
    // TODO: better

    // Access world clock
    TimePoint now = GlobalNow();

    auto u = ThisServerTimeModel()->TrueTimeUncertainty();

    auto earliest = (now > u) ? now - u : 0;
    auto latest = now + u;

    return {earliest, latest};
  }
};

}  // namespace whirl::matrix
