#pragma once

#include <commute/rpc/channel.hpp>

namespace whirl::matrix::client {

commute::rpc::IChannelPtr MakeRpcChannel(const std::string& pool_name,
                                         uint16_t port,
                                         bool log_retries = true);

}  // namespace whirl::matrix::client
