#pragma once

#include <matrix/log/event.hpp>
#include <matrix/log/env.hpp>

#include <timber/backend.hpp>

#include <optional>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

namespace whirl::matrix::log {

class LogBackend : public timber::ILogBackend {
 public:
  LogBackend();

  // ILogBackend

  // Context: Server
  timber::Level GetMinLevelFor(const std::string& component) const override;

  // Context: Server
  void Log(timber::Event event) override;

  // Matrix

  void AppendToFile(const std::string& path);

  const EventLog& GetEvents() const {
    return events_;
  }

 private:
  void Write(const Event& event);

  void InitLevels();

 private:
  LogLevels levels_;

  EventLog events_;
  std::optional<std::ofstream> file_;
};

}  // namespace whirl::matrix::log
