#pragma once

#include <matrix/fault/server.hpp>
#include <matrix/fault/network.hpp>
#include <matrix/fault/listener.hpp>

namespace whirl::matrix::fault {

IFaultyServer& Server(const std::string& hostname);
IFaultyNetwork& Network();

INetworkListener& NetworkListener();

}  // namespace whirl::matrix::fault
