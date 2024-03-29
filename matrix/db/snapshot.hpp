#pragma once

#include <whirl/node/db/snapshot.hpp>

#include <matrix/db/entries.hpp>

namespace whirl::matrix::db {

class Database;

class Snapshot : public node::db::ISnapshot,
                 public std::enable_shared_from_this<Snapshot> {
 public:
  Snapshot(Database* db, Entries entries, uint64_t version)
      : db_(db),
        entries_(std::move(entries))
      , version_(version) {
  }

  // ISnapshot

  std::optional<node::db::Value> TryGet(
      const node::db::Key& key) const override;

  node::db::IIteratorPtr MakeIterator() override;

  // Access

  const Entries& GetEntries() const {
    return entries_;
  }

  uint64_t Version() const {
    return version_;
  }

  Database* Db() {
    return db_;
  }

 private:
  Database* db_;
  const Entries entries_;
  uint64_t version_;
};

using SnapshotRef = std::shared_ptr<Snapshot>;

}  // namespace whirl::matrix::db
