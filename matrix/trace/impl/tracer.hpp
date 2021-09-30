#pragma once

#include <matrix/trace/tracer.hpp>

#include <matrix/network/frame.hpp>

#include <matrix/helpers/json_writer.hpp>

#include <commute/rpc/wire.hpp>

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
  JsonWriter writer_;
};

}  // namespace whirl::matrix
