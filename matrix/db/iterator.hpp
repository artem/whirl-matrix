#pragma once

#include <whirl/node/db/iterator.hpp>
#include <whirl/node/db/snapshot.hpp>

#include <matrix/db/entries.hpp>
#include <matrix/db/snapshot.hpp>

namespace whirl::matrix::db {

class Iterator : public node::db::IIterator {
 public:
  Iterator(node::db::ISnapshotPtr snapshot, const Entries& entries)
      : snapshot_(std::move(snapshot)),
        entries_(entries) {
    SeekToFirst();
  }

  node::db::KeyView Key() const override;
  node::db::ValueView Value() const override;

  void SeekToFirst() override;
  void SeekToLast() override;
  void Seek(const node::db::Key& target) override;

  bool Valid() const override;

  void Next() override;
  void Prev() override;

 private:
  void EnsureValid() const;

 private:
  node::db::ISnapshotPtr snapshot_;

  const Entries& entries_;

  bool valid_ = true;
  Entries::const_iterator it_;
};

}  // namespace whirl::matrix::db
