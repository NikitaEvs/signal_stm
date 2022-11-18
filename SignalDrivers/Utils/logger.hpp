#pragma once

#include <type_traits>
#include <cstring>

#include "Hardware/uart.hpp"

#include "Utils/converters.hpp"

namespace SD {

namespace Utils {

class Logger : public Hardware::BlockingUART {
 public:
  explicit Logger(Hardware::AbstractMaster& master)
      : Hardware::BlockingUART(master, Hardware::UART::UART1) {
  }

  void Log(const char* msg) {
    SendMsg(msg);
    SendMsg(kDelimiter);
  }

  void Log(uint32_t data) {
    static constexpr std::size_t kBufferSize = 100;
    char buffer[kBufferSize] = {};

    to_chars(buffer, buffer + kBufferSize, data);
    SendMsg(buffer);
    SendMsg(kDelimiter);
  }

 private:
  static const constexpr char* kDelimiter = "\r\n";

  template<typename CharT,
      std::enable_if_t<sizeof(CharT) == sizeof(uint8_t), bool> = true>
  void SendMsg(const CharT* msg) {
    const auto* msg_cast_ptr = reinterpret_cast<const uint8_t*>(msg);
    const std::size_t msg_size = strlen(msg); // TODO: Fix problem with different types

    SendBytes(msg_cast_ptr, msg_size);
  }
};

} // namespace Utils
} // namespace SD
