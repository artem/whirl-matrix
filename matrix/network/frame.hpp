#pragma once

#include <matrix/network/packet.hpp>

#include <matrix/time/time_point.hpp>

namespace whirl::matrix::net {

// ~ Ethernet frames

struct Frame {
  struct Header {
    std::string source_host;
    std::string dest_host;
    TimePoint send_time;
  };

  Header header;
  Packet packet;
};

}  // namespace whirl::matrix::net
