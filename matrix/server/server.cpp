#include <matrix/server/server.hpp>

#include <matrix/world/global/actor.hpp>
#include <matrix/world/global/log.hpp>
#include <matrix/world/global/scope.hpp>

#include <matrix/process/trampoline.hpp>

#include <matrix/server/runtime/runtime.hpp>

#include <matrix/helpers/digest.hpp>

#include <timber/log.hpp>

namespace whirl::matrix {

//////////////////////////////////////////////////////////////////////

Server::Server(net::Network& net, ServerConfig config,
               node::program::Main program)
    : config_(config),
      program_(program),
      transport_(net, config.hostname, heap_, scheduler_),
      logger_("Server", GetLogBackend()) {
}

Server::~Server() {
  WHEELS_VERIFY(state_ == State::Crashed, "Invalid state");
}

// INetServer

void Server::HandlePacket(const net::Packet& packet, net::Link* out) {

  auto actor_scope = SwitchToActor(this);
  transport_.HandlePacket(packet, out);
}

// IFaultyServer

bool Server::IsAlive() const {
  return state_ == State::Running || state_ == State::Paused;
}

void Server::Crash() {
#if __has_feature(address_sanitizer)
  WHEELS_PANIC("Crashes are incompatible with Address Sanitizer");
#endif

  GlobalAllocatorGuard g;

  WHEELS_VERIFY(state_ != State::Crashed, "Server already crashed");

  LOG_INFO("Crash server {}", HostName());

  auto actor_scope = SwitchToActor(this);

  // Remove all network endpoints
  transport_.Reset();
  // Close opened files
  filesystem_.Reset();
  // Drop scheduled tasks
  scheduler_.Reset();
  // 2) Clean memory
  heap_.Reset();

  runtime_ = nullptr;

  // Clear stdout?

  state_ = State::Crashed;
}

void Server::FastReboot() {
  GlobalAllocatorGuard g;

  if (!IsAlive()) {
    return;
  }

  Crash();
  Launch();
}

void Server::Pause() {
  GlobalAllocatorGuard g;

  if (!IsAlive()) {
    return;
  }

  if (state_ == State::Paused) {
    return;
  }

  //WHEELS_VERIFY(state_ != State::Paused, "Server already paused");
  state_ = State::Paused;
}

void Server::Resume() {
  GlobalAllocatorGuard g;

  if (state_ != State::Paused) {
    return;
  }

  auto actor_scope = SwitchToActor(this);

  WHEELS_VERIFY(state_ == State::Paused, "Server is not paused");

  scheduler_.Resume(GlobalNow());

  state_ = State::Running;
}

void Server::AdjustWallClock() {
  LOG_INFO("Adjust wall time clock on {}", HostName());

  GlobalAllocatorGuard g;

  {
    auto actor_scope = SwitchToActor(this);
    wall_clock_.AdjustOffset();
  }
}

persist::fs::FileList Server::ListFiles(std::string_view prefix) {
  persist::fs::FileList listed;

  auto iter = filesystem_.ListAllFiles();
  while (iter.IsValid()) {
    if ((*iter).starts_with(prefix)) {
      listed.push_back(*iter);
    }
  }

  return listed;
}

void Server::CorruptFile(const persist::fs::Path& file_path) {
  GlobalAllocatorGuard g;
  filesystem_.Corrupt(file_path);
}

// IActor

const std::string& Server::Name() const {
  return HostName();
}

void Server::Start() {
  time_model_ = AcquireTimeModel(config_.hostname);

  wall_clock_.Init();
  monotonic_clock_.Init();

  Launch();
}

void Server::Launch() {
  WHEELS_VERIFY(state_ == State::Initial || state_ == State::Crashed,
                "Invalid state");

  auto actor_scope = SwitchToActor(this);

  monotonic_clock_.Reset();

  LOG_INFO("Starting process");
  StartProcess();

  state_ = State::Running;
}

void Server::StartProcess() {
  auto guard = heap_.Use();

  // Prepare runtime
  runtime_ = MakeNodeRuntime();

  // Run user program
  process::Schedule(scheduler_, GlobalNow(), [this]() {
    process::MainTrampoline(program_);
  });
}

bool Server::IsRunnable() const {
  if (state_ != State::Running) {
    return false;
  }
  return !scheduler_.IsEmpty();
}

TimePoint Server::NextStepTime() const {
  return scheduler_.NextTaskTime();
}

void Server::Step() {
  process::ITask* task = scheduler_.TakeNext();
  {
    auto g = heap_.Use();
    task->Run();
  }
}

void Server::Shutdown() {
  if (state_ != State::Crashed) {
    Crash();
  }
}

size_t Server::ComputeDigest() const {
  if (state_ == State::Crashed) {
    return 0;
  }

  DigestCalculator digest;
  // Memory
  digest.Eat(heap_.BytesAllocated());
  // Fs
  digest.Combine(filesystem_.ComputeDigest());
  return digest.GetValue();
}

// Private

node::IRuntime* Server::MakeNodeRuntime() {
  NodeRuntime* runtime = new NodeRuntime();

  runtime->thread_pool.Init(scheduler_);
  runtime->fibers.Init();

  runtime->time.Init(wall_clock_, monotonic_clock_, scheduler_);

  runtime->fs.Init(&filesystem_, runtime->time.Get());

  runtime->db.Init(runtime->fs.Get());

  runtime->transport.Init(transport_);

  runtime->random.Init();
  runtime->guids.Init();

  runtime->true_time.Init();

  runtime->config.Init(config_);

  runtime->discovery.Init();

  runtime->terminal.Init(stdout_);

  return runtime;
}

node::IRuntime& Server::GetNodeRuntime() {
  return *runtime_;
}

IServerTimeModel* Server::GetTimeModel() {
  return time_model_.get();
}

//////////////////////////////////////////////////////////////////////

Server& ThisServer() {
  Server* this_server = dynamic_cast<Server*>(ThisActor());
  WHEELS_VERIFY(this_server != nullptr, "Current actor is not a server");
  return *this_server;
}

}  // namespace whirl::matrix
