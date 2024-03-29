#include <matrix/history/channel.hpp>

#include <commute/rpc/channel.hpp>
#include <commute/rpc/errors.hpp>

#include <matrix/world/global/global.hpp>

#include <await/futures/core/future.hpp>
#include <await/futures/helpers.hpp>

using namespace commute::rpc;
using await::futures::Future;
using wheels::Result;

namespace whirl::matrix {

using Cookie = HistoryRecorder::Cookie;

class HistoryChannel : public IChannel {
 public:
  HistoryChannel(IChannelPtr impl) : impl_(std::move(impl)) {
  }

  void Close() override {
    impl_->Close();
  }

  const std::string& Peer() const override {
    return impl_->Peer();
  }

  Future<Message> Call(const Method& method, const Message& input,
                       CallOptions options) override {
    auto& recorder = GetHistoryRecorder();

    auto cookie = recorder.CallStarted(method.name, input);

    if (!options.trace_id.empty()) {
      recorder.AddLabel(cookie, options.trace_id);
    }

    auto f = impl_->Call(method, input, std::move(options));

    auto record = [cookie](const Result<Message>& result) mutable {
      RecordCallResult(cookie, result);
    };

    return await::futures::SubscribeConst(std::move(f), std::move(record));
  }

 private:
  static void RecordCallResult(Cookie cookie, const Result<Message>& result) {
    auto& recorder = GetHistoryRecorder();

    if (result.IsOk()) {
      recorder.CallCompleted(cookie, result.ValueUnsafe());
    } else {
      if (MaybeCompleted(result.GetErrorCode())) {
        recorder.CallLost(cookie);
      } else {
        recorder.RemoveCall(cookie);
      }
    }
  }

  static bool MaybeCompleted(const std::error_code e) {
    return e == RPCErrorCode::TransportError ||
           e == RPCErrorCode::ExecutionError;
  }

 private:
  IChannelPtr impl_;
};

IChannelPtr MakeHistoryChannel(IChannelPtr channel) {
  return std::make_shared<HistoryChannel>(std::move(channel));
}

}  // namespace whirl::matrix
