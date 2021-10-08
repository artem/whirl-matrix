#include <matrix/config/config.hpp>

#include <matrix/world/global/vars.hpp>

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

  // Globals

  {
    auto value = TryGetGlobal(key);
    if (value.has_value()) {
      return std::any_cast<std::string>(value);
    }
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

  // Globals

  {
    auto value = TryGetGlobal(key);
    if (value.has_value()) {
      return std::any_cast<int64_t>(value);
    }
  }


  KeyNotFound(key);
}

bool NodeConfig::GetBool(std::string_view key) const {
  // Globals

  {
    auto value = TryGetGlobal(key);
    if (value.has_value()) {
      return std::any_cast<bool>(value);
    }
  }


  KeyNotFound(key);
}

std::any NodeConfig::TryGetGlobal(std::string_view key) const {
  auto var = fmt::format("config.{}", key);
  return matrix::detail::GetGlobal(var);
}

}  // namespace whirl::matrix::cfg
