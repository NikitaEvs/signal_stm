#pragma once

#include "Hardware/abstract_master.hpp"
#include "Hardware/ioport.hpp"

namespace SD {

namespace Utils {

/**
 * @brief Class for the board's LED
 *
 * This class contains board-specific functions just for the testing purposes
 *
 * @warning There are board-specific constants kLED_Port and kLED_Pin
 */
class LED {
 public:
  /**
   * @brief Initialize board's LED using Master
   * @param master
   */
  LED(Hardware::AbstractMaster& master) :
    port_(master, kLED_Port, kLED_Pin) {
  }

  /**
   * @brief Turn on board's LED
   *
   * @warning In some cases this function can be inverted
   */
  inline void On() {
    port_.Reset();
  }

  /**
   * @brief Turn off board's LED
   *
   * @warning In some cases this function can be inverted
   */
  inline void Off() {
    port_.Set();
  }

 private:
  static constexpr Hardware::GPIO kLED_Port = Hardware::GPIO::C;
  static constexpr Hardware::Pin kLED_Pin = Hardware::Pin::Pin13;

  Hardware::IOPort port_;
};

} // namespace Devices
} // namespace SD
