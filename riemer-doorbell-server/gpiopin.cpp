#include "gpiopin.h"

#include <fstream>
#include <iostream>

namespace fs = boost::filesystem;

GPIOPin::GPIOPin(int num) : num(num) {
  std::cout << "Ctor for Pin#" << num << std::endl;
  ensurePinEnabled();
}

bool GPIOPin::readValue() const {
  std::fstream file(fsValue(), std::ios::in);
  int val = -1;
  file >> val;

  return val == 1;
}

void GPIOPin::ensurePinEnabled() {
  if (!fs::is_directory(fsDir())) {
    // Activate the GPIO pin without resorting to the shell
    // echo 2 > /sys/class/gpio/export
    std::ofstream file(fsExportDir(), std::ios::out);
    file << num;
    file.close();

    if (!fs::is_directory(fsDir())) {
      throw std::runtime_error(std::string("Could not find GPIO pin at ") +
                               fsDir().string());
    }
  }
}

boost::filesystem::path GPIOPin::fsBaseDir() const {
  return fs::path{"/sys/class/gpio"};
}

boost::filesystem::path GPIOPin::fsExportDir() const {
  return fsBaseDir() / "export";
}

fs::path GPIOPin::fsDir() const {
  const std::string folder = std::string("gpio") + std::to_string(this->num);

  return fsBaseDir() / folder;
}

fs::path GPIOPin::fsValue() const { return fsDir() / "value"; }
