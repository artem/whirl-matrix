#pragma once

#include <whirl/node/random/service.hpp>

// Impl
#include <matrix/world/global/random.hpp>

#include <wheels/support/assert.hpp>

namespace whirl::matrix {

struct RandomGenerator : node::random::IRandomService {
  uint64_t GenerateNumber(uint64_t bound) override {
    WHEELS_ASSERT(bound > 0, "bound == 0");
    return GlobalRandomNumber() % bound;
  }
};

}  // namespace whirl::matrix
