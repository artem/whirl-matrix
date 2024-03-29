#pragma once

#include <wheels/memory/view.hpp>

#include <vector>

namespace whirl::matrix::fs {

class File {
 public:
  size_t Size() const;
  void Truncate(size_t new_size);
  void Append(wheels::ConstMemView data);
  size_t PRead(size_t offset, wheels::MutableMemView buffer) const;

  size_t ComputeDigest() const;

 private:
  wheels::ConstMemView Tail(size_t offset) const;

 private:
  std::vector<char> data_;
};

}  // namespace whirl::matrix::fs
