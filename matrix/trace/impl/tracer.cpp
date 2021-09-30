#include <matrix/trace/impl/tracer.hpp>

#include <matrix/world/global/time.hpp>

#include <matrix/log/bytes.hpp>

#include <muesli/serialize.hpp>

namespace whirl::matrix {

//////////////////////////////////////////////////////////////////////

template <typename T>
std::optional<T> TryDeserialize(std::string payload) {
  try {
    return muesli::Deserialize<T>(payload);
  } catch (...) {
    return std::nullopt;
  }
}

//////////////////////////////////////////////////////////////////////

Tracer::Tracer(const std::string& path)
    : file_(path, std::ofstream::out), writer_(file_) {
  writer_.OpenList();
}

bool Tracer::IsData(const net::Frame& frame) const {
  return frame.packet.header.type == net::Packet::Type::Data;
}

void Tracer::Deliver(const net::Frame& frame) {
  if (!IsData(frame)) {
    return;  // Ignore service packets
  }

  TimePoint receive_time = GlobalNow();

  auto packet = frame.packet;

  auto payload = frame.packet.message;

  writer_.OpenMap();

  writer_.WriteField("type");
  writer_.WriteString("message");

  writer_.WriteField("source_host");
  writer_.WriteString(frame.header.source_host);

  writer_.WriteField("dest_host");
  writer_.WriteString(frame.header.dest_host);

  writer_.WriteField("send_time");
  writer_.WriteInteger(frame.header.send_time);

  writer_.WriteField("receive_time");
  writer_.WriteInteger(receive_time);

  writer_.WriteField("payload");
  writer_.OpenMap();

  // Payload
  if (auto req = TryDeserialize<commute::rpc::proto::Request>(payload)) {
    WriteRequest(*req);
  } else if (auto rsp = TryDeserialize<commute::rpc::proto::Response>(payload)) {
    WriteResponse(*rsp);
  } else {
    // ???
  }

  writer_.CloseMap();

  writer_.CloseMap();
}

static std::string Describe(commute::rpc::Method method) {
  return method.service + "." + method.name;
}

void Tracer::WriteResponse(commute::rpc::proto::Response rsp) {
  writer_.WriteField("type");
  writer_.WriteString("commute::rpc::proto::Response");

  writer_.WriteField("id");
  writer_.WriteInteger(rsp.request_id);

  writer_.WriteField("trace_id");
  writer_.WriteString(rsp.trace_id);

  writer_.WriteField("method");
  writer_.WriteString(Describe(rsp.method));

  writer_.WriteField("error");
  writer_.WriteInteger((int)rsp.error);

  if (rsp.IsOk()) {
    // TODO: result
  }
}

void Tracer::WriteRequest(commute::rpc::proto::Request req) {
  writer_.WriteField("type");
  writer_.WriteString("commute::rpc::proto::Request");

  writer_.WriteField("id");
  writer_.WriteInteger(req.id);

  writer_.WriteField("trace_id");
  writer_.WriteString(req.trace_id);

  writer_.WriteField("method");
  writer_.WriteString(Describe(req.method));

  // TODO: arguments
}

void Tracer::Stop() {
  writer_.CloseList();
  file_ << std::endl;
  file_.flush();
}

}  // namespace whirl::matrix
