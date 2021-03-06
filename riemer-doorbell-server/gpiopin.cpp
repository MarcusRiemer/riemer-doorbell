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
  // Setting up GPIO via the filesystem followed these (german) instructions
  // http://raspberrypiguide.de/howtos/raspberry-pi-gpio-how-to/

  if (!fs::is_directory(fsDir())) {
    // Activate the GPIO pin without resorting to the shell
    // echo 2 > /sys/class/gpio/export
    std::ofstream exportFile(fsExportFile());
    exportFile << num;
    exportFile.close();

    if (!fs::is_directory(fsDir())) {
      throw std::runtime_error(std::string("Could not find GPIO pin at ") +
                               fsDir().string());
    }
  }

  // echo "out" > /sys/class/gpio/gpio17/direction
  std::fstream directionFile(fsDirectionFile(), std::ios::out | std::ios::in);
  directionFile << "out";
  directionFile.close();
}

boost::filesystem::path GPIOPin::fsBaseDir() const {
  return fs::path{"/sys/class/gpio"};
}

boost::filesystem::path GPIOPin::fsExportFile() const {
  return fsBaseDir() / "export";
}

fs::path GPIOPin::fsDir() const {
  const std::string folder = std::string("gpio") + std::to_string(this->num);

  return fsBaseDir() / folder;
}

boost::filesystem::path GPIOPin::fsDirectionFile() const {
  return fsDir() / "direction";
}

fs::path GPIOPin::fsValue() const { return fsDir() / "value"; }
