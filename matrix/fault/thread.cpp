#include <matrix/fault/thread.hpp>

#include <matrix/server/server.hpp>
#include <matrix/world/global/time_model.hpp>

#include <await/fibers/core/api.hpp>
#include <await/fibers/core/await.hpp>
#include <await/fibers/sync/future.hpp>

namespace whirl::matrix {

void ThreadPause() {
  auto& runtime = ThisServer().GetNodeRuntime();

  if (!await::fibers::AmIFiber()) {
    return;
  }

  auto pause = ThisServerTimeModel()->ThreadPause();
  auto after = runtime.TimeService()->After(pause);
  await::fibers::Await(std::move(after)).ExpectOk();
}

}  // namespace whirl::matrix
