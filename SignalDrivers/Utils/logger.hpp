#pragma once

#include <type_traits>
#include <cstring>

#include "Hardware/uart.hpp"

namespace SD {

namespace Utils {

// TODO: Move to the another file?
void to_chars(char* start, char* end, uint32_t data);
void to_chars(char* start, char* end, int32_t data);


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

void to_chars(char* start, char* end, int32_t data) {
  static constexpr char kSign = '-';

  if (start == end) [[unlikely]] {
    return;
  }

  if (data < 0) {
    *start++ = '-';
    data = -data;
  }

  to_chars(start, end, static_cast<uint32_t>(data));
}

void to_chars(char* start, char* end, uint32_t data) {
  if (start == end) [[unlikely]] {
    return;
  }

  static constexpr uint32_t kBase = 10;
  static constexpr char kZeroChar = '0';

  if (data == 0) {
    *start = kZeroChar;
    return;
  }

  uint32_t pos = 1;
  uint32_t base = kBase;
  while (true) {
    if (data < base) {
      break;
    }

    ++pos;
    base *= kBase;
  }

  while (data > 0) {
    --pos;
    start[pos] = kZeroChar + (data % kBase);
    data /= kBase;

    if (start == end) [[unlikely]] {
      return;
    }
  }
}

} // namespace Utils
} // namespace SD
