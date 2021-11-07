#pragma once

#include <string>

class SinkAzure {
public:
  SinkAzure(const std::string sasKeyName, const std::string sasKey,
            const std::string namespaceName);
};
