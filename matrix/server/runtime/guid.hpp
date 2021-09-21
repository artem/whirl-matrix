#pragma once

#include <whirl/node/guids/service.hpp>

#include <matrix/world/global/guids.hpp>

namespace whirl::matrix {

struct GuidGenerator : public node::guids::IGuidGenerator {
  GuidGenerator() = default;

  std::string Generate() override {
    return GenerateGuid();
  }
};

}  // namespace whirl::matrix
