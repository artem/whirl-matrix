#include <matrix/guids/generator.hpp>

#include <fmt/core.h>

namespace whirl::matrix::guids {

std::string GuidGenerator::GenerateNext() {
  static const std::string_view kChars = "YXZ";

  uint64_t value = ++next_value_;
  return fmt::format("guid-{}-{}", kChars[value % kChars.length()], value);
}

}  // namespace whirl::matrix::guids
