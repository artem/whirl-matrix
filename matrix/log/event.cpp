#include <matrix/log/event.hpp>

#include <matrix/world/global/global.hpp>

#include <commute/rpc/trace.hpp>

#include <await/fibers/core/api.hpp>

#include <wheels/support/string_builder.hpp>

namespace whirl::matrix::log {

//////////////////////////////////////////////////////////////////////

// Current context

static std::string ThisFiberServiceName() {
  return wheels::StringBuilder() << 'T' << await::fibers::self::GetId();
}

static std::string ThisFiberName() {
  auto name = await::fibers::self::GetName();
  if (name.has_value()) {
    return *name;
  } else {
    return ThisFiberServiceName();
  }
}

static std::string ThisActorName() {
  if (AmIActor()) {
    return ThisActor()->Name();
  } else {
    return "World";
  }
}

static std::string DescribeThisActor() {
  wheels::StringBuilder descr;

  descr << ThisActorName();
  if (await::fibers::AmIFiber()) {
    descr << " /" << ThisFiberName();
  }

  return descr;
}

//////////////////////////////////////////////////////////////////////

Event CaptureMatrixContext(const timber::Event& e) {
  Event event;

  event.time = GlobalNow();
  event.step = WorldStepNumber();
  event.level = e.level;
  event.actor = DescribeThisActor();
  event.component = e.component;
  event.trace_id = commute::rpc::TryGetCurrentTraceId();
  event.message = e.message;

  return event;
}

}  // namespace whirl::matrix::log
