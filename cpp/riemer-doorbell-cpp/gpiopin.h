#ifndef GPIOPIN_H
#define GPIOPIN_H

#include <boost/filesystem.hpp>

/**
 * @brief A single GPIO pin that may be polled.
 */
class GPIOPin {
public:
  GPIOPin(int num);

  /**
   * @brief Reads the current value from the /sys filesystem
   * @return State of the wrapped pin.
   */
  bool readValue() const;

  /**
   * @return GPIO number of the PIN.
   */
  inline int pinNum() const { return num; }

private:
  void ensurePinEnabled();

  boost::filesystem::path fsBaseDir() const;
  boost::filesystem::path fsExportDir() const;
  boost::filesystem::path fsDir() const;
  boost::filesystem::path fsValue() const;

private:
  const int num;
};

#endif // GPIOPIN_H
