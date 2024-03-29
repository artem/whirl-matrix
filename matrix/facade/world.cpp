#include <matrix/facade/world.hpp>

#include <matrix/world/world.hpp>

namespace whirl::matrix::facade {

PoolBuilder::~PoolBuilder() {
  world_->AddPool(pool_name_, program_, size_, name_template_);
}

World::World(size_t seed) : impl_(std::make_unique<matrix::World>(seed)) {
}

World::~World() {
}

size_t World::Seed() const {
  return impl_->Seed();
}

void World::AddServer(std::string hostname, node::program::Main program) {
  impl_->AddServer(hostname, program);
}

void World::AddPool(std::string pool_name, node::program::Main program,
                    size_t size, std::string server_name_template) {
  impl_->AddPool(pool_name, program, size, server_name_template);
}

void World::AddClient(node::program::Main program) {
  impl_->AddClient(program);
}

void World::AddClients(node::program::Main program, size_t count) {
  for (size_t i = 0; i < count; ++i) {
    AddClient(program);
  }
}

void World::SetTimeModel(ITimeModelPtr time_model) {
  impl_->SetTimeModel(std::move(time_model));
}

void World::AddAdversary(node::program::Main program) {
  impl_->AddAdversary(program);
}

void World::WriteLogTo(std::string fpath) {
  impl_->WriteLogTo(fpath);
}

void World::WriteTraceTo(std::string fpath) {
  impl_->WriteTraceTo(fpath);
}

void World::Start() {
  impl_->Start();
}

bool World::Step() {
  return impl_->Step();
}

void World::MakeSteps(size_t count) {
  impl_->MakeSteps(count);
}

void World::RestartServer(const std::string& hostname) {
  impl_->RestartServer(hostname);
}

size_t World::Stop() {
  return impl_->Stop();
}

size_t World::Digest() const {
  return impl_->Digest();
}

const log::EventLog& World::EventLog() const {
  return impl_->GetLog().GetEvents();
}

const semantics::History& World::History() const {
  return impl_->History();
}

std::vector<std::string> World::GetStdout(const std::string& hostname) const {
  return impl_->GetStdout(hostname);
}

size_t World::StepCount() const {
  return impl_->CurrentStep();
}

Jiffies World::TimeElapsed() const {
  return impl_->TimeElapsed();
}

void World::SetGlobalImpl(const std::string& key, std::any value) {
  impl_->SetGlobal(key, value);
}

std::any World::GetGlobalImpl(const std::string& key) const {
  return impl_->GetGlobal(key);
}

}  // namespace whirl::matrix::facade
