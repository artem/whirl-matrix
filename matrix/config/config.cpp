#include <matrix/config/config.hpp>

namespace whirl::matrix::cfg {

std::string NodeConfig::GetString(std::string_view key) const {
  // This server

  if (key == "pool.name" || key == "pool") {
    return server_.pool;
  }

  // Global constant

  if (key == "db.path") {
    return "/db";
  }
  KeyNotFound(key);
}

int64_t NodeConfig::GetInt64(std::string_view key) const {
  // This server

  if (key == "node.id") {
    return server_.id;
  }

  // Global constant

  if (key == "rpc.port") {
    return 42;
  }

  // Backoff params
  // Defined by time model

  if (key == "rpc.backoff.init") {
    return TimeModel()->BackoffParams().init;
  }
  if (key == "rpc.backoff.max") {
    return TimeModel()->BackoffParams().max;
  }
  if (key == "rpc.backoff.factor") {
    return TimeModel()->BackoffParams().factor;
  }

  KeyNotFound(key);
}

bool NodeConfig::GetBool(std::string_view key) const {
  KeyNotFound(key);
}

}  // namespace whirl::matrix::cfg
