#include <matrix/db/database.hpp>

#include <matrix/db/snapshot.hpp>

#include <matrix/world/global/random.hpp>
#include <matrix/world/global/log.hpp>
#include <matrix/world/global/time_model.hpp>

#include <matrix/log/bytes.hpp>

#include <wheels/memory/view_of.hpp>

#include <timber/log.hpp>

using whirl::node::db::Key;
using whirl::node::db::Value;
using whirl::node::db::WriteBatch;

namespace whirl::matrix::db {

Database::Database(persist::fs::IFileSystem* fs)
    : fs_(fs),
      logger_("Database", GetLogBackend()) {
}

void Database::Open(const std::string& directory) {
  dir_ = fs_->MakePath(directory);

  auto wal_path = LogPath();
  wal_.emplace(fs_, wal_path);
  wal_->Open(ReplayWAL(wal_path));

  PrepareSSTable();
}

void Database::Put(const Key& key, const Value& value) {
  //LOG_INFO("Put('{}', '{}')", key, log::FormatMessage(value));

  node::db::WriteBatch batch;
  batch.Put(key, value);
  DoWrite(batch);
}

void Database::Delete(const Key& key) {
  //LOG_INFO("Delete('{}')", key);

  node::db::WriteBatch batch;
  batch.Delete(key);
  DoWrite(batch);
}

std::optional<Value> Database::TryGet(const Key& key) const {
  LOG_INFO("TryGet({})", key);

  if (ThisServerTimeModel()->GetCacheMiss()) {
    AccessSSTable();
  }
  return mem_table_.TryGet(key);
}

void Database::IteratorMove() {
  if (ThisServerTimeModel()->IteratorCacheMiss()) {
    AccessSSTable();
  }
}

node::db::ISnapshotPtr Database::MakeSnapshot() {
  LOG_INFO("Make snapshot at version {}", version_);
  return std::make_shared<Snapshot>(this, mem_table_.GetEntries(), version_);
}

void Database::Write(WriteBatch batch) {
  LOG_INFO("Write({} mutations)", batch.muts.size());
  DoWrite(batch);
}

void Database::DoWrite(WriteBatch& batch) {
  auto guard = write_mutex_.Guard();

  wal_->Append(batch);
  ApplyToMemTable(batch);
  ++version_;
}

void Database::ApplyToMemTable(const node::db::WriteBatch& batch) {
  for (const auto& mut : batch.muts) {
    switch (mut.type) {
      case node::db::MutationType::Put:
        LOG_INFO("Put('{}', '{}')", mut.key, log::FormatMessage(*mut.value));
        mem_table_.Put(mut.key, *mut.value);
        break;
      case node::db::MutationType::Delete:
        LOG_INFO("Delete('{}')", mut.key);
        mem_table_.Delete(mut.key);
        break;
    }
  }
}

size_t Database::ReplayWAL(persist::fs::Path wal_path) {
  mem_table_.Clear();

  LOG_INFO("Replaying WAL -> MemTable");

  if (!fs_->Exists(wal_path)) {
    return 0;
  }

  version_ = 0;

  WALReader wal_reader(fs_, wal_path);

  while (auto batch = wal_reader.ReadNext()) {
    ApplyToMemTable(*batch);
    ++version_;
  }

  LOG_INFO("MemTable populated");

  return wal_reader.WriterOffset();
}

// Emulate read latency

void Database::PrepareSSTable() {
  auto sstable_path = SSTablePath();
  if (!fs_->Exists(sstable_path)) {
    fs_->Create(sstable_path).ExpectOk();

    persist::fs::FileWriter writer(fs_, sstable_path);
    writer.Open().ExpectOk();
    writer.Write(wheels::ViewOf("data")).ExpectOk();
  }
}

void Database::AccessSSTable() const {
  LOG_INFO("Cache miss, access SSTable on disk");

  persist::fs::FileReader reader(fs_, SSTablePath());
  reader.Open().ExpectOk();

  char buf[128];
  reader.ReadSome(wheels::MutViewOf(buf)).ExpectOk();
}

}  // namespace whirl::matrix::db
