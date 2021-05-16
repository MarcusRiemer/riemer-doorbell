#include "knownchats.h"

#include <algorithm>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

KnownChats::KnownChats() {
  std::ifstream fin("known_chats.txt");
  TelegramChatId id;
  while (fin >> id) {
    known.insert(id);
  }
}

void KnownChats::add(TelegramChatId chatId) {
  known.insert(chatId);
  save();
}

void KnownChats::remove(TelegramChatId chatId) {
  known.erase(chatId);
  save();
}

void KnownChats::save() {
  std::ofstream fout("known_chats.txt");
  std::copy(known.begin(), known.end(),
            std::ostream_iterator<TelegramChatId>(fout, "\n"));
}
