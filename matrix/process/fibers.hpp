#pragma once

#include <await/fibers/core/manager.hpp>

#include <cstring>
#include <stack>

namespace whirl::matrix::process {

//////////////////////////////////////////////////////////////////////

static const size_t kStackSize =  64 * 4 * 1024;

struct FiberStack {
  char buf[kStackSize];

  void ResetForReuse() {
    memset(buf, 0, kStackSize);
  }
};

//////////////////////////////////////////////////////////////////////

// Fiber resource manager

class FiberManager : public await::fibers::IFiberManager {
 public:
  // Ids
  await::fibers::FiberId GenerateId() override;

  // Stacks
  wheels::MutableMemView AcquireStack() override;
  void ReleaseStack(wheels::MutableMemView view) override;

 private:
  size_t next_id_{0};
  std::stack<FiberStack*> pool_;
};

}  // namespace whirl::matrix::process
