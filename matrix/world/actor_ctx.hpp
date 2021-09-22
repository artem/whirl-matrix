#pragma once

#include <matrix/world/actor.hpp>

namespace whirl::matrix {

//////////////////////////////////////////////////////////////////////

class ActorContext {
  using Ctx = ActorContext;

 public:
  class ScopeGuard {
   public:
    ScopeGuard(Ctx* ctx, IActor* actor) : ctx_(ctx), actor_(actor) {
      parent_ = ctx_->current_;
      ctx_->current_ = actor;
    }

    IActor* operator->() {
      return actor_;
    }

    ~ScopeGuard() {
      // Rollback
      ctx_->current_ = parent_;
    }

   private:
    Ctx* ctx_;
    IActor* actor_;
    IActor* parent_;
  };

  ScopeGuard Scope(IActor* actor) {
    return ScopeGuard(this, actor);
  }

  IActor* Get() const {
    return current_;
  }

 private:
  bool NotSet() const {
    return current_ == nullptr;
  }

 private:
  IActor* current_{nullptr};
};

}  // namespace whirl::matrix
