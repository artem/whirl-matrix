#include <matrix/test/runner.hpp>

#include <matrix/new/new.hpp>

#include <wheels/support/assert.hpp>

#include <random>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

namespace whirl::matrix {

static thread_local TestRunner* active{nullptr};

TestRunner& TestRunner::Access() {
  WHEELS_ASSERT(active != nullptr, "Not in simulation context");
  return *active;
}

void TestRunner::TestDeterminism() {
  static const size_t kSeed = 104107713;

  Debug() << "Test determinism with seed " << kSeed << ":" << std::endl;

  // ActivateAllocsTracker();

  size_t digest1 = RunSimulation(kSeed);

  auto allocs_checksum_1 = GlobalAllocsCheckSum();

  size_t digest2 = RunSimulation(kSeed);

  auto allocs_checksum_2 = GlobalAllocsCheckSum();

  if (digest1 != digest2) {
    Out() << "Simulation is not deterministic: digest = "
      << digest1 << ", digest2 = {}" << digest2 << std::endl;
    Fail();
  }

  // Simulator internal check
  if (allocs_checksum_1 != allocs_checksum_2) {
    Out() << "Global allocator checksums mismatch" << std::endl;
    Fail();
  }

  Out() << "Determinism test is OK" << std::endl;
}

void TestRunner::RunSimulations(size_t count) {
  std::mt19937 seeds{42};

  Out() << "Run simulations..." << std::endl;

  for (size_t i = 1; i <= count; ++i) {
    Debug() << "Simulation " << i << "...";
    RunSimulation(/*seed=*/seeds());
  }
}

void TestRunner::RunSingleSimulation(size_t seed) {
  Out() << "Run single simulation with seed = " << seed << std::endl;

  size_t digest = RunSimulation(seed);

  Out() << "Simulation digest = " << digest << std::endl;
}

size_t TestRunner::RunSimulation(size_t seed) {
  active = this;
  size_t digest = sim_(seed);
  active = nullptr;

  // Workaround for allocations check
  Cleanup();

  return digest;
}


void TestRunner::Congratulate() {
  std::cout << std::endl << "Looks good! ヽ(‘ー`)ノ" << std::endl;
}

void TestRunner::Fail() {
  std::cout << "(ﾉಥ益ಥ）ﾉ ┻━┻" << std::endl;
  std::cout.flush();
  std::exit(1);
}

void TestRunner::ResetLogFile(fs::path path) {
  if (!fs::exists(path.parent_path())) {
    WHEELS_PANIC(
        "Log directory does not exist: " << path.parent_path());
  }

  if (fs::exists(path)) {
    fs::resize_file(path, 0);
  }

  // Write header
  std::ofstream log(path);
  log << "Whirl simulator log" << std::endl;
  log.close();
}

}  // namespace whirl::matrix
