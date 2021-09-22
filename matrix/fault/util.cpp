#include <matrix/fault/util.hpp>

#include <matrix/fault/access.hpp>

#include <whirl/node/runtime/shortcuts.hpp>

namespace whirl::matrix::fault {

void RandomPause(Jiffies lo, Jiffies hi) {
  auto delay = node::rt::RandomNumber(lo.Count(), hi.Count());
  node::rt::SleepFor({delay});
}

IFaultyServer& RandomServer(const std::vector<std::string>& hosts) {
  auto hostname = hosts.at(node::rt::RandomNumber(hosts.size()));
  return matrix::fault::Server(hostname);
}

}  // namespace whirl::matrix::fault