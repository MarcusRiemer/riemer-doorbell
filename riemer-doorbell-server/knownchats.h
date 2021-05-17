#ifndef KNOWNCHATS_H
#define KNOWNCHATS_H

#include <cstdint>
#include <mutex>
#include <set>

#include <tgbot/tgbot.h>

/**
 * Stores & restores chats that want the notifications
 */
class KnownChats {
public:
  /**
   * Identifier for a chat according to Telegram.
   */
  typedef std::int64_t TelegramChatId;

public:
  /**
   * Attempts to remember relevant chats according to the
   * given bot instance. Restores previously saved chats.
   *
   * @param bot The bot to use for broadcasts.
   */
  KnownChats(const TgBot::Bot &bot);

  /**
   * Remember the given chat in memory and on disk.
   *
   * @param chatId The chat to remember.
   */
  void add(TelegramChatId chatId);

  /**
   * Remove the given chat from memory and from disk.
   *
   * @param chatId The chat to remove.
   */
  void remove(TelegramChatId chatId);

  /**
   * Tell something to all chats that are known to this instance.
   * @param msg The something to tell.
   */
  void broadcast(const std::string &msg) const;

private:
  /**
   * Persists the container to disk.
   */
  void save();

private:
  typedef std::set<TelegramChatId> Container;

private:
  /**
   * The chat IDs that are known to this.
   */
  Container known;

  /**
   * The bot to use for broadcasts.
   */
  const TgBot::Bot &bot;

  /**
   * Guards against operations that may involve the underlying set.
   */
  mutable std::mutex mutex;

  /**
   * The name of the file to use for disk serialization.
   */
  static const std::string filename;
};

#endif // KNOWNCHATS_H
