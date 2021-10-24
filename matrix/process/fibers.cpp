#include <matrix/process/fibers.hpp>

namespace whirl::matrix::process {

await::fibers::FiberId FiberManager::GenerateId() {
  return ++next_id_;
}

wheels::MutableMemView FiberManager::AcquireStack() {
  FiberStack* stack;

  if (!pool_.empty()) {
    // Reuse from pool
    stack = pool_.top();
    pool_.pop();
    stack->ResetForReuse();
  } else {
    // Allocate new
    stack = new FiberStack{};
  }

  return {(char*)stack, sizeof(FiberStack)};
}

void FiberManager::ReleaseStack(wheels::MutableMemView view) {
  FiberStack* stack = (FiberStack*)view.Begin();
  pool_.push(stack);
}

}  // namespace whirl::matrix::process
