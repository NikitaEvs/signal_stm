#pragma once

#include "processor.hpp"

#include "arm_math.h"

namespace SD {

namespace Utils {

template <typename U, typename V>
class RealFFTFactory : public TransformerFactory<U, V> {
 public:
  explicit RealFFTFactory(bool isForwardFFT) : isForwardFFT_(isForwardFFT) {
  }

  Transformer<U, V> CreateTransformer() override {
    // TODO: WIP, rewrite this function
    static_assert(sizeof(U) == 16);
    static_assert(sizeof(V) == 16);

    Transformer<U, V> transformer;

    transformer.SetMainFunction(
        [this](U* in,
                          V* out,
                          std::size_t in_size,
                          std::size_t /*out_size*/){
          uint32_t direction_flag = isForwardFFT_? 0 : 1;
          const uint32_t bit_order = 1;

          auto in_ptr = reinterpret_cast<q15_t*>(in);
          auto out_ptr = reinterpret_cast<q15_t*>(out);

          arm_rfft_instance_q15 fft_instance;
          arm_rfft_init_q15(&fft_instance, in_size, direction_flag, bit_order);
          arm_rfft_q15(&fft_instance, in_ptr, out_ptr);
          arm_abs_q15(out_ptr, out_ptr, in_size);
    });

    return transformer;
  }

 private:
  bool isForwardFFT_;
};

} // namespace Utils
} // namespace SD
