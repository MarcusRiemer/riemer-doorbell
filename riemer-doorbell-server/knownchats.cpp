#include "knownchats.h"

#include <algorithm>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

const std::string KnownChats::filename = "known_chats.txt";

KnownChats::KnownChats(const TgBot::Bot &bot) : bot(bot) {
  // Read all known chats from "Database"
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
  for (auto chatId : known) {
    bot.getApi().sendMessage(chatId, msg);
  }
}

void KnownChats::save() {
  // Replace known chats on disk with currently known chats
  // Note to self: Do not lock here, this is a private function
  //               and the public function is responsible for the lock
  std::ofstream fout("known_chats.txt");
  std::copy(known.begin(), known.end(),
            std::ostream_iterator<TelegramChatId>(fout, "\n"));

  std::cout << "Rewritten chats" << std::endl;
}
