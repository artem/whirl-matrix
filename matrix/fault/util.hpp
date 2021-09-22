#pragma once

#include <matrix/fault/server.hpp>

#include <whirl/node/time/jiffies.hpp>

#include <string>
#include <vector>

namespace whirl::matrix::fault {

void RandomPause(Jiffies lo, Jiffies hi);

IFaultyServer& RandomServer(const std::vector<std::string>& hosts);

}  // namespace whirl::matrix::fault
