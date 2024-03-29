#pragma once

#include <whirl/node/cluster/discovery.hpp>

#include <matrix/world/global/global.hpp>

namespace whirl::matrix {

class DiscoveryService : public node::cluster::IDiscoveryService {
 public:
  node::cluster::List ListPool(const std::string& name) override {
    return ::whirl::matrix::GetPool(name);
  }
};

}  // namespace whirl::matrix
