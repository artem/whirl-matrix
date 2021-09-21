#include <matrix/log/backend.hpp>

#include <matrix/log/format.hpp>

#include <matrix/new/new.hpp>

#include <iostream>

namespace whirl::matrix::log {

LogBackend::LogBackend() {
  InitLevels();
}

void LogBackend::Write(const LogEvent& event) {
  events_.push_back(event);

  if (file_.has_value()) {
    FormatLogEventTo(event, *file_);
    *file_ << std::endl;
  }
}

static timber::Level kDefaultMinLogLevel = timber::Level::Info;

void LogBackend::InitLevels() {
  levels_ = GetLogLevelsFromEnv();
}

timber::Level LogBackend::GetMinLevelFor(const std::string& component) const {
  if (auto it = levels_.find(component); it != levels_.end()) {
    return it->second;
  }
  return kDefaultMinLogLevel;
}

void LogBackend::Log(timber::Event event) {
  GlobalAllocatorGuard g;
  Write(MakeLogEvent(event));
}

void LogBackend::AppendToFile(const std::string& path) {
  file_.emplace(path, std::ofstream::out | std::ofstream::app);

  // Write simulation separator
  *file_ << std::string(80, '-') << std::endl;
}

}  // namespace whirl::matrix::log
