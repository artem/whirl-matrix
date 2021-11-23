#pragma once

#include <cstdlib>
#include <string>
#include <vector>

namespace whirl::matrix::fault {

// Pause all incoming and outgoing links
void Isolate(std::vector<std::string> pool, std::string victim);

void PauseIncomingLinks(std::vector<std::string> pool, std::string victim);

}  // namespace whirl::matrix::fault
