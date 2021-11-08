#include <atomic>
#include <chrono>
#include <iostream>
#include <optional>
#include <thread>
#include <vector>

#include <signal.h>

#include "gpiopin.h"
#include "sink-azure.h"
#include "sink-telegram.h"

/** Name of environment variable for bot token */
const char *ENV_TELEGRAM_BOT_TOKEN = "TELEGRAM_BOT_TOKEN";

/** Name of environment variable for GPIO pin */
const char *ENV_GPIO_PIN = "GPIO_PIN";

/** Name of environment variable for Azure SAS key pin */
const char *ENV_AZURE_SAS_KEY = "AZURE_SAS_KEY";

/** Name of environment variable for debug run */
const char *ENV_DEBUG_EMIT = "DEBUG_EMIT";

/** Communicate shutdown requests to threads */
std::atomic_bool shutdown_requested = false;

// This seems to be harder on ARM than I hoped for, so lets live
// static_assert(std::atomic_bool::is_always_lock_free);

/**
 * Read a value from the environment and convert it to the desired type.
 */
template <typename T>
T parseEnv(const char *varName, std::optional<T> fallback = std::nullopt) {
  const char *value = std::getenv(varName);
  if (!value) {
    if (!fallback.has_value()) {
      throw std::runtime_error(std::string("Environment variable ") + varName +
                               " is strictly required");
    } else {
      return fallback.value();
    }
  }

  std::stringstream conv;
  conv << value;

  T result;
  conv >> result;

  return result;
}

void sigintHandler(int) { shutdown_requested = true; }

int main() {
  signal(SIGINT, sigintHandler);
  signal(SIGTERM, sigintHandler);

  // The sinks that a bell should go out to
  std::vector<std::shared_ptr<Sink>> allSinks;

  // Is there a way to run telegram?
  const std::string TELEGRAM_BOT_TOKEN =
      parseEnv<std::string>(ENV_TELEGRAM_BOT_TOKEN, std::optional(""));

  if (TELEGRAM_BOT_TOKEN != "") {
    allSinks.push_back(std::make_shared<SinkTelegram>(TELEGRAM_BOT_TOKEN));
  }

#ifdef WITH_AZURE_EVENTHUB
  const std::string AZURE_SAS_KEY = parseEnv<std::string>(ENV_AZURE_SAS_KEY);
  if (AZURE_SAS_KEY != "") {
    allSinks.push_back(std::make_shared<SinkAzure>(AZURE_SAS_KEY));
  }
#endif

  bool debugEmit = parseEnv<bool>(ENV_DEBUG_EMIT, std::optional(false));

  // Starting the polling for the GPIO pin. This runs in a separate thread
  // because the telegram server seems to hog the main thread.
  //
  // TODO: Make outputs & sends properly threadsafe.
  if (!debugEmit) {
    int GPIO_PIN = parseEnv<int>(ENV_GPIO_PIN);
    const GPIOPin pin(GPIO_PIN);

    std::thread threadGPIO([&allSinks, &pin]() {
      std::cout << "Started GPIO Thread" << std::endl;

      bool lastValue = pin.readValue();
      bool currentValue = lastValue;

      while (!shutdown_requested) {
        currentValue = pin.readValue();
        if (currentValue != lastValue) {
          std::cout << "Pin " << currentValue << std::endl;
          if (currentValue) {
            std::cout << "Sending DingDong" << std::endl;

            for (auto s : allSinks) {
              s->sendDingDong();
            }
          }
        }

        // Don't hog up all resources of that poor Raspberry device
        std::this_thread::sleep_for(std::chrono::milliseconds(30));

        lastValue = currentValue;
      }

      std::cout << "GPIO Thread ended" << std::endl;
    });

    std::vector<std::thread> threads;

    // Starting threads for all sinks that require them
    for (auto s : allSinks) {
      auto t = s->start(shutdown_requested);
      if (t) {
        // Threads must be owned by the vector, so we move them there
        threads.push_back(std::move(t.value()));
      }
    }

    // Wait for all threads
    for (auto &t : threads) {
      t.join();
    }

    std::cout << "Proper shutdown, goodbye!" << std::endl;
  } else {
    for (auto s : allSinks) {
      s->sendDingDong();
    }
  }

  return 0;
}
