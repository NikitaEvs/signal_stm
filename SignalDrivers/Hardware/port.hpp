#pragma once

#include "hardware_layout.hpp"

namespace SD {

namespace Hardware {

/**
 * @brief Abstraction for a low-level hardware port
 */
class Port {
 public:
  Port(GPIO gpio, Pin pin) : gpio(gpio), pin(pin) {
  }

  GPIO gpio;
  Pin pin;
};

} // namespace Hardware
} // namespace SD
