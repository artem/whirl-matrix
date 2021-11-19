#pragma once

#include <random>

namespace whirl::matrix {

class RandomSource {
  // NB: Consistent across all platforms
  // https://eel.is/c++draft/rand.eng.mers
  using Impl = std::mt19937;

 public:
  using ResultType = Impl::result_type;

 public:
  RandomSource(ResultType seed) {
    Reset(seed);
  }

  void Reset(ResultType seed) {
    steps_ = 0;
    impl_.seed(seed);
  }

  ResultType Next() {
    ++steps_;
    return impl_();
  }

  size_t Steps() const {
    return steps_;
  }

 private:
  Impl impl_;
  size_t steps_ = 0;
};

}  // namespace whirl::matrix
