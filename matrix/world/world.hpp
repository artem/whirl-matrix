#pragma once

#include <matrix/time/time.hpp>
#include <matrix/guids/generator.hpp>
#include <matrix/server/server.hpp>
#include <matrix/network/network.hpp>
#include <matrix/world/actor.hpp>
#include <matrix/world/actor_ctx.hpp>
#include <matrix/world/random_source.hpp>
#include <matrix/time_model/time_model.hpp>
#include <matrix/history/recorder.hpp>
#include <matrix/log/backend.hpp>
#include <matrix/trace/impl/tracer.hpp>

#include <matrix/time_model/catalog/adversary.hpp>

#include <matrix/helpers/digest.hpp>
#include <matrix/helpers/untyped_dict.hpp>

#include <timber/logger.hpp>

#include <wheels/support/id.hpp>

#include <deque>
#include <vector>

namespace whirl::matrix {

//////////////////////////////////////////////////////////////////////

struct NextStep {
  IActor* actor;
  size_t actor_index;
  TimePoint time;
};

//////////////////////////////////////////////////////////////////////

class World {
  struct WorldGuard {
    WorldGuard(World* world);
    ~WorldGuard();
  };

  using Servers = std::deque<Server>;

 public:
  World(size_t seed)
      : seed_(seed),
        random_source_(seed),
        time_model_(DefaultTimeModel()),
        network_(&log_backend_),
        logger_("World", &log_backend_) {
  }

  void AddServer(std::string hostname, node::program::Main program) {
    WorldGuard g(this);

    static const std::string kPoolName = "snowflakes";

    Servers& pool = pools_[kPoolName];
    return AddServerImpl(pool, program, kPoolName, hostname);
  }

  void AddPool(std::string pool_name, node::program::Main program, size_t size,
               std::string name_template) {
    WorldGuard g(this);

    Servers& pool = pools_[pool_name];
    for (size_t i = 0; i < size; ++i) {
      AddToPool(pool, program, pool_name, name_template);
    }
  }

  void AddClient(node::program::Main program) {
    WorldGuard g(this);

    AddToPool(clients_, program,
              /*pool_name=*/"clients",
              /*host_name_template=*/"Client");
  }

  void AddAdversary(node::program::Main program) {
    WorldGuard g(this);

    AddToPool(adversaries_, program,
              /*pool_name=*/"adversaries",
              /*host_name_template=*/"Adversary");
  }

  bool HasAdversary() const {
    return !adversaries_.empty();
  }

  void SetTimeModel(ITimeModelPtr time_model) {
    time_model_ = std::move(time_model);
  }

  void WriteLogTo(const std::string& fpath) {
    log_backend_.AppendToFile(fpath);
  }

  void WriteTraceTo(const std::string& fpath) {
    tracer_.emplace(fpath);
  }

  IServerTimeModelPtr MakeServerTimeModel(const std::string& hostname) {
    // TODO
    if (hostname.starts_with("Adversary")) {
      return MakeAdversaryTimeModel();
    }
    return time_model_->MakeServerModel(hostname);
  }

  void Start();
  void SetConfigGlobals();

  // Returns false if simulation is in deadlock state
  bool Step();

  // Stop simulation and compute digest
  size_t Stop();

  // Returns number of steps actually made
  size_t MakeSteps(size_t steps);

  // Time budget is _virtual_!
  void RunFor(Jiffies time_budget);

  void RestartServer(const std::string& hostname);

  // Methods used by running actors

  static World* Access();

  size_t Seed() const {
    return seed_;
  }

  size_t Digest() const {
    return digest_.GetValue();
  }

  Server& GetServer(const std::string& hostname) {
    return *FindServer(hostname);
  }

  net::Network& GetNetwork() {
    return network_;
  }

  size_t CurrentStep() const {
    return step_number_;
  }

  log::LogBackend& GetLog() {
    return log_backend_;
  }

  HistoryRecorder& GetHistoryRecorder() {
    return history_recorder_;
  }

  ITracer* GetTracer() {
    if (tracer_.has_value()) {
      return &tracer_.value();
    } else {
      return nullptr;
    }
  }

  // Context: Server
  std::vector<std::string> GetPool(const std::string& name) {
    auto it = pools_.find(name);

    if (it == pools_.end()) {
      return {};
    }

    Servers& pool = it->second;

    std::vector<std::string> hosts;
    for (auto& server : pool) {
      hosts.push_back(server.HostName());
    }
    return hosts;
  }

  const semantics::History& History() const {
    return history_recorder_.GetHistory();
  }

  std::vector<std::string> GetStdout(const std::string& hostname) {
    const Server* server = FindServer(hostname);
    return server->GetStdout();
  }

  TimePoint Now() const {
    return time_.Now();
  }

  Jiffies TimeElapsed() const {
    return time_.Now() - start_time_;
  }

  uint64_t RandomNumber() {
    return random_source_.Next();
  }

  ITimeModel* TimeModel() const {
    return time_model_.get();
  }

  IActor* CurrentActor() const {
    return active_.Get();
  }

  void SetGlobal(const std::string& name, std::any value) {
    globals_.Set(name, value);
  }

  std::any GetGlobal(const std::string& name) const {
    return globals_.Get(name);
  }

  // Context: Server!
  std::string GenerateGuid() {
    return guids_.GenerateNext();
  }

  ActorContext::ScopeGuard Scope(IActor* actor) {
    return active_.Scope(actor);
  }

  ActorContext::ScopeGuard Scope(IActor& actor) {
    return active_.Scope(&actor);
  }

 private:
  static ITimeModelPtr DefaultTimeModel();

  std::string MakeServerName(std::string name_template, size_t index) {
    wheels::StringBuilder name;
    name << name_template << '-' << index;
    return name;
  }

  void AddToPool(Servers& pool, node::program::Main program,
                 std::string pool_name, std::string host_name_template) {
    auto host_name = MakeServerName(host_name_template, pool.size() + 1);
    AddServerImpl(pool, program, pool_name, host_name);
  }

  // Returns host name
  void AddServerImpl(Servers& pool, node::program::Main program,
                     std::string pool_name, std::string hostname) {
    size_t id = server_ids_.NextId();

    pool.emplace_back(network_, ServerConfig{id, hostname, pool_name}, program);

    network_.AddServer(&pool.back());
    AddActor(&pool.back());
  }

  size_t ClusterSize() const {
    size_t count = 0;
    for (auto& [_, pool] : pools_) {
      count += pool.size();
    }
    return count;
  }

  Server* FindServer(const std::string& hostname) {
    for (auto& [_, pool] : pools_) {
      for (Server& server : pool) {
        if (server.HostName() == hostname) {
          return &server;
        }
      }
    }
    return nullptr;
  }

  void SetStartTime() {
    time_.FastForwardTo(TimeModel()->GlobalStartTime());
  }

  void AddActor(IActor* actor) {
    actors_.push_back(actor);
  }

  std::optional<NextStep> FindNextStep();

 private:
  const size_t seed_;

  Time time_;
  RandomSource random_source_;
  guids::GuidGenerator guids_;

  ITimeModelPtr time_model_;

  log::LogBackend log_backend_;

  // Actors

  std::map<std::string, Servers> pools_;
  Servers clients_;
  Servers adversaries_;

  wheels::IdGenerator server_ids_;

  net::Network network_;

  // Event loop

  std::vector<IActor*> actors_;
  ActorContext active_;

  size_t step_number_{0};

  TimePoint start_time_;

  DigestCalculator digest_;
  HistoryRecorder history_recorder_;

  std::optional<Tracer> tracer_;

  UntypedDict globals_;

  timber::Logger logger_;
};

}  // namespace whirl::matrix
