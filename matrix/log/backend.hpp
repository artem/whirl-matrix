#pragma once

#include <matrix/log/event.hpp>
#include <matrix/log/env.hpp>
#include <matrix/log/file.hpp>

#include <timber/backend.hpp>

#include <iostream>
#include <sstream>
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

  const EventLog& GetEvents() const {
    return events_;
  }

 private:
  void Write(const LogEvent& event);

  void InitLevels();

 private:
  LogLevels levels_;

  EventLog events_;
  std::ofstream file_;
};

}  // namespace whirl::matrix::log
