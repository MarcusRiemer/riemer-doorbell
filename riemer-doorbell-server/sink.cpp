#include "sink.h"

std::optional<std::thread>
Sink::start(const std::atomic<bool> &shutdownRequested) {
  return std::nullopt;
}
