#include <whirl/engines/matrix/client/util.hpp>

#include <whirl/runtime/methods.hpp>

namespace whirl::matrix::client {

bool Either() {
  return node::rt::RandomNumber(2) == 1;
}

}  // namespace whirl::matrix::client