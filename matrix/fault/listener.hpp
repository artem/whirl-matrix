#pragma once

#include <matrix/network/frame.hpp>

namespace whirl::matrix::fault {

struct INetworkListener {
  virtual ~INetworkListener() = default;

  virtual size_t FrameCount() const = 0;
  virtual const net::Frame& GetFrame(size_t index) const = 0;
};

}  // namespace whirl::matrix::fault