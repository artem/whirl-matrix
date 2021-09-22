#pragma once

#include <matrix/time_model/time_model.hpp>

namespace whirl::matrix {

//////////////////////////////////////////////////////////////////////

class AdversaryTimeModel : public IServerTimeModel {
 public:
  // Clocks

  int InitClockDrift() override {
    return 0;
  }

  TimePoint ResetMonotonicClock() override {
    return 0;
  }

  TimePoint InitWallClockOffset() override {
    return 0;
  }

  // TrueTime

  Jiffies TrueTimeUncertainty() override {
    return 1;
  }

  // Disk

  Jiffies DiskWrite(size_t /*bytes*/) override {
    return 1;
  }

  Jiffies DiskRead(size_t /*bytes*/) override {
    return 1;
  }

  // Threads

  Jiffies ThreadPause() override {
    return 1;
  }
};

//////////////////////////////////////////////////////////////////////

IServerTimeModelPtr MakeAdversaryTimeModel() {
  return std::make_unique<AdversaryTimeModel>();
}

}  // namespace whirl::matrix
