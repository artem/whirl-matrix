#include "main.hpp"

// Node
#include <whirl/node/runtime/shortcuts.hpp>
#include <whirl/node/rpc/server.hpp>
#include <whirl/node/cluster/peer.hpp>
#include <whirl/node/store/kv.hpp>

// RPC
#include <commute/rpc/service_base.hpp>
#include <commute/rpc/call.hpp>

// Serialization
#include <muesli/serializable.hpp>
// Support std::string serialization
#include <cereal/types/string.hpp>

// Logging
#include <timber/log.hpp>

// Concurrency
#include <await/fibers/core/api.hpp>
#include <await/fibers/sync/future.hpp>
#include <await/fibers/sync/mutex.hpp>
#include <await/futures/combine/quorum.hpp>
#include <await/futures/util/never.hpp>

#include <algorithm>

using await::fibers::Await;
using await::futures::Future;
using wheels::Result;

using namespace whirl;

//////////////////////////////////////////////////////////////////////

// Key -> Value
using Key = std::string;
using Value = std::string;

//////////////////////////////////////////////////////////////////////

struct WriteTimestamp {
  uint64_t value;

  static WriteTimestamp Min() {
    return {0};
  }

  bool operator<(const WriteTimestamp& that) const {
      return value < that.value;
  };

  // Serialization support (RPC, Database)
  MUESLI_SERIALIZABLE(value)
};

// Logging
std::ostream& operator<<(std::ostream& out, const WriteTimestamp& ts) {
  out << ts.value;
  return out;
}

//////////////////////////////////////////////////////////////////////

// Replicas store versioned (stamped) values

struct StampedValue {
  Value value;
  WriteTimestamp timestamp;

  MUESLI_SERIALIZABLE(value, timestamp)
};

std::ostream& operator<<(std::ostream& out, const StampedValue& stamped_value) {
  out << "{" << stamped_value.value << ", ts: " << stamped_value.timestamp
      << "}";
  return out;
}

//////////////////////////////////////////////////////////////////////

// Coordinator role, stateless

class Coordinator : public commute::rpc::ServiceBase<Coordinator>,
                    public node::cluster::Peer {
 public:
  Coordinator()
      : Peer(node::rt::Config()),
        logger_("KVNode.Coordinator", node::rt::LoggerBackend()) {
  }

  void RegisterMethods() override {
    COMMUTE_RPC_REGISTER_METHOD(Set);
    COMMUTE_RPC_REGISTER_METHOD(Get);
  };

  // RPC handlers

  void Set(Key key, Value value) {
    WriteTimestamp write_ts = ChooseWriteTimestamp();
    LOG_INFO("Write timestamp: {}", write_ts);

    std::vector<Future<void>> writes;

    // Broadcast
    for (const auto& peer : ListPeers().WithMe()) {
      writes.push_back(  //
          commute::rpc::Call("Replica.LocalWrite")
              .Args<Key, StampedValue>(key, {value, write_ts})
              .Via(Channel(peer))
              .Context(await::context::ThisFiber())
              .AtLeastOnce());
    }

    // Await acknowledgements from the majority of storage replicas
    Await(Quorum(std::move(writes), /*threshold=*/Majority())).ThrowIfError();
  }

  Value Get(Key key) {
    std::vector<Future<StampedValue>> reads;

    // Broadcast LocalRead request to replicas
    for (const auto& peer : ListPeers().WithMe()) {
      reads.push_back(  //
          commute::rpc::Call("Replica.LocalRead")
              .Args(key)
              .Via(Channel(peer))
              .Context(await::context::ThisFiber())
              .AtLeastOnce());
    }

    // Await responses from the majority of replicas

    // 1) Combine futures from read RPC-s to single quorum future
    Future<std::vector<StampedValue>> quorum_reads =
        Quorum(std::move(reads), /*threshold=*/Majority());
    // 2) Block current fiber until quorum collected
    Result<std::vector<StampedValue>> results = Await(std::move(quorum_reads));
    // 3) Unpack vector or throw error
    std::vector<StampedValue> stamped_values = results.ValueOrThrow();

    // Or combine all steps into:
    // auto stamped_values = Await(Quorum(std::move(reads),
    // Majority())).ValueOrThrow();

    for (size_t i = 0; i < stamped_values.size(); ++i) {
      LOG_INFO("{}-th value in read quorum: {}", i + 1, stamped_values[i]);
    }

    auto most_recent = FindMostRecent(stamped_values);
    return most_recent.value;
  }

 private:
  WriteTimestamp ChooseWriteTimestamp() const {
    // Local wall clock may be out of sync with other replicas
    // Use TrueTime service (node::rt::TrueTime())
    return {node::rt::WallTimeNow().ToJiffies().Count()};
  }

  // Find value with the largest timestamp
  StampedValue FindMostRecent(const std::vector<StampedValue>& values) const {
    return *std::max_element(
        values.begin(), values.end(),
        [](const StampedValue& lhs, const StampedValue& rhs) {
          return lhs.timestamp < rhs.timestamp;
        });
  }

  // Quorum size
  size_t Majority() const {
    return NodeCount() / 2 + 1;
  }

 private:
  timber::Logger logger_;
};

//////////////////////////////////////////////////////////////////////

// Storage replica role

class Replica : public commute::rpc::ServiceBase<Replica> {
 public:
  Replica()
      : kv_store_(node::rt::Database(), "data"),
        logger_("KVNode.Replica", node::rt::LoggerBackend()) {
  }

  void RegisterMethods() override {
    COMMUTE_RPC_REGISTER_METHOD(LocalWrite);
    COMMUTE_RPC_REGISTER_METHOD(LocalRead);
  };

  // RPC handlers

  void LocalWrite(Key key, StampedValue target_value) {
    std::lock_guard guard(write_mutex_);

    std::optional<StampedValue> local_value = kv_store_.TryGet(key);

    if (!local_value.has_value()) {
      // First write for this key
      Update(key, target_value);
    } else {
      // Write timestamp > timestamp of locally stored value
      if (local_value->timestamp < target_value.timestamp) {
        Update(key, target_value);
      }
    }
  }

  StampedValue LocalRead(Key key) {
    return kv_store_.GetOr(key, {"", WriteTimestamp::Min()});
  }

 private:
  void Update(Key key, StampedValue target_value) {
    LOG_INFO("Write '{}' -> {}", key, target_value);
    kv_store_.Put(key, target_value);
  }

 private:
  // Local persistent K/V storage
  // strings -> StampedValues
  node::store::KVStore<StampedValue> kv_store_;
  // Mutex for _fibers_
  // Guards writes to kv_store_
  await::fibers::Mutex write_mutex_;

  timber::Logger logger_;
};

//////////////////////////////////////////////////////////////////////

// Main routine

void KVNodeMain() {
  node::rt::PrintLine("Starting at {}", node::rt::WallTimeNow());

  node::rt::Database()->Open(
      node::rt::Config()->GetString("db.path"));

  auto rpc_server =
      node::rpc::MakeServer(node::rt::Config()->GetInt<uint16_t>("rpc.port"));

  rpc_server->RegisterService("KV", std::make_shared<Coordinator>());
  rpc_server->RegisterService("Replica", std::make_shared<Replica>());

  rpc_server->Start();

  await::futures::BlockForever();
}
