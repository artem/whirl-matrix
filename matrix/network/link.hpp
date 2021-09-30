#pragma once

#include <matrix/time/time_point.hpp>

#include <matrix/network/packet.hpp>
#include <matrix/network/server.hpp>
#include <matrix/network/frame.hpp>
#include <matrix/trace/tracer.hpp>

#include <matrix/helpers/priority_queue.hpp>

#include <timber/logger.hpp>

namespace whirl::matrix::net {

class Network;

// One-way link
class Link {
 private:
  struct FrameEvent {
    Frame frame;
    TimePoint time;

    bool operator<(const FrameEvent& that) const {
      return time < that.time;
    }
  };

  using FrameQueue = PriorityQueue<FrameEvent>;

 public:
  Link(Network* net, IServer* start, IServer* end);

  IServer* Start() const {
    return start_;
  }

  IServer* End() const {
    return end_;
  }

  void SetOpposite(Link* link) {
    opposite_ = link;
  }

  Link* GetOpposite() const {
    return opposite_;
  }

  bool IsLoopBack() const {
    return start_ == end_;
  }

  void Add(Packet packet);

  bool IsPaused() const {
    return paused_;
  }

  bool HasFrames() const {
    return !frames_.IsEmpty();
  }

  TimePoint NextFrameTime() const {
    return frames_.Smallest().time;
  }

  Frame ExtractNextFrame();

  void Shutdown() {
    frames_.Clear();
  }

  // Faults

  void Pause();
  void Resume();

 private:
  Frame MakeFrame(Packet packet);

  TimePoint ChooseDeliveryTime(const Packet& packet) const;

  void Add(Frame frame, TimePoint delivery_time);

 private:
  Network* net_;
  IServer* start_;
  IServer* end_;

  FrameQueue frames_;
  bool paused_{false};

  Link* opposite_{nullptr};

  timber::Logger logger_;
};

}  // namespace whirl::matrix::net
