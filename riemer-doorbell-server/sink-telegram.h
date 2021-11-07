#pragma once

#include <string>
#include <thread>

#include <tgbot/tgbot.h>

#include "gpiopin.h"
#include "knownchats.h"
#include "sink.h"

class SinkTelegram : Sink {

private:
  TgBot::Bot _bot;
  KnownChats _knownChats;
  const GPIOPin &_pin;

public:
  SinkTelegram(const std::string &botToken, const GPIOPin &pin);

  std::thread start();

  virtual void sendDingDong() override;
};
