#pragma once

#include <matrix/world/global/actor.hpp>
#include <matrix/world/global/random.hpp>
#include <matrix/world/global/time.hpp>
#include <matrix/world/global/vars.hpp>
#include <matrix/world/global/guids.hpp>

#include <matrix/time_model/time_model.hpp>
#include <matrix/history/recorder.hpp>
#include <matrix/log/backend.hpp>

#include <cstdlib>
#include <vector>
#include <string>
#include <any>

namespace whirl::matrix {

// Global world services used by different components of simulation

//////////////////////////////////////////////////////////////////////

size_t WorldSeed();
size_t WorldStepNumber();

bool IsThereAdversary();

HistoryRecorder& GetHistoryRecorder();

std::vector<std::string> GetPool(const std::string& name);

}  // namespace whirl::matrix
