#pragma once

#include "port.hpp"
#include "abstract_master.hpp"

namespace SD {

namespace Hardware {

/**
 * @brief Wrapper for the base Port abstraction that adds Set/Reset function
 * and an initialization using Master
 */
class IOPort : public Port {
 public:
  /**
   * @brief Simple port initialization with a necessary link to the Master
   * in the purpose of an initialization and Set/Reset functions
   * @param master - link to the main Master object
   * @param port
   * @param pin
   */
  IOPort(AbstractMaster& master, GPIO port, Pin pin) :
      Port(port, pin), master_(master) {
    master_.ConfigureIOPort(port, pin);
  }

  /**
   * @brief Set the port's pin using Master
   */
  inline void Set() {
    master_.SetIOPort(gpio, pin);
  }

  /**
   * @brief Reset the port's pin using Master
   */
  inline void Reset() {
    master_.ResetIOPort(gpio, pin);
  }

 private:
  AbstractMaster& master_;
};

} // namespace Hardware
} // namespace SD
