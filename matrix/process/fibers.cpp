#include <matrix/process/fibers.hpp>

namespace whirl::matrix::process {

await::fibers::FiberId FiberManager::GenerateId() {
  return ++next_id_;
}

wheels::MutableMemView FiberManager::AcquireStack() {
  FiberStack* stack;

#if __has_feature(address_sanitizer)
  stack = new FiberStack{};
#else
  if (!pool_.empty()) {
    // Reuse from pool
    stack = pool_.top();
    pool_.pop();
    stack->ResetForReuse();
  } else {
    // Allocate new
    stack = new FiberStack{};
  }
#endif

  return {(char*)stack, sizeof(FiberStack)};
}

void FiberManager::ReleaseStack(wheels::MutableMemView view) {
  FiberStack* stack = reinterpret_cast<FiberStack*>(view.Begin());
#if __has_feature(address_sanitizer)
  delete stack;
#else
  pool_.push(stack);
#endif
}

}  // namespace whirl::matrix::process
