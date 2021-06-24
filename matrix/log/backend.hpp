#pragma once

#include <whirl/logger/backend.hpp>

#include <whirl/engines/matrix/log/event.hpp>
#include <whirl/engines/matrix/log/env.hpp>
#include <whirl/engines/matrix/log/file.hpp>

#include <iostream>
#include <sstream>

namespace whirl::matrix {

class LogBackend : public ILoggerBackend {
 public:
  LogBackend();

  // Context: Server
  LogLevel GetMinLevel(const std::string& component) const;

  // Context: Server
  void Log(const std::string& component, LogLevel level,
           const std::string& message);

  std::string TextLog() const {
    return memory_.str();
  }

 private:
  void Write(const LogEvent& event);

  void InitLevels();

 private:
  LogLevels levels_;

  std::stringstream memory_;
  std::ofstream file_;
};

}  // namespace whirl::matrix