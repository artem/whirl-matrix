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

  void WriteLogTo(const std::string& path) {
    log_path_.emplace(path);
    ResetLogFile(path);
  }

  // Run

  void TestDeterminism();
  void RunSimulations(size_t count);
  void RunSingleSimulation(size_t seed);

  // Access current test runner
  static TestRunner& Access();

  // Options

  std::optional<std::string> LogFile() const {
    return log_path_;
  };

  // Output streams

  std::ostream& Debug() {
    if (verbose_) {
      return std::cout;
    } else {
      return sink_;
    }
  }

  std::ostream& Out() {
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

  static void ResetLogFile(std::filesystem::path path);

 private:
  Simulation sim_;

  bool verbose_{false};
  std::optional<std::filesystem::path> log_path_;

  std::stringstream sink_;
};

}  // namespace whirl::matrix