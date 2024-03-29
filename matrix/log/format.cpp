#include <matrix/log/format.hpp>

#include <timber/level/string.hpp>

#include <iomanip>

namespace whirl::matrix::log {

static std::string_view LimitWidth(std::string_view str, size_t width) {
  if (str.length() < width) {
    return str;
  } else {
    return str.substr(0, width);
  }
}

#define _FMT(str, width) std::setw(width) << LimitWidth(str, width)

void FormatLogEventTo(const Event& event, std::ostream& out) {
  // clang-format off

  out << std::left
      << "[T " << event.time << " | " << event.step << ']'
      << '\t'
      << '[' << _FMT(timber::LevelToString(event.level), 7) << ']'
      << '\t'
      << '[' << _FMT(event.actor, 15) << ']'
      << '\t'
      << '[' << _FMT(event.component, 12) << ']';

  if (event.trace_id.has_value()) {
    out << "\t"
        << "[" << event.trace_id.value() << "]";
  }

  out << "\t" << event.message;

  // clang-format on
}

}  // namespace whirl::matrix::log
