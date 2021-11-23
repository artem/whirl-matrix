#include <matrix/fault/net/isolate.hpp>

#include <matrix/fault/access.hpp>

namespace whirl::matrix::fault {

void Isolate(std::vector<std::string> pool, std::string victim) {
  auto& net = Network();

  for (const auto& host : pool) {
    if (host != victim) {
      net.PauseLink(host, victim);
      net.PauseLink(victim, host);
    }
  }
}

void PauseIncomingLinks(std::vector<std::string> pool, std::string victim) {
  auto& net = Network();

  for (const auto& host : pool) {
    if (host != victim) {
      net.PauseLink(host, victim);
      net.PauseLink(victim, host);
    }
  }
}

}  // namespace whirl::matrix::fault
