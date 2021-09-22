#pragma once

#include <matrix/time_model/time_model.hpp>

namespace whirl::matrix {

ITimeModelPtr MakeTransition(ITimeModelPtr before, ITimeModelPtr after,
                             TimePoint transition_time);

}  // namespace whirl::matrix
