#include <iostream>
#include <thread>

#include <tgbot/tgbot.h>

#include "gpiopin.h"
#include "knownchats.h"

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

const char *ENV_TELEGRAM_BOT_TOKEN = "TELEGRAM_BOT_TOKEN";

int main() {
  const char *TELEGRAM_BOT_TOKEN = std::getenv(ENV_TELEGRAM_BOT_TOKEN);
  if (!TELEGRAM_BOT_TOKEN) {
    throw std::runtime_error(std::string("Environment variable ") +
                             ENV_TELEGRAM_BOT_TOKEN + " is strictly required");
  }
  KnownChats knownChats;
  const GPIOPin pin(2);

  // Setting up the bot
  TgBot::Bot bot(TELEGRAM_BOT_TOKEN);
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

  std::thread t([&knownChats, &bot, &pin]() {
    bool lastValue = pin.readValue();
    bool currentValue = lastValue;

    while (true) {
      currentValue = pin.readValue();
      if (currentValue != lastValue) {
        std::cout << "Pin " << currentValue << std::endl;
        if (!knownChats.empty() && currentValue) {
          for (auto chatId : knownChats) {
            bot.getApi().sendMessage(chatId, "Ding Dong");
          }
        }
      }

      lastValue = currentValue;
    }
  });

  // Starting the Bot
  try {
    std::cout << "Bot username: " << bot.getApi().getMe()->username
              << std::endl;

    TgBot::TgLongPoll longPoll(bot);
    while (true) {
      std::cout << "Long poll started" << std::endl;
      longPoll.start();
    }
  } catch (TgBot::TgException &e) {
    std::cout << "Unhandled Exception: " << e.what() << std::endl;
  }
  return 0;
}
