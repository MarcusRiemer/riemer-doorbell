#ifdef WITH_AZURE_EVENTHUB

#include "sink-azure.h"

#include <iostream>
#include <sstream>

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
class ProtonSimpleSender : public proton::messaging_handler {
private:
  const std::string url;
  proton::sender sender;

public:
  ProtonSimpleSender(const std::string &s, int c) : url(s) {}

  void on_container_start(proton::container &c) override {
    sender = c.open_sender(url);
  }

  void on_sendable(proton::sender &s) override {}

  void on_tracker_accept(proton::tracker &t) override {}

  void sendDingDong() {
    proton::message msg;
    std::map<std::string, int> m;
    m["test"] = 1;

    msg.id();
    msg.body(m);

    sender.send(msg);
  }
};

SinkAzure::SinkAzure(const std::string sasKey) {
  // Endpoint=sb://riemer-doorbell-events.servicebus.windows.net/;SharedAccessKeyName=SendKey;SharedAccessKey=R8JH+isl1FQ8UlP0jLLxlsi2R9T1V+EPIEvkAYzRRec=

  std::string sasKeyName("SendKey");
  std::string namespaceName("riemer-doorbell-events");
  std::string eventHubName("riemer-doorbell");

  std::ostringstream urlStream;
  urlStream << "amqps://" << sasKeyName << ":" << sasKey << "@" << namespaceName
            << ".servicebus.windows.net/" << eventHubName;

  std::string urlString = urlStream.str();
  std::cout << "Azure EventHub URL: " << urlString << std::endl;

  ProtonSimpleSender send(urlString, 1);
}

void SinkAzure::sendDingDong() {}

#endif
