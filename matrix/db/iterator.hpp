#pragma once

#include <whirl/node/db/iterator.hpp>
#include <whirl/node/db/snapshot.hpp>

#include <matrix/db/entries.hpp>
#include <matrix/db/snapshot.hpp>

namespace whirl::matrix::db {

class Iterator : public node::db::IIterator {
 public:
  explicit Iterator(SnapshotRef snapshot);

  // IIterator

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

  // Access new key
  void Move();

 private:
  SnapshotRef snapshot_;

  const Entries& entries_;

  bool valid_ = false;
  Entries::const_iterator it_;
};

}  // namespace whirl::matrix::db
