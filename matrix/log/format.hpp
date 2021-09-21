#pragma once

#include <matrix/log/event.hpp>

#include <iostream>

namespace whirl::matrix::log {

void FormatLogEventTo(const Event& event, std::ostream& out);

}  // namespace whirl::matrix::log
