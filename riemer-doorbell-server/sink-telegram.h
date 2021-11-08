#pragma once

#include <atomic>
#include <string>
#include <thread>

#include <tgbot/tgbot.h>

#include "knownchats.h"
#include "sink.h"

class SinkTelegram : public Sink {

private:
  TgBot::Bot _bot;
  KnownChats _knownChats;

public:
  SinkTelegram(const std::string &botToken);

  virtual std::optional<std::thread>
  start(const std::atomic<bool> &shutdownRequested) override;

  virtual void sendDingDong() override;
};
