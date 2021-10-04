#pragma once

#include <whirl/node/db/kv.hpp>

#include <map>

namespace whirl::matrix::db {

using Entries = std::map<node::db::Key, node::db::Value>;

}  // namespace whirl::matrix::db
