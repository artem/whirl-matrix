#pragma once

#include <matrix/fault/server.hpp>
#include <matrix/fault/network.hpp>

namespace whirl::matrix::fault {

IFaultyServer& Server(const std::string& hostname);
IFaultyNetwork& Network();

}  // namespace whirl::matrix::fault
