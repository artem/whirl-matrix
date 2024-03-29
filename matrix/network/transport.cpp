#include <matrix/network/transport.hpp>

#include <matrix/network/network.hpp>

#include <matrix/new/new.hpp>

// IsThereAdversary
#include <matrix/world/global/global.hpp>
#include <matrix/world/global/log.hpp>

#include <matrix/log/bytes.hpp>

#include <timber/log.hpp>

#include <wheels/support/assert.hpp>

namespace whirl::matrix::net {

namespace detail {

template <typename Callback>
class TransportTask : public process::ITask {
 public:
  TransportTask(Callback cb) : cb_(std::move(cb)) {
  }

  void Run() override {
    cb_();
    delete this;
  }

 private:
  Callback cb_;
};

}  // namespace detail

Transport::Transport(Network& net, const std::string& host,
                     process::Memory& heap, process::Scheduler& scheduler)
    : net_(net),
      host_(host),
      heap_(heap),
      scheduler_(scheduler),
      logger_("Transport", GetLogBackend()) {
}

ClientSocket Transport::ConnectTo(const Address& address,
                                  ISocketHandler* handler) {
  GlobalAllocatorGuard g;

  Link* link = net_.GetLink(host_, address.host);

  Port port = FindFreePort();
  Timestamp ts = GetNewEndpointTimestamp();

  LOG_INFO("Connecting to {}: local port = {}", address, port);

  endpoints_.emplace(port, Endpoint{handler, ts});

  if (IsThereAdversary()) {
    // Init ping-pong for detecting crashes / reboots
    link->Add({{Packet::Type::Ping, port, address.port, ts}, "<ping>"});
  }

  {
    auto g = heap_.Use();
    return ClientSocket{this, link, port, address.port, ts};
  }
};

ServerSocket Transport::Serve(Port port, ISocketHandler* handler) {
  GlobalAllocatorGuard g;

  WHEELS_VERIFY(endpoints_.count(port) == 0, "Port already in use");

  auto ts = GetNewEndpointTimestamp();
  endpoints_.emplace(port, Endpoint{handler, ts});

  LOG_INFO("Serving at port {}", port);

  {
    auto g = heap_.Use();
    return ServerSocket{this, port};
  }
};

void Transport::RemoveEndpoint(Port port) {
  GlobalAllocatorGuard g;

  LOG_INFO("Remove endpoint at port {}", port);
  endpoints_.erase(port);
}

void Transport::Reset() {
  GlobalAllocatorGuard g;

  for ([[maybe_unused]] const auto& [port, _] : endpoints_) {
    LOG_INFO("Remove endpoint at port {}", port);
  }
  endpoints_.clear();
}

class Replier {
 public:
  Replier(const Packet& packet, Link* out) : packet_(packet), out_(out) {
  }

  void Ping() {
    Reply(Packet::Type::Ping, "<ping>");
  }

  void Reset() {
    Reply(Packet::Type::Reset, "<reset>");
  }

  void Data(const Message& message) {
    Reply(Packet::Type::Data, message);
  }

 private:
  void Send(const Packet& packet) {
    out_->Add(packet);
  }

  void Reply(Packet::Type type, Message payload) {
    Send({{type, packet_.header.dest_port, packet_.header.source_port,
           packet_.header.ts},
          std::move(payload)});
  }

 private:
  const Packet packet_;
  Link* out_;
};

void Transport::HandlePacket(const Packet& packet, Link* out) {
  GlobalAllocatorGuard g;

  Address from{out->End()->HostName(), packet.header.source_port};
  Address to{host_, packet.header.dest_port};

  /*
  if (packet.type != EPacketType::Ping) {
    WHIRL_FMT_LOG("Handle packet from {} with ts = {}", from, packet.ts);
  }
  */

  Replier replier(packet, out);

  auto endpoint_it = endpoints_.find(packet.header.dest_port);

  if (endpoint_it == endpoints_.end()) {
    // Endpoint not found

    if (packet.header.type != Packet::Type::Reset) {
      if (packet.header.type == Packet::Type::Data) {
        LOG_WARN("Endpoint {} not found, drop incoming packet from {}", to,
                 from);
      }
      replier.Reset();
    }
    return;
  }

  const auto& endpoint = endpoint_it->second;

  if (packet.header.ts < endpoint.ts) {
    // WHIRL_FMT_LOG("Outdated packet, send <reset> packet to {}", from);
    replier.Reset();
    return;

  } else if (packet.header.type == Packet::Type::Ping) {
    // Ping
    // WHIRL_FMT_LOG("Send ping back to {}", source);
    replier.Ping();
    return;

  } else if (packet.header.type == Packet::Type::Reset) {
    // Disconnect
    auto g = heap_.Use();

    auto callback = [handler = endpoint.handler, host = from.host]() {
      handler->HandleDisconnect(host);
    };

    auto task = new detail::TransportTask(std::move(callback));
    scheduler_.ScheduleAsap(task);

    return;

  } else if (packet.header.type == Packet::Type::Data) {
    // Message

    LOG_INFO("Handle message from {}: {}", from, log::FormatMessage(packet.message));

    auto g = heap_.Use();

    auto callback = [handler = endpoint.handler, message = packet.message,
                     reply_socket = ReplySocket(packet.header, out)]() {
      handler->HandleMessage(message, reply_socket);
    };

    auto* task = new detail::TransportTask(std::move(callback));

    scheduler_.ScheduleAsap(task);

    //    endpoint.handler->HandleMessage(packet.message,
    //                                    ReplySocket(packet.header, out));
    return;
  }
}

Port Transport::FindFreePort() {
  while (true) {
    if (endpoints_.count(next_port_) == 0) {
      return next_port_++;
    }
    next_port_++;
  }
}

}  // namespace whirl::matrix::net
