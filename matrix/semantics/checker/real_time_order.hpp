#pragma once

#include <matrix/semantics/history.hpp>

namespace whirl::semantics {

// https://jepsen.io/consistency/models/linearizable

inline bool PrecedesInRealTime(const Call& lhs, const Call& rhs) {
  return lhs.IsCompleted() && lhs.end_time.value() < rhs.start_time;
}

}  // namespace whirl::semantics
