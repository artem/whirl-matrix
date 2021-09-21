#include <matrix/world/global/global.hpp>

#include <matrix/world/world.hpp>

namespace whirl::matrix {

static World* ThisWorld() {
  return World::Access();
}

//////////////////////////////////////////////////////////////////////

size_t WorldSeed() {
  return ThisWorld()->Seed();
}

//////////////////////////////////////////////////////////////////////

uint64_t GlobalRandomNumber() {
  return ThisWorld()->RandomNumber();
}

//////////////////////////////////////////////////////////////////////

TimePoint GlobalNow() {
  return ThisWorld()->Now();
}

//////////////////////////////////////////////////////////////////////

// Behaviour

ITimeModel* TimeModel() {
  return ThisWorld()->TimeModel();
}

bool IsThereAdversary() {
  return ThisWorld()->HasAdversary();
}

//////////////////////////////////////////////////////////////////////

HistoryRecorder& GetHistoryRecorder() {
  return ThisWorld()->GetHistoryRecorder();
}

//////////////////////////////////////////////////////////////////////

std::vector<std::string> GetPool(const std::string& name) {
  return ThisWorld()->GetPool(name);
}

//////////////////////////////////////////////////////////////////////

size_t WorldStepNumber() {
  return ThisWorld()->CurrentStep();
}

bool AmIActor() {
  return ThisWorld()->CurrentActor() != nullptr;
}

IActor* ThisActor() {
  return ThisWorld()->CurrentActor();
}

//////////////////////////////////////////////////////////////////////

std::string GenerateGuid() {
  return ThisWorld()->GenerateGuid();
}

//////////////////////////////////////////////////////////////////////

timber::ILogBackend* GetLogBackend() {
  return &(ThisWorld()->GetLog());
}

//////////////////////////////////////////////////////////////////////

namespace detail {

std::any GetGlobal(const std::string& name) {
  return ThisWorld()->GetGlobal(name);
}

void SetGlobal(const std::string& name, std::any value) {
  return ThisWorld()->SetGlobal(name, std::move(value));
}

}  // namespace detail

}  // namespace whirl::matrix
