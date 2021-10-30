#include <matrix/db/wal.hpp>

using whirl::node::db::WriteBatch;

namespace whirl::matrix::db {

std::optional<WriteBatch> WALReader::ReadNext() {
  auto record = log_reader_.ReadNext();
  if (!record.has_value()) {
    return std::nullopt;
  }
  auto entry = muesli::Deserialize<WALEntry>(*record);
  return node::db::WriteBatch{entry.muts};
}

}  // namespace whirl::matrix::db
