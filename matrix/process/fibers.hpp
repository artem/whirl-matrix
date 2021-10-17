#pragma once

#include <await/fibers/core/manager.hpp>

#include <cstring>
#include <stack>

namespace whirl::matrix::process {

//////////////////////////////////////////////////////////////////////

static const size_t kStackSize = 64 * 1024;

struct FiberStack {
  char buf[kStackSize];

  void Reset() {
    memset(buf, 0, kStackSize);
  }
};

//////////////////////////////////////////////////////////////////////

class FiberManager : public await::fibers::IFiberManager {
 public:
  await::fibers::FiberId GenerateId() override {
    return ++next_id_;
  }

  wheels::MutableMemView AcquireStack() override {
    FiberStack* stack;

    if (!pool_.empty()) {
      // Reuse from pool
      stack = pool_.top();
      pool_.pop();
      stack->Reset();
    } else {
      // Allocate new
      stack = new FiberStack{};
    }

    return {(char*)stack, sizeof(FiberStack)};
  }

  void ReleaseStack(wheels::MutableMemView view) override {
    FiberStack* stack = (FiberStack*)view.Begin();
    pool_.push(stack);
  }

 private:
  size_t next_id_{0};
  std::stack<FiberStack*> pool_;
};

}  // namespace whirl::matrix::process
