#pragma once

#include <cstdint>
#include <string>

namespace whirl::matrix::guids {

// Generates short, unique and non-monotonic strings

class GuidGenerator {
 public:
  std::string GenerateNext();

 private:
  uint64_t next_value_{0};
};

}  // namespace whirl::matrix::guids
