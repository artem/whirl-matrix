#pragma once

#include <matrix/network/frame.hpp>

namespace whirl::matrix {

struct ITracer {
  virtual ~ITracer() = default;

  virtual void Deliver(const net::Frame& frame) = 0;

  // After last `Deliver`
  virtual void Finalize() = 0;
};

}  // namespace whirl::matrix
