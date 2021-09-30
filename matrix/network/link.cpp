#include <matrix/network/link.hpp>

#include <matrix/network/network.hpp>

#include <matrix/world/global/time.hpp>
#include <matrix/world/global/time_model.hpp>
#include <matrix/world/global/log.hpp>

#include <matrix/log/bytes.hpp>

#include <timber/log.hpp>

#include <wheels/support/assert.hpp>

namespace whirl::matrix::net {

Link::Link(Network* net, IServer* start, IServer* end)
    : net_(net), start_(start), end_(end), logger_("Network", GetLogBackend()) {
}

void Link::Add(Packet packet) {
  if (packet.header.type == Packet::Type::Data) {
    Address to{End()->HostName(), packet.header.dest_port};
    LOG_INFO("Send packet to {}: {}", to, log::FormatMessage(packet.message));
  }
  Add(MakeFrame(packet), ChooseDeliveryTime(packet));
}

Frame Link::MakeFrame(Packet packet) {
  return {{start_->HostName(), end_->HostName(), GlobalNow()},
          std::move(packet)};
}

TimePoint Link::ChooseDeliveryTime(const Packet& packet) const {
  if (IsLoopBack()) {
    return GlobalNow() + 1;
  }
  const auto flight_time = TimeModel()->FlightTime(Start(), End(), packet);
  return GlobalNow() + flight_time.Count();
}

Frame Link::ExtractNextFrame() {
  WHEELS_VERIFY(!paused_, "Link is paused");
  return frames_.Extract().frame;
}

void Link::Pause() {
  WHEELS_VERIFY(!paused_, "Link is already paused");
  paused_ = true;
}

void Link::Resume() {
  if (!paused_) {
    return;
  }

  paused_ = false;

  auto now = GlobalNow();

  if (!frames_.IsEmpty()) {
    while (frames_.Smallest().time < now) {
      Add(frames_.Extract().frame, now + 1);
    }
  }
}

void Link::Add(Frame frame, TimePoint delivery_time) {
  frames_.Insert({frame, delivery_time});
  net_->AddLinkEvent(this, delivery_time);
}

}  // namespace whirl::matrix::net
