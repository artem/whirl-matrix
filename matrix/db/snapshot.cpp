#include <matrix/db/snapshot.hpp>

#include <matrix/db/iterator.hpp>

namespace whirl::matrix::db {

std::optional<node::db::Value> Snapshot::TryGet(
    const node::db::Key& key) const {
  auto it = entries_.find(key);
  if (it != entries_.end()) {
    return it->second;
  } else {
    return std::nullopt;
  }
}

node::db::IIteratorPtr Snapshot::MakeIterator() {
  auto self = shared_from_this();
  return std::make_shared<Iterator>(self, entries_);
}

}  // namespace whirl::matrix::db
