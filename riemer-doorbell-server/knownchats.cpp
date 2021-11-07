#include "knownchats.h"

#include <algorithm>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

const std::string KnownChats::filename = "known_chats.txt";

KnownChats::KnownChats(const TgBot::Bot &bot) : bot(bot) {
  std::ifstream fin("known_chats.txt");
  TelegramChatId id;
  while (fin >> id) {
    known.insert(id);
  }
}

void KnownChats::add(TelegramChatId chatId) {
  const std::lock_guard<std::mutex> lock(mutex);
  known.insert(chatId);
  save();
}

void KnownChats::remove(TelegramChatId chatId) {
  const std::lock_guard<std::mutex> lock(mutex);
  known.erase(chatId);
  save();
}

void KnownChats::broadcast(const std::string &msg) const {
  const std::lock_guard<std::mutex> lock(mutex);
  for (auto chatId : known) {
    bot.getApi().sendMessage(chatId, msg);
  }
}

void KnownChats::save() {
  const std::lock_guard<std::mutex> lock(mutex);

  std::ofstream fout("known_chats.txt");
  std::copy(known.begin(), known.end(),
            std::ostream_iterator<TelegramChatId>(fout, "\n"));
}
