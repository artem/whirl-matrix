#pragma once

#include <matrix/world/time_model.hpp>

namespace whirl::matrix {

ITimeModel* TimeModel();

IServerTimeModel* ThisServerTimeModel();

IServerTimeModelPtr AcquireTimeModel(const std::string& hostname);

}  // namespace whirl::matrix
