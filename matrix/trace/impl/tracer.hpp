#pragma once

#include <matrix/trace/tracer.hpp>

#include <matrix/network/frame.hpp>

#include <commute/rpc/wire.hpp>

// TODO
#include <matrix/helpers/rapidjson.hpp>
#include <cereal/external/rapidjson/prettywriter.h>

#include <filesystem>

#include <ostream>
#include <fstream>

namespace whirl::matrix {

class Tracer : public ITracer {
 public:
  Tracer(const std::string& path);

  void Deliver(const net::Frame& frame) override;
  void Stop() override;

 private:
  bool IsData(const net::Frame& frame) const;

  // Payload
  void WriteRequest(commute::rpc::proto::Request req);
  void WriteResponse(commute::rpc::proto::Response rsp);

 private:
  std::ofstream file_;
  OStreamAdapter file_adapter_;
  rapidjson::PrettyWriter<OStreamAdapter> writer_;
};

}  // namespace whirl::matrix
