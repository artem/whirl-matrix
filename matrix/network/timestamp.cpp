#include <matrix/network/timestamp.hpp>

#include <matrix/world/global/global.hpp>

namespace whirl::matrix::net {

Timestamp GetNewEndpointTimestamp() {
  return WorldStepNumber();
}

}  // namespace whirl::matrix::net