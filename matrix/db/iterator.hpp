#pragma once

#include <whirl/node/db/iterator.hpp>
#include <whirl/node/db/snapshot.hpp>

#include <matrix/db/entries.hpp>
#include <matrix/db/snapshot.hpp>

#include <wheels/support/assert.hpp>

namespace whirl::matrix::db {

class Iterator : public node::db::IIterator {
 public:
  Iterator(node::db::ISnapshotPtr snapshot, const Entries& entries)
      : snapshot_(std::move(snapshot)),
        entries_(entries) {
    SeekToFirst();
  }

  node::db::KeyView Key() const override {
    CheckValid();
    return it_->first;
  }

  node::db::ValueView Value() const override {
    CheckValid();
    return it_->second;
  }

  void SeekToFirst() override {
    if (entries_.empty()) {
      valid_ = false;
      return;
    }

    valid_ = true;
    it_ = entries_.begin();
  }

  void SeekToLast() override {
    if (entries_.empty()) {
      valid_ = false;
      return;
    }

    valid_ = true;
    it_ = std::prev(entries_.end());
  }

  void Seek(const node::db::Key& target) override {
    it_ = entries_.lower_bound(target);
    if (it_ != entries_.end()) {
      valid_ = true;
    } else {
      valid_ = false;
    }
  }

  bool Valid() const override {
    return valid_;
  }

  void Next() override {
    CheckValid();
    ++it_;
    if (it_ == entries_.end()) {
      valid_ = false;
    }
  }

  void Prev() override {
    CheckValid();
    if (it_ == entries_.begin()) {
      valid_ = false;
    } else {
      --it_;
    }
  }

 private:
  void CheckValid() const {
    WHEELS_VERIFY(valid_, "Invalid iterator");
  }

 private:
  node::db::ISnapshotPtr snapshot_;

  const Entries& entries_;

  bool valid_ = true;
  Entries::const_iterator it_;
};

}  // namespace whirl::matrix::db
