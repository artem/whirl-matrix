#pragma once

#include <matrix/network/packet.hpp>
#include <matrix/network/zone.hpp>

#include <string>

namespace whirl::matrix::net {

class Link;

struct IServer {
  virtual ~IServer() = default;

  virtual const std::string& HostName() const = 0;
  virtual ZoneId Zone() const = 0;

  virtual void HandlePacket(const Packet& packet, Link* out) = 0;
};

}  // namespace whirl::matrix::net
