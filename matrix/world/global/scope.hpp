#pragma once

#include <matrix/world/actor_ctx.hpp>

namespace whirl::matrix {

ActorContext::ScopeGuard SwitchToActor(IActor* actor);

}  // namespace whirl::matrix
