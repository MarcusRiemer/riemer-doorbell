#ifndef KNOWNCHATS_H
#define KNOWNCHATS_H

#include <cstdint>
#include <set>

#include <tgbot/tgbot.h>

/**
 * @brief Stores & restores chats that want the notifications
 */
class KnownChats {
public:
  typedef std::int64_t TelegramChatId;

private:
  typedef std::set<TelegramChatId> Container;

public:
  typedef Container::const_iterator Iterator;

public:
  KnownChats(const TgBot::Bot &bot);

  void add(TelegramChatId chatId);
  void remove(TelegramChatId chatId);

  void broadcast(const std::string &msg) const;

  bool empty() const { return known.empty(); }

private:
  void save();

private:
  Container known;
  const TgBot::Bot &bot;
};

#endif // KNOWNCHATS_H
