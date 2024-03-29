#include <matrix/process/trampoline.hpp>

#include <whirl/node/runtime/shortcuts.hpp>

#include <await/fibers/core/fiber.hpp>
#include <await/fibers/static/services.hpp>

namespace whirl::matrix::process {

static void ServiceMain(node::program::Main user_main) {
  user_main();
  // TODO: Stop process
}

void MainTrampoline(node::program::Main main) {
  auto main_fiber = [main]() {
    ServiceMain(main);
  };

  auto* f = await::fibers::CreateFiber(
      main_fiber, node::rt::FiberManager(), node::rt::Executor(),
      await::fibers::BackgroundSupervisor(), await::context::NeverStop());

  f->Schedule();
}

}  // namespace whirl::matrix::process
