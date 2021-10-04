#include <matrix/db/snapshot.hpp>

#include <matrix/db/iterator.hpp>

namespace whirl::matrix::db {

node::db::IIteratorPtr Snapshot::MakeIterator() {
  auto self = shared_from_this();
  return std::make_shared<Iterator>(self, entries_);
}

}  // namespace whirl::matrix::db
