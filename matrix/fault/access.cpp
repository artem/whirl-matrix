#include <matrix/fault/access.hpp>

#include <matrix/world/impl.hpp>

namespace whirl::matrix::fault {

IFaultyServer& Server(const std::string& hostname) {
  return WorldImpl::Access()->GetServer(hostname);
}

IFaultyNetwork& Network() {
  return WorldImpl::Access()->GetNetwork();
}

}  // namespace whirl::matrix::fault
