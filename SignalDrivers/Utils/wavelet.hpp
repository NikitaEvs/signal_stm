#pragma once

#include <array>
#include <cstdlib>

#include "processor.hpp"

#include "waveaux.h"
#include "wavemin.h"
#include "codec.hpp"

namespace SD {

namespace Utils {

enum class WaveName {
  db1,
  db2,
  db3,
  db4,
  db5,
  db6,
  db7,
  db8,
  db9,
  db10,
};

enum class DWTExtension {
  Per,
  Sym,
};

inline const char* ConvertWaveName(WaveName name) {
  switch (name) {
    case WaveName::db1: return "db1";
    case WaveName::db2: return "db2";
    case WaveName::db3: return "db3";
    case WaveName::db4: return "db4";
    case WaveName::db5: return "db5";
    case WaveName::db6: return "db6";
    case WaveName::db7: return "db7";
    case WaveName::db8: return "db8";
    case WaveName::db9: return "db9";
    case WaveName::db10: return "db10";
    default:
      // TODO: Insert assert
      return {};
  }
}

inline const char* ConvertDWTExtension(DWTExtension extension) {
  switch (extension) {
    case DWTExtension::Per: return "per";
    case DWTExtension::Sym: return "sym";
    default:
      // TODO: Insert assert
      return {};
  }
}

template <typename U, typename V, std::size_t sig_size>
class WaveletTransformerFactory;

// TODO: Take care of case when U == double (unnecessary copy and buffer)
template <typename U, std::size_t sig_size>
class WaveletTransformerFactory<U, double, sig_size> : public TransformerFactory<U, double> {
 public:
  WaveletTransformerFactory(WaveName wave_name,
                            int decomposition_iter,
                            DWTExtension extension,
                            bool doAddOriginalSignal = false) :
    wave_name_(wave_name),
    decomposition_iter_(decomposition_iter),
    extension_(extension),
    doAddOriginalSignal_(doAddOriginalSignal) {

  }

  Transformer<U, double> CreateTransformer() override {
    Transformer<U, double> transformer;

    transformer.SetMainFunction(
        [this](U* in, double* out, std::size_t in_size, std::size_t /*out_size*/){
          wave_object wave_obj;
          wt_object wt_obj;

          // const_cast for the library compatibility
          // TODO: Make PR in the library to fix it
          wave_obj = wave_init(
              const_cast<char*>(ConvertWaveName(wave_name_)));
          wt_obj = wt_init(wave_obj, 
                           const_cast<char*>(kMethod_name_),
                           static_cast<int>(in_size),
                           decomposition_iter_);
          setDWTExtension(wt_obj,
                          const_cast<char*>(ConvertDWTExtension(extension_)));
          setWTConv(wt_obj,
                    const_cast<char*>(kCMethod_));

          for (std::size_t i = 0; i < in_size; ++i) {
            // TODO: Insert check in_size == sig_size
            buffer_[i] = in[i];
          }

          dwt(wt_obj, buffer_.data());

          for (int i = 0; i < wt_obj->outlength; ++i) {
            out[i] = wt_obj->output[i];
          }

          if (doAddOriginalSignal_) {
            for (std::size_t i = 0; i < in_size; ++i) {
              out[wt_obj->outlength + i] = in[i];
            }
          }

          wave_free(wave_obj);
          wt_free(wt_obj);
        });

    return transformer;
  }

 private:
  // TODO: Maybe move it out of the factory
  std::array<double, sig_size> buffer_;
  
  WaveName wave_name_;
  int decomposition_iter_;
  const char* kMethod_name_ = "dwt";
  DWTExtension extension_;
  const char* kCMethod_ = "direct";

  bool doAddOriginalSignal_;
};

template <typename U, std::size_t sig_size>
class WaveletTransformerFactory<U, char, sig_size> : public TransformerFactory<U, char> {
 public:
  WaveletTransformerFactory(WaveName wave_name,
                            int decomposition_iter,
                            DWTExtension extension,
                            int num_channels,
                            bool doAddOriginalSignal = false) :
      wave_name_(wave_name),
      decomposition_iter_(decomposition_iter),
      extension_(extension),
      num_channels_(num_channels),
      doAddOriginalSignal_(doAddOriginalSignal) {

  }

  Transformer<U, char> CreateTransformer() override {
    Transformer<U, char> transformer;

    transformer.SetMainFunction(
        [this](U* in, char* out, std::size_t in_size, std::size_t /*out_size*/){
          auto comp = [](const void* lhs, const void* rhs) {
            auto lhs_value = *reinterpret_cast<const double*>(lhs);
            auto rhs_value = *reinterpret_cast<const double*>(rhs);

            if (lhs_value < rhs_value) {
              return -1;
            } else if (lhs_value > rhs_value) {
              return 1;
            } else {
              return 0;
            }
          };

          // TODO: Insert assert in_size % num_channels_ == 0
          std::size_t data_size_one_channel = in_size / num_channels_;

          for (std::size_t i = 0; i < in_size; i += num_channels_) {
            // TODO: Insert check in_size == sig_size
            for (int j = 0; j < num_channels_; ++j) {
              buffer_[j * data_size_one_channel + i / num_channels_] = in[i + j];
            }
          }

          for (int channel_idx = 0; channel_idx < num_channels_; ++channel_idx) {
            wave_object wave_obj;
            wt_object wt_obj;

            // const_cast for the library compatibility
            // TODO: Make PR in the library to fix it
            wave_obj = wave_init(
                const_cast<char*>(ConvertWaveName(wave_name_)));
            wt_obj = wt_init(wave_obj,
                             const_cast<char*>(kMethod_name_),
                             static_cast<int>(data_size_one_channel),
                             decomposition_iter_);
            setDWTExtension(wt_obj,
                            const_cast<char*>(ConvertDWTExtension(extension_)));
            setWTConv(wt_obj,
                      const_cast<char*>(kCMethod_));

            dwt(wt_obj, buffer_.data() + channel_idx * data_size_one_channel);

            double signal_mse = 0, signal_quantile = 0;
            for (int i = channel_idx * data_size_one_channel; i < (channel_idx + 1) * data_size_one_channel; ++i) {
              auto val = buffer_[i];
              signal_mse += val * val / (decomposition_iter_ * decomposition_iter_);
            }

            qsort(buffer_.data() + channel_idx * data_size_one_channel,
                  data_size_one_channel,
                  sizeof(double),
                  comp);
            auto signal_percentile_idx = static_cast<int>(0.98 * data_size_one_channel);
            signal_quantile = buffer_[channel_idx * data_size_one_channel + signal_percentile_idx];

            size_t buffer_idx = channel_idx * data_size_one_channel;
            size_t output_idx = 0;

            buffer_[buffer_idx] = sqrt(signal_mse);
            buffer_[buffer_idx + decomposition_iter_ + 2] = signal_quantile;
            buffer_idx += 1;

            // i = 0 -> approximation
            for (int j = 0; j < decomposition_iter_ + 1; ++j) {
              auto decomposition_len = wt_obj->length[j];

              buffer_[buffer_idx] = 0;

              qsort(wt_obj->output + output_idx,
                    decomposition_len,
                    sizeof(double),
                    comp);

              for (int idx = 0; idx < decomposition_len; ++idx) {
                auto val = wt_obj->output[output_idx + idx];
                buffer_[buffer_idx] += val * val / (decomposition_len * decomposition_len);
              }

              auto percentile_idx = static_cast<int>(0.98 * decomposition_len);

              buffer_[buffer_idx] = sqrt(buffer_[buffer_idx]);
              // decomposition_iter_ + 1 (approximation) + 1 (signal value)
              buffer_[buffer_idx + decomposition_iter_ + 2] = wt_obj->output[output_idx + percentile_idx];

              output_idx += decomposition_len;
              buffer_idx += 1;
            }

//            EncodeDoubles(buffer_.data(), 2 * (decomposition_iter_ + 1), out);
//            EncodeDoubles(wt_obj->output, wt_obj->outlength, out);

            wave_free(wave_obj);
            wt_free(wt_obj);
          }

          size_t out_copy_data = 0;

          for (int channel_idx = 0; channel_idx < num_channels_; ++channel_idx) {
            for (int j = 0; j < 2 * (decomposition_iter_ + 1); ++j) {
              buffer_[out_copy_data] = buffer_[channel_idx * data_size_one_channel + j];
              ++out_copy_data;
            }
          }

          size_t load_output_size = num_channels_ * 2 * (decomposition_iter_ + 1 + 1);
          EncodeDoubles(buffer_.data(), load_output_size, out);
        });

    return transformer;
  }

 private:
  // TODO: Maybe move it out of the factory
  std::array<double, sig_size> buffer_;

  WaveName wave_name_;
  int decomposition_iter_;
  const char* kMethod_name_ = "dwt";
  DWTExtension extension_;
  const char* kCMethod_ = "direct";

  int num_channels_;
  bool doAddOriginalSignal_;
};


} // namespace Utils
} // namespace SD
