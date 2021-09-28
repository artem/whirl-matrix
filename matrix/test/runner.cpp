#include <matrix/test/runner.hpp>

#include <matrix/new/debug.hpp>

#include <wheels/support/assert.hpp>

#include <wheels/support/progress.hpp>

#include <fmt/core.h>

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

  Verbose() << "Test determinism with seed " << kSeed << ":" << std::endl;

  // ActivateAllocsTracker();

  size_t digest1 = RunSimulation(kSeed);

  auto allocs_checksum_1 = GlobalAllocsCheckSum();

  size_t digest2 = RunSimulation(kSeed);

  auto allocs_checksum_2 = GlobalAllocsCheckSum();

  if (digest1 != digest2) {
    Report() << "Simulation is not deterministic: digest = "
      << digest1 << ", digest2 = {}" << digest2 << std::endl;
    Fail();
  }

  // Simulator internal check
  if (allocs_checksum_1 != allocs_checksum_2) {
    Report() << "Global allocator checksums mismatch" << std::endl;
    Fail();
  }

  Report() << "Determinism test is OK" << std::endl;
}

void TestRunner::RunSimulations(size_t count) {
  std::mt19937 seeds{42};

  Report() << "Run " << count << " simulations..." << std::endl;

  wheels::ProgressBar progress_bar("Progress", {false, '#', 50, false});

  if (!verbose_) {
    progress_bar.Start(count);
  }

  for (size_t i = 1; i <= count; ++i) {
    if (verbose_) {
      Verbose() << "Simulation " << i << "...";
    }

    RunSimulation(/*seed=*/seeds());

    if (!verbose_) {
      progress_bar.MakeProgress();
    }
  }

  if (!verbose_) {
    progress_bar.Complete();
  }
}

void TestRunner::RunSingleSimulation(size_t seed) {
  Report() << "Run single simulation with seed = " << seed << std::endl;

  size_t digest = RunSimulation(seed);

  Report() << "Simulation digest = " << digest << std::endl;
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

void TestRunner::ResetLogFile() {
  auto path = *log_path_;

  if (!path.is_absolute()) {
    Panic(fmt::format("Absolute log path expected: {}", path));
  }

  if (!path.has_parent_path()) {
    Panic(fmt::format("Invalid log file path: {}", path));
  }

  auto parent_dir = path.parent_path();

  if (!fs::exists(parent_dir)) {
    Panic(fmt::format("Log directory does not exist: {}", path.parent_path()));
  }

  if (!fs::is_directory(parent_dir)) {
    Panic(fmt::format("Parent path does not refer to a directory: {}", parent_dir));
  }

  if (fs::exists(path) && !fs::is_regular_file(path)) {
    Panic(fmt::format("Regular file expected: {}", path));
  }

  if (fs::exists(path)) {
    fs::resize_file(path, 0);
  }

  WriteLogHeader();

  Report() << "Log file: " << path << std::endl;
}

void TestRunner::WriteLogHeader() {
  std::ofstream log(*log_path_);
  log << "Whirl simulator log" << std::endl;
  log.close();
}

void TestRunner::Panic(const std::string& reason) {
  std::cerr << "Whirl test runner FAILED: " << reason << std::endl;
  std::exit(1);
}

}  // namespace whirl::matrix
