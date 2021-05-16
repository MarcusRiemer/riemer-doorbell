#include "gpiopin.h"

#include <fstream>
#include <iostream>

namespace fs = boost::filesystem;

GPIOPin::GPIOPin(int num) : num(num) {
  std::cout << "Ctor for Pin#" << num << std::endl;
  if (!fs::is_directory(fsDir())) {
    throw std::runtime_error(std::string("Could not find GPIO pin at ") +
                             fsDir().string());
  }
}

bool GPIOPin::readValue() const {
  std::fstream file(fsValue(), std::ios::in);
  int val = -1;
  file >> val;

  return val == 1;
}

fs::path GPIOPin::fsDir() const {
  const fs::path GPIO_FS{"/sys/class/gpio"};
  const std::string folder = std::string("gpio") + std::to_string(this->num);

  return GPIO_FS / folder;
}

fs::path GPIOPin::fsValue() const { return fsDir() / "value"; }
