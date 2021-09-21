#include <matrix/fault/access.hpp>

#include <matrix/world/world.hpp>

namespace whirl::matrix::fault {

IFaultyServer& Server(const std::string& hostname) {
  return World::Access()->GetServer(hostname);
}

IFaultyNetwork& Network() {
  return World::Access()->GetNetwork();
}

}  // namespace whirl::matrix::fault
