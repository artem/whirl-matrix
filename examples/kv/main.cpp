#include "node/main.hpp"

// Node
#include <whirl/node/program/prologue.hpp>
#include <whirl/node/runtime/shortcuts.hpp>

// RPC
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

// Simulation
#include <matrix/facade/world.hpp>
#include <matrix/world/global/vars.hpp>
#include <matrix/client/main.hpp>
#include <matrix/client/rpc.hpp>
#include <matrix/client/util.hpp>
#include <matrix/test/random.hpp>
#include <matrix/test/main.hpp>
#include <matrix/test/event_log.hpp>
#include <matrix/test/runner.hpp>

#include <matrix/fault/access.hpp>
#include <matrix/fault/net/star.hpp>

#include <matrix/semantics/printers/kv.hpp>
#include <matrix/semantics/checker/check.hpp>
#include <matrix/semantics/models/kv.hpp>

#include <commute/rpc/id.hpp>

#include <algorithm>

using await::futures::Future;
using await::fibers::Await;
using wheels::Result;

using namespace whirl;

//////////////////////////////////////////////////////////////////////

using Key = std::string;
using Value = uint32_t;

//////////////////////////////////////////////////////////////////////

class KVBlockingStub {
 public:
  KVBlockingStub(commute::rpc::IChannelPtr channel) : channel_(channel) {
  }

  void Set(Key key, Value value) {
    Await(commute::rpc::Call("KV.Set")  //
              .Args(key, value)
              .Via(channel_)
              .TraceWith(GenerateTraceId("Set"))
              .Start()
              .As<void>())
        .ThrowIfError();
  }

  Value Get(Key key) {
    return Await(commute::rpc::Call("KV.Get")  //
                     .Args(key)
                     .Via(channel_)
                     .TraceWith(GenerateTraceId("Get"))
                     .Start()
                     .As<Value>())
        .ValueOrThrow();
  }

 private:
  std::string GenerateTraceId(std::string cmd) const {
    return fmt::format("{}-{}", cmd, node::rt::GenerateGuid());
  }

 private:
  commute::rpc::IChannelPtr channel_;
};

//////////////////////////////////////////////////////////////////////

static const std::vector<std::string> kKeys({"a", "b", "c"});

const std::string& ChooseRandomKey() {
  return kKeys.at(node::rt::RandomNumber(matrix::GetGlobal<size_t>("keys")));
}

//////////////////////////////////////////////////////////////////////

[[noreturn]] void Client() {
  matrix::client::Prologue();

  timber::Logger logger_{"Client", node::rt::LoggerBackend()};

  KVBlockingStub kv_store{matrix::client::MakeRpcChannel(
      /*pool_name=*/"kv", /*port=*/42)};

  for (size_t i = 1;; ++i) {
    Key key = ChooseRandomKey();
    if (matrix::client::Either()) {
      Value value = node::rt::RandomNumber(1, 100);
      LOG_INFO("Execute Set({}, {})", key, value);
      kv_store.Set(key, value);
      LOG_INFO("Set completed");
    } else {
      LOG_INFO("Execute Get({})", key);
      [[maybe_unused]] Value result = kv_store.Get(key);
      LOG_INFO("Get({}) -> {}", key, result);
    }

    matrix::GlobalCounter("requests").Increment();

    // Random pause
    node::rt::SleepFor(node::rt::RandomNumber(1, 100));
  }
}

//////////////////////////////////////////////////////////////////////

[[noreturn]] void Adversary() {
  timber::Logger logger_{"Adversary", node::rt::LoggerBackend()};

  // List system nodes
  auto pool = node::rt::Discovery()->ListPool("kv");

  auto& net = matrix::fault::Network();

  while (true) {
    node::rt::SleepFor(node::rt::RandomNumber(10, 1000));

    size_t center = node::rt::RandomNumber(pool.size());

    LOG_INFO("Make star with center at {}", pool[center]);

    matrix::fault::MakeStar(pool, center);

    node::rt::SleepFor(node::rt::RandomNumber(100, 300));

    net.Heal();
  }
}

//////////////////////////////////////////////////////////////////////

// Sequential specification for KV storage
// Used by linearizability checker
using KVStoreModel = semantics::KVStoreModel<Key, Value>;

//////////////////////////////////////////////////////////////////////

// Seed -> simulation digest
// Deterministic
size_t RunSimulation(size_t seed) {
  auto& runner = matrix::TestRunner::Access();

  static const size_t kTimeLimit = 10000;
  static const size_t kRequestsThreshold = 7;

  matrix::Random random{seed};

  // Randomize simulation parameters
  const size_t replicas = random.Get(3, 5);
  const size_t clients = random.Get(2, 3);
  const size_t keys = random.Get(1, 2);

  runner.Debug() << "Simulation seed: " << seed << std::endl;

  runner.Debug() << "Parameters: "
            << "replicas = " << replicas << ", "
            << "clients = " << clients << ", "
            << "keys = " << keys << std::endl;

  // Reset RPC ids
  commute::rpc::ResetIds();

  matrix::facade::World world{seed};

  // Cluster
  world.MakePool("kv", KVNodeMain).Size(replicas);

  // Clients
  world.AddClients(Client, /*count=*/clients);

  world.AddAdversary(Adversary);

  // Log file
  auto log_fpath = runner.LogFile();
  if (log_fpath) {
    world.WriteLogTo(*log_fpath);
  }

  // Globals
  world.SetGlobal("keys", keys);
  world.InitCounter("requests", 0);

  // Run simulation
  world.Start();
  while (world.GetCounter("requests") < kRequestsThreshold &&
         world.TimeElapsed() < kTimeLimit) {
    if (!world.Step()) {
      break;  // Deadlock
    }
  }

  // Stop and compute simulation digest
  size_t digest = world.Stop();

  // Print report
  runner.Debug() << "Seed " << seed << " -> "
            << "digest: " << digest << ", time: " << world.TimeElapsed()
            << ", steps: " << world.StepCount() << std::endl;

  const auto event_log = world.EventLog();

  // Time limit exceeded
  if (world.GetCounter("requests") < kRequestsThreshold) {
    // Log
    std::cout << "Log:" << std::endl;
    matrix::WriteTextLog(event_log, std::cout);
    std::cout << std::endl;

    if (world.TimeElapsed() < kTimeLimit) {
      runner.Out() << "Deadlock in simulation" << std::endl;
    } else {
      runner.Out() << "Simulation time limit exceeded" << std::endl;
    }
    runner.Fail();
  }

  runner.Debug() << "Requests completed: " << world.GetCounter("requests")
            << std::endl;

  // Check linearizability
  const auto history = world.History();
  const bool linearizable = semantics::LinCheck<KVStoreModel>(history);

  if (!linearizable) {
    // Log
    runner.Debug() << "Log:" << std::endl;
    matrix::WriteTextLog(event_log, runner.Debug());
    runner.Debug() << std::endl;

    // History
    runner.Out() << "History is NOT LINEARIZABLE for seed = "
      << seed << ":" << std::endl;
    semantics::PrintKVHistory<Key, Value>(history, runner.Out());

    runner.Fail();
  }

  return digest;
}

// Usage:
// 1) --det --sims 12345 - check determinism and run 12345 simulations
// 2) --seed 54321 - run single simulation with seed 54321

int main(int argc, const char** argv) {
  return matrix::Main(argc, argv, RunSimulation);
}
