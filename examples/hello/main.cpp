// Node
#include <whirl/node/runtime/shortcuts.hpp>

// Simulation
#include <matrix/facade/world.hpp>
#include <matrix/test/event_log.hpp>

using namespace whirl;

//////////////////////////////////////////////////////////////////////

void HelloWorld() {
  node::rt::PrintLine("Hello, World!");
  node::rt::PrintLine("I am '{}' from pool '{}'", node::rt::HostName(),
                      node::rt::Config()->GetString("pool"));

  node::rt::PrintLine("net.rtt: {}", node::rt::Config()->GetInt64("net.rtt"));
}

//////////////////////////////////////////////////////////////////////

int main() {
  static const size_t kSeed = 17;

  matrix::facade::World world{kSeed};

  static const std::string kHostName = "Julie";

  world.AddServer(kHostName, HelloWorld);

  world.Start();
  world.MakeSteps(100500);

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
