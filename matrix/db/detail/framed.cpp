#include <matrix/db/detail/framed.hpp>

#include <wheels/io/read.hpp>
#include <wheels/io/write.hpp>
#include <wheels/io/limit.hpp>
#include <wheels/io/string.hpp>

#include <wheels/memory/view_of.hpp>

namespace whirl {

//////////////////////////////////////////////////////////////////////

std::optional<std::string> FramedReader::ReadNextFrame() {
  auto header = ReadNextFrameHeader();
  if (!header) {
    return std::nullopt;
  }
  wheels::io::LimitReader frame_reader(reader_, header->size);
  auto frame = wheels::io::ReadAll(&frame_reader).ExpectValue();
  WHEELS_VERIFY(frame.length() == header->size, "Cannot read next frame");
  return frame;
}

std::optional<detail::FrameHeader> FramedReader::ReadNextFrameHeader() {
  detail::FrameHeader header;
  wheels::Result<size_t> bytes_read = wheels::io::Read(reader_, detail::MutViewOf(header));
  if (!bytes_read.IsOk() || *bytes_read == 0) {
    return std::nullopt;
  }
  if (*bytes_read < sizeof(detail::FrameHeader)) {
    WHEELS_PANIC("Broken frame header");
  }
  return header;
}

//////////////////////////////////////////////////////////////////////

void FramedWriter::WriteFrame(const std::string& data) {
  std::string frame;
  wheels::io::StringWriter frame_writer(frame);

  WriteFrameHeader(data, &frame_writer);
  frame_writer.Write(wheels::ViewOf(data)).ExpectOk();

  // One frame = one write
  writer_->Write(wheels::ViewOf(frame)).ExpectOk("Failed to write frame header");
}

void FramedWriter::WriteFrameHeader(
    const std::string& frame,
    wheels::io::IWriter* writer) {
  detail::FrameHeader header{frame.length()};
  writer->Write(detail::MutViewOf(header)).ExpectOk();
}

}  // namespace whirl