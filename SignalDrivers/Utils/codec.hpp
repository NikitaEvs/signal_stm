#pragma once

#include <cstdint>

//extern "C" {
//#include "baseencode.h"
//}
#include "base32_rfc4648.hpp"

namespace SD {

namespace Utils {

using base32 = cppcodec::base32_rfc4648;

// Only for base32 method
// TODO: Specify magic numbers
template <typename T>
static constexpr std::size_t GetDestBufSize(std::size_t src_size) {
  auto len = src_size * sizeof(T);
  return ((len) / 5) * 8 + ((len) % 5 ? 8 : 0);
}

void EncodeDoubles(const double* plain,
                    std::size_t len,
                    char* coded) {
  const auto* plain_casted = reinterpret_cast<const char*>(plain);
  std::size_t len_casted = len * sizeof(double);

//  base32_encode(plain_casted, len_casted, coded);
  base32::encode(coded, GetDestBufSize<double>(len), plain_casted, len_casted);
}


} // namespace Util

} // namespace SD