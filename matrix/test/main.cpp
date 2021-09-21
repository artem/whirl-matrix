#include <matrix/test/main.hpp>

#include <matrix/log/file.hpp>
#include <matrix/new/new.hpp>

#include <wheels/cmdline/argparse.hpp>

#include <iostream>

namespace whirl::matrix {

//////////////////////////////////////////////////////////////////////

static void CLI(wheels::ArgumentParser& parser) {
  parser.AddHelpFlag();

  parser.Add("det").Flag().Help("Test determinism");
  parser.Add("sims").ValueDescr("uint").Help("Number of simulations to run");
  parser.Add("seed").ValueDescr("uint").Optional();
  parser.Add("log").ValueDescr("path").Optional();
  parser.Add("verbose").Flag().Help("Print debug message");
}

template <typename T>
static T FromString(std::string str) {
  std::istringstream input{str};
  T value;
  input >> value;
  return value;
}

//////////////////////////////////////////////////////////////////////

int Main(int argc, const char** argv, Simulation sim) {
  // For simulator debugging
  // ActivateAllocsTracker();

  wheels::ArgumentParser parser{"Whirl simulator"};
  CLI(parser);

  wheels::ParsedArgs args = parser.Parse(argc, argv);

  TestRunner runner{sim};

  // Initialize
  log::GetLogFile();

  if (args.Has("log")) {
    runner.WriteLogTo(args.Get("log"));
  }

  if (args.HasFlag("verbose")) {
    runner.BeVerbose();
  }

  if (args.Has("seed")) {
    size_t seed = FromString<size_t>(args.Get("seed"));
    runner.RunSingleSimulation(seed);
    return 0;
  }

  if (args.HasFlag("det")) {
    runner.TestDeterminism();
  }

  size_t sims_count = FromString<size_t>(args.Get("sims"));
  runner.RunSimulations(sims_count);

  runner.Congratulate();

  return 0;
}

}  // namespace whirl::matrix
