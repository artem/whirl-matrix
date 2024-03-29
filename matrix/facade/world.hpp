#pragma once

#include <matrix/time_model/time_model.hpp>
#include <matrix/log/event.hpp>
#include <matrix/semantics/history.hpp>
#include <whirl/node/program/main.hpp>

#include <memory>
#include <ostream>
#include <any>

//////////////////////////////////////////////////////////////////////

namespace whirl::matrix {

class World;

}  // namespace whirl::matrix

//////////////////////////////////////////////////////////////////////

namespace whirl::matrix::facade {

// Facade

//////////////////////////////////////////////////////////////////////

class World;

//////////////////////////////////////////////////////////////////////

class PoolBuilder {
 public:
  PoolBuilder(World* world, std::string pool_name, node::program::Main program)
      : world_(world),
        pool_name_(pool_name),
        program_(program),
        name_template_(MakeNameTemplate(pool_name)) {
  }

  PoolBuilder& Size(size_t value) {
    size_ = value;
    return *this;
  }

  PoolBuilder& NameTemplate(std::string value) {
    name_template_ = value;
    return *this;
  }

  // Add pool to the world
  ~PoolBuilder();

 private:
  static std::string MakeNameTemplate(std::string pool_name) {
    return std::string("Server-") + pool_name;
  }

 private:
  World* world_;

  std::string pool_name_;
  node::program::Main program_;
  size_t size_ = 1;
  std::string name_template_;
};

//////////////////////////////////////////////////////////////////////

class World {
  static const size_t kDefaultSeed = 42;

  friend class PoolBuilder;

 public:
  World(size_t seed = kDefaultSeed);
  ~World();

  size_t Seed() const;

  void AddServer(std::string hostname, node::program::Main program);

  PoolBuilder MakePool(std::string pool_name, node::program::Main program) {
    return PoolBuilder{this, pool_name, program};
  }

  void AddClient(node::program::Main program);
  void AddClients(node::program::Main program, size_t count);

  void SetTimeModel(ITimeModelPtr time_model);

  void AddAdversary(node::program::Main program);

  // Globals

  template <typename T>
  void SetGlobal(const std::string& key, T value) {
    SetGlobalImpl(key, value);
  }

  template <typename T>
  T GetGlobal(const std::string& key) const {
    auto value = GetGlobalImpl(key);
    return std::any_cast<T>(value);
  }

  // Global counters

  void InitCounter(const std::string& name, size_t value = 0) {
    SetGlobal(name, value);
  }

  size_t GetCounter(const std::string& name) const {
    return GetGlobal<size_t>(name);
  }

  void WriteLogTo(std::string fpath);

  void WriteTraceTo(std::string fpath);

  void Start();

  bool Step();
  void MakeSteps(size_t count);

  // For tests
  void RestartServer(const std::string& hostname);

  // Returns simulation digest
  size_t Stop();

  size_t Digest() const;

  size_t StepCount() const;
  Jiffies TimeElapsed() const;

  const log::EventLog& EventLog() const;
  const semantics::History& History() const;

  std::vector<std::string> GetStdout(const std::string& hostname) const;

 private:
  void AddPool(std::string pool_name, node::program::Main program, size_t size,
               std::string server_name_template);

  void SetGlobalImpl(const std::string& key, std::any value);
  std::any GetGlobalImpl(const std::string& key) const;

 private:
  std::unique_ptr<matrix::World> impl_;
};

}  // namespace whirl::matrix::facade
