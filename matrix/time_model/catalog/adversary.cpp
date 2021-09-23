#include <matrix/time_model/catalog/adversary.hpp>

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

  Jiffies InitWallClockOffset() override {
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
