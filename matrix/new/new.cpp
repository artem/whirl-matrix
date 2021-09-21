#include <matrix/new/new.hpp>

#include <matrix/new/tracker.hpp>
#include <matrix/new/debug.hpp>

#include <wheels/support/panic.hpp>

#include <cstdlib>
#include <cstdio>
#include <new>

//////////////////////////////////////////////////////////////////////

static thread_local whirl::matrix::IMemoryAllocator* allocator{nullptr};

namespace whirl::matrix {

void SetAllocator(IMemoryAllocator* a) {
  allocator = a;
}

IMemoryAllocator* GetCurrentAllocator() {
  return allocator;
}

}  // namespace whirl::matrix

//////////////////////////////////////////////////////////////////////

static uintptr_t global_allocs_checksum = 0;
static GlobalAllocTracker global_allocs_tracker;

static void* AllocateGlobal(size_t size) {
  if (void* addr = std::malloc(size)) {
    global_allocs_checksum ^= (uintptr_t)addr;
    global_allocs_tracker.Allocate(addr, size);
    return addr;
  } else {
    WHEELS_PANIC("Failed to malloc " << size << " bytes");
  }
}

static void FreeGlobal(void* addr) {
  std::free(addr);
  global_allocs_checksum ^= (uintptr_t)addr;
  global_allocs_tracker.Deallocate(addr);
}

//////////////////////////////////////////////////////////////////////

uintptr_t GlobalAllocsCheckSum() {
  return global_allocs_checksum;
}

void ActivateAllocsTracker() {
  global_allocs_tracker.Activate();
}

void PrintAllocsTrackerReport() {
  global_allocs_tracker.PrintReport();
}

//////////////////////////////////////////////////////////////////////

void* operator new(size_t size) {
  if (allocator != nullptr) {
    return allocator->Allocate(size);
  }
  return AllocateGlobal(size);
}

void operator delete(void* addr) noexcept {
  if (allocator != nullptr) {
    allocator->Free(addr);
    return;
  }
  return FreeGlobal(addr);
}
