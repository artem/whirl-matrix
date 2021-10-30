#pragma once

#include <whirl/node/db/write_batch.hpp>

#include <persist/fs/fs.hpp>
#include <persist/log/reader.hpp>
#include <persist/log/writer.hpp>

#include <muesli/serialize.hpp>
#include <muesli/serializable.hpp>

#include <wheels/io/buffered.hpp>

#include <cereal/types/vector.hpp>

namespace whirl::matrix::db {

// Write ahead log (WAL) writer / reader

//////////////////////////////////////////////////////////////////////

struct WALEntry {
  std::vector<node::db::Mutation> muts;

  MUESLI_SERIALIZABLE(muts);
};

//////////////////////////////////////////////////////////////////////

class WALWriter {
 public:
  WALWriter(persist::fs::IFileSystem* fs, persist::fs::Path file_path)
      : log_writer_(fs, file_path) {
  }

  void Open(size_t offset) {
    log_writer_.Open(offset).ExpectOk();
  }

  void Append(node::db::WriteBatch batch) {
    AppendImpl({batch.muts});
  }

 private:
  // Atomic
  void AppendImpl(WALEntry entry) {
    auto record = muesli::Serialize(entry);
    log_writer_.Append(wheels::ViewOf(record)).ExpectOk();
  }

 private:
  persist::log::LogWriter log_writer_;
};

//////////////////////////////////////////////////////////////////////

class WALReader {
 public:
  WALReader(persist::fs::IFileSystem* fs, persist::fs::Path file_path)
      : log_reader_(fs, file_path) {
  }

  std::optional<node::db::WriteBatch> ReadNext();

  size_t WriterOffset() const {
    return log_reader_.Offset();
  }

 private:
  persist::log::LogReader log_reader_;
};

}  // namespace whirl::matrix::db
