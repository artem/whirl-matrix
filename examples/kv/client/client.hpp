#pragma once

#include <commute/rpc/channel.hpp>
#include <commute/rpc/call.hpp>

#include <await/fibers/core/api.hpp>
#include <await/fibers/sync/future.hpp>

#include <whirl/node/runtime/shortcuts.hpp>

#include <fmt/core.h>

//////////////////////////////////////////////////////////////////////

using Key = std::string;
using Value = uint32_t;

//////////////////////////////////////////////////////////////////////

class KVBlockingClient {
 public:
  explicit KVBlockingClient(commute::rpc::IChannelPtr channel)
    : channel_(channel) {
  }

  void Set(Key key, Value value) {
    await::fibers::Await(
        commute::rpc::Call("KV.Set")  //
          .Args(key, value)
          .Via(channel_)
          .TraceWith(GenerateTraceId("Set"))
          .Start()
          .As<void>())
          .ThrowIfError();
  }

  Value Get(Key key) {
    return await::fibers::Await(
        commute::rpc::Call("KV.Get")  //
          .Args(key)
          .Via(channel_)
          .TraceWith(GenerateTraceId("Get"))
          .Start()
          .As<Value>())
          .ValueOrThrow();
  }

 private:
  std::string GenerateTraceId(std::string op) const {
    return fmt::format("{}-{}", op, whirl::node::rt::GenerateGuid());
  }

 private:
  commute::rpc::IChannelPtr channel_;
};
