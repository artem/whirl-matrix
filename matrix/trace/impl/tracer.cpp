#include <matrix/trace/impl/tracer.hpp>

#include <matrix/world/global/time.hpp>

#include <matrix/log/bytes.hpp>

#include <cereal/external/rapidjson/document.h>

#include <muesli/serialize.hpp>
#include <muesli/archives.hpp>

#include <wheels/support/assert.hpp>

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

template <typename JsonWriter>
static void WriteJson(JsonWriter& writer, const std::string& json) {
  rapidjson::Document doc;
  doc.Parse(json.c_str());
  doc.Accept(writer);
}

template <typename JsonWriter>
static void WriteMessage(JsonWriter& writer, const std::string& message) {
  if (muesli::archives::IsBinaryFormat()) {
    writer.String("<binary>");
  } else {
    WriteJson(writer, message);
  }
}

//////////////////////////////////////////////////////////////////////

Tracer::Tracer(const std::string& path)
    : file_(path),
      file_adapter_(file_),
      writer_(file_adapter_) {

  WHEELS_VERIFY(!file_.fail(), "Failed to open '" << path << "'");

  writer_.SetIndent(' ', 2);
  writer_.StartArray();
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

  writer_.StartObject();

  writer_.Key("event_type");
  writer_.String("message");

  writer_.Key("source_host");
  writer_.String(frame.header.source_host.c_str());

  writer_.Key("dest_host");
  writer_.String(frame.header.dest_host.c_str());

  writer_.Key("send_time");
  writer_.Int(frame.header.send_time);

  writer_.Key("receive_time");
  writer_.Int(receive_time);

  writer_.Key("payload");
  writer_.StartObject();

  // Payload
  if (auto req = TryDeserialize<commute::rpc::proto::Request>(payload)) {
    WriteRequest(*req);
  } else if (auto rsp = TryDeserialize<commute::rpc::proto::Response>(payload)) {
    WriteResponse(*rsp);
  } else {
    // ???
  }

  writer_.EndObject();

  writer_.EndObject();
}

static std::string Describe(commute::rpc::Method method) {
  return method.service + "." + method.name;
}

void Tracer::WriteResponse(commute::rpc::proto::Response rsp) {
  writer_.Key("type");
  writer_.String("commute::rpc::proto::Response");

  writer_.Key("id");
  writer_.Int(rsp.request_id);

  writer_.Key("trace_id");
  writer_.String(rsp.trace_id.c_str());

  writer_.Key("method");
  writer_.String(Describe(rsp.method).c_str());

  writer_.Key("error");
  writer_.Int((int)rsp.error);

  if (rsp.IsOk()) {
    writer_.Key("result");
    WriteMessage(writer_, rsp.result);
  }
}

void Tracer::WriteRequest(commute::rpc::proto::Request req) {
  writer_.Key("type");
  writer_.String("commute::rpc::proto::Request");

  writer_.Key("id");
  writer_.Int(req.id);

  writer_.Key("trace_id");
  writer_.String(req.trace_id.c_str());

  writer_.Key("method");
  writer_.String(Describe(req.method).c_str());

  writer_.Key("arguments");
  WriteMessage(writer_, req.input);
}

void Tracer::Stop() {
  writer_.EndArray();
  file_ << std::endl;
  file_.flush();
}

}  // namespace whirl::matrix
