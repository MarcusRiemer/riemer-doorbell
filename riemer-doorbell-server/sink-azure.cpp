#ifdef AZURE_SINK

#include "sink-azure.h"

#include <iostream>

#include <proton/connection.hpp>
#include <proton/connection_options.hpp>
#include <proton/container.hpp>
#include <proton/message.hpp>
#include <proton/message_id.hpp>
#include <proton/messaging_handler.hpp>
#include <proton/tracker.hpp>
#include <proton/types.hpp>

// Sending Proton Messabes based on
// https://qpid.apache.org/releases/qpid-proton-0.33.0/proton/cpp/api/simple_send_8cpp-example.html
class ProtonSimpleSend : public proton::messaging_handler {
private:
  const std::string url;
  proton::sender sender;
  int sent;
  int confirmed;
  int total;

public:
  ProtonSimpleSend(const std::string &s, int c)
      : url(s), sent(0), confirmed(0), total(c) {}

  void on_container_start(proton::container &c) override {
    sender = c.open_sender(url);
  }

  void on_connection_open(proton::connection &c) override {
    if (c.reconnected()) {
      sent = confirmed; // Re-send unconfirmed messages after a reconnect
    }
  }

  void on_sendable(proton::sender &s) override {
    while (s.credit() && sent < total) {
      proton::message msg;
      std::map<std::string, int> m;
      m["sequence"] = sent + 1;

      msg.id(sent + 1);
      msg.body(m);

      s.send(msg);
      sent++;
    }
  }

  void on_tracker_accept(proton::tracker &t) override {
    confirmed++;

    if (confirmed == total) {
      std::cout << "all messages confirmed" << std::endl;
      t.connection().close();
    }
  }

  void on_transport_close(proton::transport &) override { sent = confirmed; }
};

SinkAzure::SinkAzure(const std::string sasKeyName, const std::string sasKey,
                     const std::string namespaceName) {
  ProtonSimpleSend send("", 1);
}

#endif
