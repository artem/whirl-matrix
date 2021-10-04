#include <whirl/node/db/kv.hpp>
#include <whirl/node/fs/io.hpp>

#include <whirl/node/runtime/shortcuts.hpp>

// Simulation
#include <matrix/facade/world.hpp>
#include <matrix/test/event_log.hpp>

#include <wheels/memory/view_of.hpp>
#include <wheels/io/read.hpp>

#include <chrono>

using namespace whirl;

//////////////////////////////////////////////////////////////////////

void PrintSnapshot(std::string_view title, node::db::ISnapshotPtr snapshot) {
  node::rt::PrintLine("Iterate over snapshot {}", title);

  auto it = snapshot->MakeIterator();

  while (it->Valid()) {
    node::rt::PrintLine("{} -> {}", it->Key(), it->Value());
    it->Next();
  }
}

void TestNode() {
  node::rt::Database()->Open(
      node::rt::Config()->GetString("db.path"));

  node::rt::Database()->Put("X", "v1");
  node::rt::Database()->Put("Y", "v1");
  node::rt::Database()->Put("Z", "v1");

  auto snapshot_1 = node::rt::Database()->MakeSnapshot();

  node::rt::Database()->Put("Y", "v2");

  auto snapshot_2 = node::rt::Database()->MakeSnapshot();

  node::rt::Database()->Put("X", "v3");
  node::rt::Database()->Put("Z", "v3");

  // Print snapshots

  PrintSnapshot("1", snapshot_1);
  PrintSnapshot("2", snapshot_2);

  // Iteration

  {
    node::rt::PrintLine("Reverse iteration over snapshot 2");

    auto it = snapshot_2->MakeIterator();

    it->SeekToLast();
    while (it->Valid()) {
      node::rt::PrintLine("{} -> {}", it->Key(), it->Value());
      it->Prev();
    }
  }

  {
    node::rt::PrintLine("Seek(Y) in snapshot 2");

    auto it = snapshot_2->MakeIterator();
    it->Seek("Y");

    while (it->Valid()) {
      node::rt::PrintLine("{} -> {}", it->Key(), it->Value());
      it->Next();
    }
  }
}

//////////////////////////////////////////////////////////////////////

int main() {
  static const size_t kSeed = 17;

  matrix::facade::World world{kSeed};

  static const std::string kHostName = "Mars";

  world.AddServer(kHostName, TestNode);

  world.Start();
  world.MakeSteps(100);

  auto std_out = world.GetStdout(kHostName);

  size_t digest = world.Stop();

  std::cout << "Seed: " << kSeed << ", digest: " << digest
            << ", time: " << world.TimeElapsed()
            << ", steps: " << world.StepCount() << std::endl;

  std::cout << "Simulation log:" << std::endl;
  matrix::WriteTextLog(world.EventLog(), std::cout);

  std::cout << "Stdout of '" << kHostName << "':" << std::endl;
  for (const auto& line : std_out) {
    std::cout << '\t' << line << std::endl;
  }

  std::cout << std::endl;

  return 0;
}
