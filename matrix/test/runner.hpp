#pragma once

#include <matrix/log/event.hpp>
#include <matrix/test/simulation.hpp>

#include <matrix/test/event_log.hpp>

#include <fmt/core.h>

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

  void WriteLogTo(std::string path) {
    log_file_.emplace(path);
  }

  bool IsVerbose() const {
    return verbose_;
  }

  // Run

  void TestDeterminism();
  void RunSimulations(size_t count);
  void RunSingleSimulation(size_t seed);

  // Access current test runner
  static TestRunner& Access();

  // Options

  std::optional<std::string> LogFile() const {
    return log_file_;
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

  void PrintLog(log::EventLog event_log) {
    // Print always
    std::cout << "Log:" << std::endl;
    matrix::WriteTextLog(event_log, std::cout);
    std::cout << std::endl;
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

 private:
  Simulation sim_;

  bool verbose_{false};
  std::optional<std::string> log_file_;

  std::stringstream sink_;
};

}  // namespace whirl::matrix