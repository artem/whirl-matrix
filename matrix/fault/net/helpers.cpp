#include <matrix/fault/net/helpers.hpp>

#include <whirl/node/runtime/shortcuts.hpp>

namespace whirl::matrix::fault::net {

void Shuffle(std::vector<std::string>& pool) {
  for (size_t i = 0; i + 1 < pool.size()) {
    int j = node::rt::RandomNumber(i, pool.size() - 1);
    std::swap(pool[i], pool[j]);
  }
}

}  // namespace matrix::fault::net
