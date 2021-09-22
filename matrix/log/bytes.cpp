#include <matrix/log/bytes.hpp>

#include <muesli/archives.hpp>

#include <fmt/core.h>

namespace whirl::matrix::log {

std::string FormatMessage(const std::string& bytes) {
  if (muesli::archives::IsBinaryFormat()) {
    return "<binary>";
  } else {
    // TODO: Line breaks?
    return fmt::format("<{}>", bytes);
  }
}

}  // namespace whirl::matrix::net