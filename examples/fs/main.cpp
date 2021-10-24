#include <persist/fs/io/file_reader.hpp>
#include <persist/fs/io/file_writer.hpp>

#include <whirl/node/db/kv.hpp>

#include <whirl/node/runtime/shortcuts.hpp>

// Simulation
#include <matrix/facade/world.hpp>
#include <matrix/test/event_log.hpp>

#include <wheels/memory/view_of.hpp>
#include <wheels/io/read.hpp>

#include <chrono>

using namespace whirl;

//////////////////////////////////////////////////////////////////////

void TestNode() {
  node::rt::Database()->Open(
      node::rt::Config()->GetString("db.path"));

  auto flag_fpath = node::rt::MakeFsPath("/flag");
  auto test_fpath = node::rt::MakeFsPath("/file");

  if (!node::rt::FileSystem()->Exists(flag_fpath)) {

    persist::fs::FileWriter file_writer(node::rt::FileSystem(), test_fpath);

    file_writer.Open().ExpectOk();
    file_writer.Write(wheels::ViewOf("Hello, World!")).ExpectOk();

    node::rt::Database()->Put("Test-Put", "Ok!");
    node::rt::Database()->Put("Test-Delete", "...");
    node::rt::Database()->Delete("Test-Delete");

    auto chunks = node::rt::FsRootPath() / "chunks";

    node::rt::FileSystem()->Create(chunks / "1").ExpectOk();
    node::rt::FileSystem()->Create(chunks / "2").ExpectOk();
    node::rt::FileSystem()->Create(chunks / "3").ExpectOk();

    node::rt::FileSystem()->Create(flag_fpath).ExpectOk();
  }

  // Check database

  auto v1 = node::rt::Database()->TryGet("Test-Put");
  if (v1) {
    node::rt::PrintLine("Test-Put -> {}", *v1);
  }
  auto v2 = node::rt::Database()->TryGet("Test-Delete");
  if (!v2) {
    node::rt::PrintLine("Test-Delete key not found in database");
  }

  // Check file

  {
    persist::fs::FileReader file_reader(node::rt::FileSystem(), test_fpath);
    file_reader.Open().ExpectOk();
    auto content = wheels::io::ReadAll(&file_reader).ExpectValueOr("Failed to read from file");
    node::rt::PrintLine("Content of '{}': <{}>", "/file", content);
  }

  // List directory

  {
    node::rt::PrintLine("List chunks:");
    auto chunks = node::rt::FileSystem()->ListFiles("/chunks/");
    for (const auto& fpath : chunks) {
      node::rt::PrintLine("\tChunk: {}", fpath);
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
  world.RestartServer(kHostName);
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
