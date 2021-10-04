#pragma once

#include <matrix/db/entries.hpp>

namespace whirl::matrix::db {

// Sorted in-memory string -> string mapping

class MemTable {
 public:
  MemTable() = default;

  void Put(node::db::Key key, node::db::Value value) {
    entries_.insert_or_assign(key, value);
  }

  std::optional<node::db::Value> TryGet(node::db::Key key) const {
    auto it = entries_.find(key);
    if (it != entries_.end()) {
      return it->second;
    } else {
      return std::nullopt;
    }
  }

  void Delete(node::db::Key key) {
    entries_.erase(key);
  }

  void Clear() {
    entries_.clear();
  }

  const Entries& GetEntries() const {
    return entries_;
  }

 private:
  Entries entries_;
};

}  // namespace whirl::matrix::db
