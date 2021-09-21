#pragma once

#include <matrix/log/event.hpp>

#include <iostream>

namespace whirl::matrix {

void WriteTextLog(const log::EventLog& log, std::ostream& out);

}  // namespace whirl::matrix
