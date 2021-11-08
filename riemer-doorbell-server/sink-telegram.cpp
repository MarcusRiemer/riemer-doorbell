#include "sink-telegram.h"

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

SinkTelegram::SinkTelegram(const std::string &botToken)
    : _bot(botToken), _knownChats(_bot) {
  _bot.getEvents().onCommand("start", [this](TgBot::Message::Ptr message) {
    KnownChats::TelegramChatId chatId = message->chat->id;
    std::cout << "User " << senderDisplayName(message) << " started the bot "
              << "for chat " << chatId << std::endl;

    _knownChats.add(chatId);

    _bot.getApi().sendMessage(message->chat->id,
                              "Türklingelnachrichten aktiviert");
  });

  _bot.getEvents().onCommand("stop", [this](TgBot::Message::Ptr message) {
    KnownChats::TelegramChatId chatId = message->chat->id;
    std::cout << "User " << senderDisplayName(message) << " removed the bot "
              << "for chat " << chatId << std::endl;

    _knownChats.remove(chatId);

    _bot.getApi().sendMessage(message->chat->id,
                              "Türklingelnachrichten deaktiviert");
  });

  /*_bot.getEvents().onCommand("pin", [this](TgBot::Message::Ptr message) {
    std::stringstream msg;
    msg << "GPIO Pin #" << _pin.pinNum() << " is " << _pin.readValue();
    _bot.getApi().sendMessage(message->chat->id, msg.str());
  });*/
}

std::optional<std::thread>
SinkTelegram::start(const std::atomic<bool> &shutdownRequested) {
  std::ostringstream tmpOut;
  tmpOut << "Bot username: " << _bot.getApi().getMe()->username;
  std::cout << tmpOut.str() << std::endl;

  // Inform the users that we are back
  _knownChats.broadcast("Türklingelbot meldet sich zum Dienst");

  std::thread t([this, &shutdownRequested]() {
    try {
      std::cout << "Started Telegram Thread" << std::endl;
      TgBot::TgLongPoll longPoll(_bot);

      while (!shutdownRequested) {
        longPoll.start();
      }
    } catch (TgBot::TgException &e) {
      std::cerr << "Unhandled Telegram Exception: " << e.what() << std::endl;
    }
  });

  return t;
}

void SinkTelegram::sendDingDong() { _knownChats.broadcast("Ding Dong"); }
