#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include "gpiopin.h"
#include "sink-azure.h"
#include "sink-telegram.h"

/** Name of environment variable for bot token */
const char *ENV_TELEGRAM_BOT_TOKEN = "TELEGRAM_BOT_TOKEN";

/** Name of environment variable for GPIO pin */
const char *ENV_GPIO_PIN = "GPIO_PIN";

/** Name of environment variable for Azure SAS key pin */
const char *ENV_AZURE_SAS_KEY = "AZURE_SAS_KEY";

int main() {
  // Nothing helpful can be done without having a connection to Telegram
  const char *TELEGRAM_BOT_TOKEN = std::getenv(ENV_TELEGRAM_BOT_TOKEN);
  if (!TELEGRAM_BOT_TOKEN) {
    throw std::runtime_error(std::string("Environment variable ") +
                             ENV_TELEGRAM_BOT_TOKEN + " is strictly required");
  }

  const char *GPIO_PIN = std::getenv(ENV_GPIO_PIN);
  if (!GPIO_PIN) {
    throw std::runtime_error(std::string("Environment variable ") +
                             ENV_GPIO_PIN + " is strictly required");
  }

  const GPIOPin pin(std::stoi(GPIO_PIN));

  SinkTelegram sinkTelegram(TELEGRAM_BOT_TOKEN, pin);

  std::vector<Sink *> allSinks;
  allSinks.push_back(&sinkTelegram);

#ifdef WITH_AZURE_EVENTHUB
  const char *AZURE_SAS_KEY = std::getenv(ENV_AZURE_SAS_KEY);

  if (!AZURE_SAS_KEY) {
    throw std::runtime_error(std::string("Environment variable ") +
                             ENV_AZURE_SAS_KEY + " is strictly required");
  }

  SinkAzure sinkAzure(AZURE_SAS_KEY);
  allSinks.push_back(&sinkAzure);

#endif

  // Starting the polling for the GPIO pin. This runs in a separate thread
  // because the telegram server seems to hog the main thread.
  //
  // TODO: Make outputs & sends properly threadsafe.
  std::thread threadGPIO([&allSinks, &pin]() {
    std::cout << "Started GPIO Thread" << std::endl;

    bool lastValue = pin.readValue();
    bool currentValue = lastValue;

    while (true) {
      currentValue = pin.readValue();
      if (currentValue != lastValue) {
        std::cout << "Pin " << currentValue << std::endl;
        if (currentValue) {
          std::cout << "Sending DingDong" << std::endl;

          for (Sink *s : allSinks) {
            s->sendDingDong();
          }
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
