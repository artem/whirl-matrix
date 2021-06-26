#include <whirl/engines/matrix/client/client.hpp>

#include <whirl/engines/matrix/channels/random.hpp>
#include <whirl/rpc/retries.hpp>
#include <whirl/engines/matrix/channels/history.hpp>

namespace whirl::matrix {

//////////////////////////////////////////////////////////////////////

static rpc::BackoffParams RetriesBackoff() {
  return {50, 1000, 2};  // Magic
}

//////////////////////////////////////////////////////////////////////

rpc::IChannelPtr ClientBase::MakeTransportChannel(const std::string& peer) {
  return RPCClient()->Dial(peer);
}

rpc::IChannelPtr ClientBase::MakeClientChannel() {
  // Peer channels
  std::vector<rpc::IChannelPtr> channels;
  for (const auto& addr : Cluster()) {
    channels.push_back(MakeTransportChannel(addr));
  }

  // Retries -> History -> Random -> Peers

  auto random = MakeRandomChannel(std::move(channels));
  auto history = MakeHistoryChannel(std::move(random));
  auto retries =
      WithRetries(std::move(history), TimeService(), RetriesBackoff());

  return retries;
}

void ClientBase::Main() {
  await::fibers::self::SetName("main");

  RandomPause();
  DiscoverCluster();
  ConnectToClusterNodes();
  MainThread();
}

//////////////////////////////////////////////////////////////////////

rpc::IChannelPtr ExactlyOnceClientBase::MakeClientChannel() {
  // Peer channels
  std::vector<rpc::IChannelPtr> channels;
  for (const auto& addr : Cluster()) {
    channels.push_back(MakeTransportChannel(addr));
  }

  // History -> Retries -> Random -> Peers

  auto random = MakeRandomChannel(std::move(channels));
  auto retries =
      WithRetries(std::move(random), TimeService(), RetriesBackoff());
  auto history = MakeHistoryChannel(std::move(retries));

  return history;
}

}  // namespace whirl::matrix
