#ifndef GPIOPIN_H
#define GPIOPIN_H

#include <boost/filesystem.hpp>

/**
 * @brief A single GPIO pin that may be polled.
 */
class GPIOPin {
public:
  /**
   * Create a GPIO reader for the given number. May throw if the
   * required PIN can't be accessed.
   *
   * @param num The number of the GPIO pin.
   */
  GPIOPin(int num);

  /**
   * Reads the current value from the /sys filesystem
   *
   * @return State of the wrapped pin.
   */
  bool readValue() const;

  /**
   * @return GPIO number of the PIN.
   */
  inline int pinNum() const { return num; }

private:
  /**
   * Checks whether the GPIO "folder" exists and attempts to instruct
   * the kernel to create it. If this fails it throws an exception.
   */
  void ensurePinEnabled();

  boost::filesystem::path fsBaseDir() const;
  boost::filesystem::path fsExportFile() const;
  boost::filesystem::path fsDirectionFile() const;
  boost::filesystem::path fsDir() const;
  boost::filesystem::path fsValue() const;

private:
  const int num;
};

#endif // GPIOPIN_H
