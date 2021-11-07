#ifdef WITH_AZURE_EVENTHUB

#pragma once

#include <memory>
#include <string>

#include "sink.h"

class SinkAzure : public Sink {
public:
  SinkAzure(const std::string sasKey);

  virtual void sendDingDong() override;
};

#endif
