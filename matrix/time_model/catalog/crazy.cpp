#include <matrix/time_model/catalog/crazy.hpp>

#include <matrix/world/global/random.hpp>

namespace whirl::matrix {

//////////////////////////////////////////////////////////////////////

class CrazyServerTimeModel : public IServerTimeModel {
 public:
  // Clocks

  // [-75, +75]
  int InitClockDrift() override {
    if (GlobalRandomNumber() % 3 == 0) {
      // Super-fast monotonic clocks
      // x3-x4 faster than global time
      return 200 + GlobalRandomNumber(100);
    } else if (GlobalRandomNumber() % 2 == 0) {
      // Relatively fast
      return 75 + GlobalRandomNumber(25);
    } else {
      // Relatively slow
      return -75 + (int)GlobalRandomNumber(25 + 1);
    }
    // return -75 + (int)GlobalRandomNumber(75 * 2 + 1);
  }

  TimePoint ResetMonotonicClock() override {
    return GlobalRandomNumber(1, 100);
  }

  Jiffies InitWallClockOffset() override {
    return GlobalRandomNumber(1000);
  }

  // TrueTime

  Jiffies TrueTimeUncertainty() override {
    if (GlobalRandomNumber() % 7 == 0) {
      return GlobalRandomNumber(300, 1000);
    }
    return GlobalRandomNumber(5, 50);
  }

  // Disk

  Jiffies DiskWrite(size_t /*bytes*/) override {
    return GlobalRandomNumber(10, 250);
  }

  Jiffies DiskRead(size_t /*bytes*/) override {
    return GlobalRandomNumber(10, 50);
  }

  // Database

  bool GetCacheMiss() override {
    return GlobalRandomNumber(11) == 0;
  }

  bool IteratorCacheMiss() override {
    return GlobalRandomNumber(17) == 0;
  }

  // Threads

  Jiffies ThreadPause() override {
    return GlobalRandomNumber(5, 50);
  }
};

//////////////////////////////////////////////////////////////////////

class CrazyTimeModel : public ITimeModel {
 public:
  void Initialize() override {
  }

  TimePoint GlobalStartTime() override {
    return GlobalRandomNumber(1, 200);
  }

  // Server

  IServerTimeModelPtr MakeServerModel(const std::string& /*host*/) override {
    return std::make_unique<CrazyServerTimeModel>();
  }

  // Network

  Jiffies FlightTime(const net::IServer* /*start*/, const net::IServer* /*end*/,
                     const net::Packet& packet) override {
    if (packet.header.type != net::Packet::Type::Data) {
      // Service packet, do not affect randomness
      return 50;
    }

    // Slow
    if (GlobalRandomNumber() % 11 == 0) {
      return GlobalRandomNumber(400, 1000);
    }
    // Fast
    if (GlobalRandomNumber() % 7 == 0) {
      return GlobalRandomNumber(5, 10);
    }
    // Unpredictable
    if (GlobalRandomNumber() % 5 == 0) {
      return GlobalRandomNumber(10, 1000);
    }
    // Default
    return GlobalRandomNumber(30, 60);
  }

  commute::rpc::BackoffParams BackoffParams() override {
    return {50, 1000, 2};
  }

};

//////////////////////////////////////////////////////////////////////

ITimeModelPtr MakeCrazyTimeModel() {
  return std::make_unique<CrazyTimeModel>();
}

}  // namespace whirl::matrix
