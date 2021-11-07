#include <chrono>
#include <iostream>
#include <thread>

#include "gpiopin.h"
#include "sink-telegram.h"

/** Name of environment variable for bot token */
const char *ENV_TELEGRAM_BOT_TOKEN = "TELEGRAM_BOT_TOKEN";

int main() {
  // Nothing helpful can be done without having a connection to Telegram
  const char *TELEGRAM_BOT_TOKEN = std::getenv(ENV_TELEGRAM_BOT_TOKEN);
  if (!TELEGRAM_BOT_TOKEN) {
    throw std::runtime_error(std::string("Environment variable ") +
                             ENV_TELEGRAM_BOT_TOKEN + " is strictly required");
  }

  const GPIOPin pin(2);

  SinkTelegram sinkTelegram(TELEGRAM_BOT_TOKEN, pin);

  // Starting the polling for the GPIO pin. This runs in a separate thread
  // because the telegram server seems to hog the main thread.
  //
  // TODO: Make this threadsafe.
  std::thread threadGPIO([&sinkTelegram, &pin]() {
    std::cout << "Started GPIO Thread" << std::endl;

    bool lastValue = pin.readValue();
    bool currentValue = lastValue;

    while (true) {
      currentValue = pin.readValue();
      if (currentValue != lastValue) {
        std::cout << "Pin " << currentValue << std::endl;
        if (currentValue) {
          std::cout << "Sending DingDong" << std::endl;

          sinkTelegram.sendDingDong();
        }
      }

      // Don't hog up all resources of that poor Raspberry device
      std::this_thread::sleep_for(std::chrono::milliseconds(30));

      lastValue = currentValue;
    }
  });

  // Starting the bot and running the main thread
  try {
    auto threadTelegram = sinkTelegram.start();

    threadTelegram.join();
    threadGPIO.join();
  } catch (TgBot::TgException &e) {
    std::cerr << "Unhandled Telegram Exception: " << e.what() << std::endl;
  }
  return 0;
}
