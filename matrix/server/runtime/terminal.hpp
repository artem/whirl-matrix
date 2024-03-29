#pragma once

#include <whirl/node/misc/terminal.hpp>

#include <matrix/server/stdout.hpp>

#include <matrix/world/global/log.hpp>

#include <timber/log.hpp>

namespace whirl::matrix {

class Terminal : public node::ITerminal {
 public:
  Terminal(Stdout& stdout)
      : stdout_(stdout), logger_("Terminal", GetLogBackend()) {
  }

  void PrintLine(std::string_view line) override {
    LOG_INFO("Stdout(\"{}\")", line);
    stdout_.PrintLine(line);
  }

 private:
  Stdout& stdout_;
  timber::Logger logger_;
};

}  // namespace whirl::matrix
