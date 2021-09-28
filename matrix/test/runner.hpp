#pragma once

#include <matrix/log/event.hpp>
#include <matrix/test/simulation.hpp>

#include <matrix/test/event_log.hpp>

#include <fmt/core.h>

#include <filesystem>

#include <iostream>
#include <sstream>

namespace whirl::matrix {

class TestRunner {
 public:
  explicit TestRunner(Simulation sim) : sim_(sim) {
  }

  // Configure

  void BeVerbose() {
    verbose_ = true;
  }

  void BeQuiet() {
    verbose_ = false;
  }

  void WriteLogTo(const std::string& path) {
    log_path_.emplace(path);
    ResetLogFile();
  }

  // Run

  void TestDeterminism();
  void RunSimulations(size_t count, uint32_t seq_seed = 42);
  void RunSingleSimulation(size_t seed);

  // Access current test runner
  static TestRunner& Access();

  // Options

  std::optional<std::string> LogFile() const {
    return log_path_;
  };

  // Output streams

  std::ostream& Verbose() {
    if (verbose_) {
      return std::cout;
    } else {
      return sink_;
    }
  }

  std::ostream& Report() {
    return std::cout;
  }

  // Fail

  void Fail();

  void Fail(std::string reason) {
    std::cout << reason << std::endl;
    Fail();
  }

  void Congratulate();

 private:
  size_t RunSimulation(size_t seed);

 private:
  void Cleanup() {
    // Release sink_ memory
    std::stringstream tmp;
    sink_.swap(tmp);
  }

  void ResetLogFile();
  void CheckLogPath(std::filesystem::path path);
  void WriteLogHeader();

  void Panic(const std::string& reason);

 private:
  Simulation sim_;

  bool verbose_{true};
  std::optional<std::filesystem::path> log_path_;

  std::stringstream sink_;
};

}  // namespace whirl::matrix
