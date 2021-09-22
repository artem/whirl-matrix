#pragma once

#include <matrix/time_model/time_model.hpp>

namespace whirl::matrix {

// Asynchronous world
ITimeModelPtr MakeCrazyTimeModel();

}  // namespace whirl::matrix
