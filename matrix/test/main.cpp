#include <matrix/test/main.hpp>

#include <wheels/cmdline/argparse.hpp>

#include <iostream>

namespace whirl::matrix {

//////////////////////////////////////////////////////////////////////

static void CLI(wheels::ArgumentParser& parser) {
  parser.AddHelpFlag();

  parser.Add("det").Flag().Help("Test determinism");
  parser.Add("sims").ValueDescr("uint").Optional().Help("Number of simulations to run");
  parser.Add("seed").ValueDescr("uint").Optional();
  parser.Add("log").ValueDescr("path").Optional();
  parser.Add("quiet").Flag().Help("Be quiet");
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
  wheels::ArgumentParser parser{"Whirl simulator"};
  CLI(parser);

  wheels::ParsedArgs args = parser.Parse(argc, argv);

  TestRunner runner{sim};

  if (args.Has("log")) {
    runner.WriteLogTo(args.Get("log"));
  }

  if (args.HasFlag("quiet")) {
    runner.BeQuiet();
  }

  if (args.Has("seed")) {
    size_t seed = FromString<size_t>(args.Get("seed"));
    runner.RunSingleSimulation(seed);
    return 0;
  }

  if (args.HasFlag("det")) {
    runner.TestDeterminism();
  }

  if (args.Has("sims")) {
    size_t count = FromString<size_t>(args.Get("sims"));
    runner.RunSimulations(count);
  }

  runner.Congratulate();

  return 0;
}

}  // namespace whirl::matrix
