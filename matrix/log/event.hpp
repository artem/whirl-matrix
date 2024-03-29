#pragma once

#include <timber/event.hpp>

#include <matrix/time/time_point.hpp>

#include <string>
#include <optional>
#include <vector>

namespace whirl::matrix::log {

//////////////////////////////////////////////////////////////////////

struct Event {
  TimePoint time;
  size_t step;
  timber::Level level;
  std::string actor;
  std::string component;
  std::optional<std::string> trace_id;
  std::string message;
};

//////////////////////////////////////////////////////////////////////

Event CaptureMatrixContext(const timber::Event& event);

//////////////////////////////////////////////////////////////////////

using EventLog = std::vector<Event>;

}  // namespace whirl::matrix::log
