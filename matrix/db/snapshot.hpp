#pragma once

#include <whirl/node/db/snapshot.hpp>

#include <matrix/db/entries.hpp>

namespace whirl::matrix::db {

class Snapshot : public node::db::ISnapshot,
                 public std::enable_shared_from_this<Snapshot> {
 public:
  explicit Snapshot(Entries entries)
      : entries_(std::move(entries)) {
  }

  std::optional<node::db::Value> TryGet(
      const node::db::Key& key) const override;

  node::db::IIteratorPtr MakeIterator() override;

 private:
  const Entries entries_;
};

}  // namespace whirl::matrix::db
