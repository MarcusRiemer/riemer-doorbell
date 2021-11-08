#pragma once

#include <atomic>
#include <optional>
#include <thread>

class Sink {
public:
  virtual void sendDingDong() = 0;

  virtual std::optional<std::thread>
  start(const std::atomic<bool> &shutdownRequested);
};
