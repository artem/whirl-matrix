#include <matrix/db/iterator.hpp>

#include <matrix/db/database.hpp>

#include <wheels/support/assert.hpp>

namespace whirl::matrix::db {

Iterator::Iterator(SnapshotRef snapshot)
  : snapshot_(snapshot),
    entries_(snapshot->GetEntries()) {
  SeekToFirst();
}

node::db::KeyView Iterator::Key() const {
  EnsureValid();
  return it_->first;
}

node::db::ValueView Iterator::Value() const {
  EnsureValid();
  return it_->second;
}

void Iterator::SeekToFirst() {
  if (entries_.empty()) {
    valid_ = false;
    return;
  }

  valid_ = true;
  it_ = entries_.begin();
  Move();
}

void Iterator::SeekToLast() {
  if (entries_.empty()) {
    valid_ = false;
    return;
  }

  valid_ = true;
  it_ = std::prev(entries_.end());
  Move();
}

void Iterator::Seek(const node::db::Key& target) {
  it_ = entries_.lower_bound(target);
  if (it_ != entries_.end()) {
    valid_ = true;
    Move();
  } else {
    valid_ = false;
  }
}

bool Iterator::Valid() const {
  return valid_;
}

void Iterator::Next() {
  EnsureValid();
  ++it_;
  if (it_ != entries_.end()) {
    Move();
  } else {
    valid_ = false;
  }
}

void Iterator::Prev() {
  EnsureValid();
  if (it_ != entries_.begin()) {
    --it_;
    Move();
  } else {
    valid_ = false;
  }
}

void Iterator::Move() {
  snapshot_->Db()->IteratorMove();
}

void Iterator::EnsureValid() const {
  WHEELS_VERIFY(valid_, "Invalid iterator");
}

}  // namespace whirl::matrix::db
