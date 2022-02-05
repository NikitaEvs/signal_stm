#pragma once

#include <array>
#include <cstdlib>

#include "processor.hpp"

#include "waveaux.h"
#include "wavemin.h"

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

// TODO: Take care of case when U == double (unnecessary copy and buffer)
template <typename U, std::size_t sig_size>
class WaveletTransformerFactory : public TransformerFactory<U, double> {
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
          
          wave_obj = wave_init(ConvertWaveName(wave_name_));
          wt_obj = wt_init(wave_obj, 
                           kMethod_name_, 
                           static_cast<int>(in_size),
                           decomposition_iter_);
          setDWTExtension(wt_obj, ConvertDWTExtension(extension_));
          setWTConv(wt_obj, kCMethod_);

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


} // namespace Utils
} // namespace SD
