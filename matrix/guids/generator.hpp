#pragma once

#include <fmt/core.h>

#include <cstdint>
#include <string>

namespace whirl::matrix::guids {

// Generates short, unique and non-monotonic (!) strings

class GuidGenerator {
 public:
  std::string GenerateNext() {
    static const std::string_view kChars = "YXZ";

    uint64_t value = ++next_value_;
    return fmt::format("guid-{}-{}", kChars[value % kChars.length()], value);
  }

 private:
  uint64_t next_value_{0};
};

}  // namespace whirl::matrix::guids
