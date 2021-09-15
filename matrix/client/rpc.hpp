#pragma once

#include <whirl/node/rpc/channel.hpp>

namespace whirl::matrix::client {

rpc::IChannelPtr MakeRpcChannel(const std::string& pool_name);

}  // namespace whirl::matrix::client
