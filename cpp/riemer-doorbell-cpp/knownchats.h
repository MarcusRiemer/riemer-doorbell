#ifndef KNOWNCHATS_H
#define KNOWNCHATS_H

#include <cstdint>
#include <set>

class KnownChats {
public:
  typedef std::int64_t TelegramChatId;

private:
  typedef std::set<TelegramChatId> Container;

public:
  typedef Container::const_iterator Iterator;

public:
  KnownChats();

  void add(TelegramChatId chatId);
  void remove(TelegramChatId chatId);

  bool empty() const { return known.empty(); }
  Iterator begin() const { return known.begin(); }
  Iterator end() const { return known.end(); }

private:
  void save();

private:
  Container known;
};

#endif // KNOWNCHATS_H
