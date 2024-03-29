#include <matrix/world/world.hpp>

#include <matrix/time_model/catalog/crazy.hpp>

#include <matrix/runtime/setup.hpp>

#include <timber/log.hpp>

#include <cstdlib>

namespace whirl::matrix {

//////////////////////////////////////////////////////////////////////

static World* this_world = nullptr;

World::WorldGuard::WorldGuard(World* world) {
  this_world = world;
}

World::WorldGuard::~WorldGuard() {
  this_world = nullptr;
}

World* World::Access() {
  WHEELS_VERIFY(this_world != nullptr, "Not in world context");
  return this_world;
}

//////////////////////////////////////////////////////////////////////

ITimeModelPtr World::DefaultTimeModel() {
  return MakeCrazyTimeModel();
}

void World::SetConfigGlobals() {
  // RTT estimation
  Jiffies rtt = time_model_->EstimateRtt();
  SetGlobal("config.net.rtt", (int64_t)rtt.Count());
}

void World::Start() {
  WorldGuard g(this);

  SetupMatrixRuntime();

  LOG_INFO("Seed: {}", seed_);

  SetStartTime();
  start_time_ = time_.Now();

  time_model_->Initialize();

  // Start network:
  AddActor(&network_);
  Scope(network_)->Start();

  LOG_INFO("Cluster: {}, clients: {}", ClusterSize(), clients_.size());

  LOG_INFO("Starting cluster...");

  SetConfigGlobals();

  // Start servers
  for (auto& [_, pool] : pools_) {
    for (auto& server : pool) {
      Scope(server)->Start();
    }
  }

  LOG_INFO("Starting clients...");

  // Start clients
  for (auto& client : clients_) {
    Scope(client)->Start();
  }

  LOG_INFO("Starting adversaries...");

  // Start adversaries
  for (auto& adversary : adversaries_) {
    Scope(adversary)->Start();
  }

  LOG_INFO("World started");
}

bool World::Step() {
  WorldGuard g(this);

  auto next = FindNextStep();
  if (!next.has_value()) {
    return false;
  }

  ++step_number_;

  digest_.Eat(next->time).Eat(next->actor_index);

  time_.FastForwardTo(next->time);

  // For determinism violation debugging
  LOG_TRACE("Next step: {}, actor: {}, random source touched: {} times",
            step_number_, next->actor->Name(), random_source_.Steps());

  Scope(next->actor)->Step();

  return true;
}

std::optional<NextStep> World::FindNextStep() {
  if (actors_.empty()) {
    return std::nullopt;
  }

  std::optional<NextStep> next_step;

  for (size_t i = 0; i < actors_.size(); ++i) {
    IActor* actor = actors_[i];

    if (actor->IsRunnable()) {
      TimePoint next_step_time = actor->NextStepTime();

      if (!next_step || next_step_time < next_step->time) {
        next_step = {actor, i, next_step_time};
      }
    }
  }

  return next_step;
}

size_t World::Stop() {
  WorldGuard g(this);

#if __has_feature(address_sanitizer)
  //std::quick_exit(0);
  std::exit(0);
#endif

  // Adversaries

  for (auto& adversary : adversaries_) {
    Scope(adversary)->Shutdown();
  }
  adversaries_.clear();

  LOG_INFO("Adversaries stopped");

  // Network

  digest_.Combine(network_.Digest());
  Scope(network_)->Shutdown();

  LOG_INFO("Network stopped");

  // Servers

  for (auto& [_, pool] : pools_) {
    for (auto& server : pool) {
      digest_.Combine(server.ComputeDigest());
      Scope(server)->Shutdown();
    }
    pool.clear();
  }
  pools_.clear();

  LOG_INFO("Servers stopped");

  // Clients

  for (auto& client : clients_) {
    Scope(client)->Shutdown();
  }
  clients_.clear();

  LOG_INFO("Clients stopped");

  actors_.clear();

  // Finalize

  if (tracer_) {
    tracer_->Finalize();
  }

  history_recorder_.Finalize();

  LOG_INFO("Simulation stopped");

  return Digest();
}

size_t World::MakeSteps(size_t steps) {
  size_t steps_made = 0;
  for (size_t i = 0; i < steps; ++i) {
    if (!Step()) {
      break;
    }
    ++steps_made;
  }
  return steps_made;
}

void World::RunFor(Jiffies time_budget) {
  while (TimeElapsed() < time_budget) {
    if (!Step()) {
      break;
    }
  }
}

void World::RestartServer(const std::string& hostname) {
  WorldGuard g(this);

  FindServer(hostname)->FastReboot();
}

}  // namespace whirl::matrix
