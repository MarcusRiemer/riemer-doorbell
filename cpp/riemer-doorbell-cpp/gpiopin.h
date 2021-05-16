#ifndef GPIOPIN_H
#define GPIOPIN_H

#include <boost/filesystem.hpp>

class GPIOPin {
public:
  GPIOPin(int num);

  bool readValue() const;

  inline bool pinNum() const { return num; }

private:
  boost::filesystem::path fsDir() const;
  boost::filesystem::path fsValue() const;

private:
  const int num;
};

#endif // GPIOPIN_H
