#pragma once

#include <matrix/semantics/history.hpp>

#include <map>
#include <vector>

namespace whirl::matrix {

class HistoryRecorder {
  struct RunningCall {
    std::string method;
    semantics::Arguments arguments;
    semantics::TimePoint start_time;
    semantics::CallLabels labels;
  };

 public:
  using Cookie = size_t;

  // Context: Global
  size_t NumCompletedCalls() const;

  // Context: Server
  Cookie CallStarted(const std::string& method, const std::string& input);

  // Context: Server
  void AddLabel(Cookie id, const std::string& label);

  // Context: Server
  void CallCompleted(Cookie id, const std::string& output);

  // Context: Server
  void CallLost(Cookie id);

  // Context: Server
  void RemoveCall(Cookie id);

  // Context: World
  void Finalize();

  // After Finalize
  const semantics::History& GetHistory() const {
    return finalized_calls_;
  }

 private:
  // Finalizers
  static semantics::Call Complete(const RunningCall& call,
                                  semantics::Value output);
  static semantics::Call Lost(const RunningCall& call);

 private:
  std::vector<semantics::Call> finalized_calls_;
  Cookie next_id_{0};
  std::map<Cookie, RunningCall> running_calls_;
};

}  // namespace whirl::matrix
