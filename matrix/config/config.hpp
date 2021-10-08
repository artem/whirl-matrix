#pragma once

#include <whirl/node/config/config.hpp>

#include <matrix/config/server.hpp>

#include <matrix/world/global/time_model.hpp>

#include <fmt/core.h>

#include <any>

namespace whirl::matrix::cfg {

class NodeConfig : public node::cfg::IConfig {
 public:
  NodeConfig(ServerConfig server_config) : server_(server_config) {
  }

  std::string GetString(std::string_view key) const override;
  int64_t GetInt64(std::string_view key) const override;
  bool GetBool(std::string_view key) const override;

 private:
  [[noreturn]] static void KeyNotFound(std::string_view key) {
    throw std::runtime_error(fmt::format("Key '{}' not found in configuration file", key));
  }

  std::any TryGetGlobal(std::string_view key) const;

 private:
  ServerConfig server_;
};

}  // namespace whirl::matrix::cfg
