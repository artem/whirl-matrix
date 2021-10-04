#pragma once

#include <whirl/node/db/database.hpp>
#include <whirl/node/db/mutation.hpp>

#include <whirl/node/fs/fs.hpp>

#include <matrix/db/mem_table.hpp>
#include <matrix/db/wal.hpp>

#include <timber/logger.hpp>

#include <await/fibers/sync/mutex.hpp>

namespace whirl::matrix::db {

// Implemented in userspace

class Database : public node::db::IDatabase {
 public:
  Database(node::fs::IFileSystem* fs);

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

  void ReplayWAL(node::fs::Path wal_path);

  bool ReadCacheMiss() const;

 private:
  node::fs::IFileSystem* fs_;

  MemTable mem_table_;
  std::optional<WALWriter> wal_;
  await::fibers::Mutex write_mutex_;

  // Incremented on each (batch) mutation
  size_t version_ = 0;

  mutable timber::Logger logger_;
};

}  // namespace whirl::matrix::db
