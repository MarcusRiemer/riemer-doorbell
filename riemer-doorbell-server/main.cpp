#include <chrono>
#include <iostream>
#include <thread>

#include <tgbot/tgbot.h>

#include "gpiopin.h"
#include "knownchats.h"

/**
 * Utility to extract a human readable display name from a message.
 *
 * @param message The message with a sender of interest.
 * @return A human readable user identification.
 */
const std::string senderDisplayName(TgBot::Message::Ptr message) {
  if (message && message->from) {
    const auto sender = message->from;
    if (sender->username != "") {
      return sender->username;
    } else if (sender->firstName != "") {
      return sender->firstName;
    } else {
      return std::string("User ID ") + std::to_string(sender->id);
    }
  } else {
    return "Broadcast";
  }
}

/** Name of environment variable for bot token */
const char *ENV_TELEGRAM_BOT_TOKEN = "TELEGRAM_BOT_TOKEN";

int main() {
  // Nothing helpful can be done without having a connection to Telegram
  const char *TELEGRAM_BOT_TOKEN = std::getenv(ENV_TELEGRAM_BOT_TOKEN);
  if (!TELEGRAM_BOT_TOKEN) {
    throw std::runtime_error(std::string("Environment variable ") +
                             ENV_TELEGRAM_BOT_TOKEN + " is strictly required");
  }

  // Setting up the bot
  TgBot::Bot bot(TELEGRAM_BOT_TOKEN);

  KnownChats knownChats(bot);
  const GPIOPin pin(2);

  bot.getEvents().onCommand("start", [&bot, &knownChats](
                                         TgBot::Message::Ptr message) {
    KnownChats::TelegramChatId chatId = message->chat->id;
    std::cout << "User " << senderDisplayName(message) << " started the bot "
              << "for chat " << chatId << std::endl;

    knownChats.add(chatId);

    bot.getApi().sendMessage(message->chat->id,
                             "Türklingelnachrichten aktiviert");
  });

  bot.getEvents().onCommand("stop", [&bot,
                                     &knownChats](TgBot::Message::Ptr message) {
    KnownChats::TelegramChatId chatId = message->chat->id;
    std::cout << "User " << senderDisplayName(message) << " removed the bot "
              << "for chat " << chatId << std::endl;

    knownChats.remove(chatId);

    bot.getApi().sendMessage(message->chat->id,
                             "Türklingelnachrichten deaktiviert");
  });

  bot.getEvents().onCommand("pin", [&bot, &pin](TgBot::Message::Ptr message) {
    std::stringstream msg;
    msg << "GPIO Pin #" << pin.pinNum() << " is " << pin.readValue();
    bot.getApi().sendMessage(message->chat->id, msg.str());
  });

  // Starting the polling for the GPIO pin. This runs in a separate thread
  // because the telegram server seems to hog the main thread.
  //
  // TODO: Make this threadsafe.
  std::thread t([&knownChats, &bot, &pin]() {
    bool lastValue = pin.readValue();
    bool currentValue = lastValue;

    while (true) {
      currentValue = pin.readValue();
      if (currentValue != lastValue) {
        std::cout << "Pin " << currentValue << std::endl;
        if (currentValue) {
          knownChats.broadcast("Ding Dong");
        }
      }

      // Don't hog up all resources of that poor Raspberry device
      std::this_thread::sleep_for(std::chrono::milliseconds(10));

      lastValue = currentValue;
    }
  });

  // Starting the Bot
  try {
    std::cout << "Bot username: " << bot.getApi().getMe()->username
              << std::endl;

    // Inform the users that we are back
    knownChats.broadcast("Türklingelbot meldet sich zum Dienst");

    TgBot::TgLongPoll longPoll(bot);
    while (true) {
      longPoll.start();
    }
  } catch (TgBot::TgException &e) {
    std::cerr << "Unhandled Exception: " << e.what() << std::endl;
  }
  return 0;
}
