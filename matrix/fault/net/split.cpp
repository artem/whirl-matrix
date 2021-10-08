#include <matrix/fault/net/split.hpp>

#include <whirl/node/runtime/shortcuts.hpp>

#include <matrix/fault/access.hpp>

#include <set>

namespace whirl::matrix::fault {

static std::set<std::string> GenerateSplit(std::vector<std::string> pool, size_t lhs_size) {
  std::set<std::string> lhs;

  for (size_t i = 0; i < lhs_size; ++i) {
    size_t j = node::rt::RandomNumber(i, pool.size() - 1);
    std::swap(pool[i], pool[j]);
    lhs.insert(pool[i]);
  }

  return lhs;
}

void RandomSplit(std::vector<std::string> pool, size_t lhs_size) {
  auto lhs = GenerateSplit(pool, lhs_size);

  auto& net = Network();

  for (size_t i = 0; i < pool.size(); ++i) {
    for (size_t j = 0; j < pool.size(); ++j) {
      // Cross split
      if (lhs.count(pool[i]) != lhs.count(pool[j])) {
        net.PauseLink(pool[i], pool[j]);
      }
    }
  }
}

}  // namespace whirl::matrix::fault
