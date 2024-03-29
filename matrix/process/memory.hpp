#pragma once

#include <matrix/memory/allocator.hpp>
#include <matrix/new/new.hpp>

namespace whirl::matrix::process {

class Memory {
 public:
  AllocatorGuard Use() {
    return AllocatorGuard(&impl_);
  }

  size_t BytesAllocated() const {
    return impl_.BytesAllocated();
  }

  bool FromHere(void* addr) const {
    return impl_.FromHere(addr);
  }

  void Reset() {
    impl_.Reset();
  }

 private:
  MemoryAllocator impl_;
};

}  // namespace whirl::matrix::process
