#pragma once

namespace SD {

namespace Utils {

namespace detail {

static constexpr char kSign = '-';
static constexpr uint32_t kBase = 10;

template <typename DataT, std::enable_if_t<sizeof(DataT) == 2, bool> = true>
uint16_t cast_to_unsigned(DataT in) {
  return static_cast<uint16_t>(in);
}

template <typename DataT, std::enable_if_t<sizeof(DataT) == 4, bool> = true>
uint32_t cast_to_unsigned(DataT in) {
  return static_cast<uint32_t>(in);
}

template <typename DataT, std::enable_if_t<sizeof(DataT) == 8, bool> = true>
uint64_t cast_to_unsigned(DataT in) {
  return static_cast<uint64_t>(in);
}

template<typename BufferT, typename DataT>
std::size_t unsigned_to_chars_generic(BufferT* start,
                                      const BufferT* end,
                                      DataT data) {
  if (start == end) [[unlikely]] {
    return {};
  }

  static constexpr char kZeroChar = '0';

  if (data == 0) {
    *start = kZeroChar;
    return {};
  }

  DataT pos = 1;
  DataT curr = kBase;
  while (true) {
    if (data < curr) {
      break;
    }

    ++pos;
    curr *= kBase;
  }

  std::size_t final_shift = pos;

  while (data > 0) {
    --pos;
    start[pos] = kZeroChar + (data % kBase);
    data /= kBase;

    if (start == end) [[unlikely]] {
      break;
    }
  }

  return final_shift;
}

template<typename BufferT, typename DataT>
std::size_t signed_to_chars_generic(BufferT* start,
                                    const BufferT* end,
                                    DataT data) {
  if (start == end) [[unlikely]] {
    return {};
  }

  std::size_t shift = 0;

  if (data < 0) {
    ++shift;
    *start++ = kSign;
    data = -data;
  }

  shift += unsigned_to_chars_generic(start, end, cast_to_unsigned(data));

  return shift;
}

} // namespace detail

std::size_t to_chars(char* start, const char* end, uint32_t data);
std::size_t to_chars(char* start, const char* end, int32_t data);
std::size_t to_chars(char* start, const char* end, uint64_t data);
std::size_t to_chars(char* start, const char* end, int64_t data);

std::size_t to_chars(char* start, const char* end, int16_t data) {
  return detail::signed_to_chars_generic(start, end, data);
}

std::size_t to_chars(char* start, const char* end, int32_t data) {
  return detail::signed_to_chars_generic(start, end, data);
}

std::size_t to_chars(char* start, const char* end, int64_t data) {
  return detail::signed_to_chars_generic(start, end, data);
}

std::size_t to_chars(char* start, const char* end, uint16_t data) {
  return detail::unsigned_to_chars_generic(start, end, data);
}

std::size_t to_chars(char* start, const char* end, uint32_t data) {
  return detail::unsigned_to_chars_generic(start, end, data);
}

std::size_t to_chars(char* start, const char* end, uint64_t data) {
  return detail::unsigned_to_chars_generic(start, end, data);
}

} // namespace Utils
} // namespace SD
