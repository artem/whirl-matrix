#pragma once

#include <whirl/runtime/runtime.hpp>
#include <whirl/node/program/main.hpp>

#include <matrix/world/actor.hpp>
#include <matrix/world/time_model.hpp>
#include <matrix/fault/server.hpp>

#include <matrix/config/server.hpp>

#include <matrix/server/stdout.hpp>

#include <matrix/clocks/monotonic.hpp>
#include <matrix/clocks/wall.hpp>

#include <matrix/fs/fs.hpp>

#include <matrix/network/server.hpp>
#include <matrix/network/network.hpp>
#include <matrix/network/transport.hpp>

#include <matrix/process/memory.hpp>
#include <matrix/process/scheduler.hpp>

#include <timber/logger.hpp>

namespace whirl::matrix {

//////////////////////////////////////////////////////////////////////

class Server : public IActor, public fault::IFaultyServer, public net::IServer {
 private:
  enum class State {
    Initial,
    Running,
    Paused,
    Crashed,
  };

 public:
  Server(net::Network& network, ServerConfig config, node::program::Main program);

  // Non-copyable
  Server(const Server&) = delete;
  Server& operator=(const Server&) = delete;

  ~Server();

  // INetServer

  const std::string& HostName() const override {
    return config_.hostname;
  }

  net::ZoneId Zone() const override {
    return 0;  // TODO
  }

  void HandlePacket(const net::Packet& packet, net::Link* out) override;

  // IFaultyServer

  bool IsAlive() const override;

  // - Execution

  void Crash() override;
  void Launch() override;
  void FastReboot() override;

  void Pause() override;
  void Resume() override;

  // - Clocks

  void AdjustWallClock() override;

  // - Filesystem

  node::fs::FileList ListFiles(std::string_view prefix) override;
  void CorruptFile(const node::fs::Path& target) override;

  // IActor

  const std::string& Name() const override;

  void Start() override;

  bool IsRunnable() const override;
  TimePoint NextStepTime() const override;
  void Step() override;
  void Shutdown() override;

  // Simulation

  std::vector<std::string> GetStdout() const {
    return stdout_.lines;
  }

  size_t ComputeDigest() const;

  node::IRuntime& GetNodeRuntime();

  IServerTimeModel* GetTimeModel();

 private:
  node::IRuntime* MakeNodeRuntime();
  void StartProcess();

 private:
  State state_{State::Initial};

  IServerTimeModelPtr time_model_;

  ServerConfig config_;
  node::program::Main program_;

  // Hardware
  clocks::WallClock wall_clock_;
  clocks::MonotonicClock monotonic_clock_;

  // Operating system
  process::Scheduler scheduler_;
  fs::FileSystem filesystem_;
  mutable process::Memory heap_;
  net::Transport transport_;

  Stdout stdout_;

  // Node process
  node::IRuntime* runtime_{nullptr};

  timber::Logger logger_;
};

//////////////////////////////////////////////////////////////////////

Server& ThisServer();

}  // namespace whirl::matrix
