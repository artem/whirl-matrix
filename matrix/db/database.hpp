#pragma once

#include <whirl/node/db/database.hpp>
#include <whirl/node/db/mutation.hpp>

#include <persist/fs/fs.hpp>

#include <matrix/db/mem_table.hpp>
#include <matrix/db/wal.hpp>

#include <timber/logger.hpp>

#include <await/fibers/sync/mutex.hpp>

namespace whirl::matrix::db {

// Implemented in userspace

class Database : public node::db::IDatabase {
  friend class Iterator;

 public:
  explicit Database(persist::fs::IFileSystem* fs);

  void Open(const std::string& directory) override;

  // Mutate

  void Put(const node::db::Key& key, const node::db::Value& value) override;
  void Delete(const node::db::Key& key) override;

  void Write(node::db::WriteBatch batch) override;

  // Read

  std::optional<node::db::Value> TryGet(
      const node::db::Key& key) const override;

  node::db::ISnapshotPtr MakeSnapshot() override;

 private:
  void DoWrite(node::db::WriteBatch& batch);
  void ApplyToMemTable(const node::db::WriteBatch& batch);

  // Returns start offset for log writer
  size_t ReplayWAL(persist::fs::Path wal_path);

  void IteratorMove();

  persist::fs::Path LogPath() const {
    return *dir_ / "wal";
  }

  // Emulate read latency

  persist::fs::Path SSTablePath() const {
    return *dir_ / "sstable";
  }

  void AccessSSTable() const;
  void PrepareSSTable();

 private:
  persist::fs::IFileSystem* fs_;

  std::optional<persist::fs::Path> dir_;

  MemTable mem_table_;
  std::optional<WALWriter> wal_;
  await::fibers::Mutex write_mutex_;

  // Incremented on each (batch) mutation
  uint64_t version_ = 0;

  mutable timber::Logger logger_;
};

}  // namespace whirl::matrix::db
