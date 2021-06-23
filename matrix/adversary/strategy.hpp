#pragma once

#include <whirl/node/threads.hpp>

#include <functional>

namespace whirl::adversary {

using Strategy = std::function<void(ThreadsRuntime&)>;

}  // namespace whirl::adversary
