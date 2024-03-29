#pragma once

#include <persist/fs/path.hpp>

#include <cstdlib>
#include <string>

namespace whirl::matrix::fault {

struct IFaultyServer {
  virtual ~IFaultyServer() = default;

  virtual const std::string& Name() const = 0;

  virtual bool IsAlive() const = 0;

  // Execution

  virtual void Pause() = 0;
  // After Pause
  virtual void Resume() = 0;

  virtual void Crash() = 0;
  // After Crash
  virtual void Launch() = 0;

  virtual void FastReboot() = 0;

  // Clocks

  virtual void AdjustWallClock() = 0;

  // Files

  virtual persist::fs::FileList ListFiles(std::string_view prefix) = 0;

  virtual void CorruptFile(const persist::fs::Path& path) = 0;
};

}  // namespace whirl::matrix::fault
