#pragma once

#include <array>
#include "processor.hpp"

namespace SD {

namespace Utils {

template <typename T>
class DataProcessorView {
 public:
  using Transformer = std::function<void(T*, std::size_t)>;

  DataProcessorView() : data_ptr_(0), size_(0), has_delimiter_(false) {
  }

  DataProcessorView(uint32_t data_ptr,
                    std::size_t variable_size, std::size_t size,
                    bool has_delimiter) :
    data_ptr_(data_ptr),
    variable_size_(variable_size), size_(size),
    has_delimiter_(has_delimiter) {
  }

  ~DataProcessorView() = default;

  DataProcessorView(const DataProcessorView&) = delete;
  DataProcessorView& operator=(const DataProcessorView&) = delete;

  DataProcessorView(DataProcessorView&&) noexcept = default;
  DataProcessorView& operator=(DataProcessorView&&) noexcept = default;

  static DataProcessorView Invalid() {
    return DataProcessorView();
  }

  [[nodiscard]] bool IsValid() const {
    return data_ptr_ != 0;
  }

  void SetProcessor(Transformer&& transformer) {
    transformer_ = std::move(transformer);
  }

  void Process(T* input_data, std::size_t input_size) {
    if (transformer_) {
      transformer_(input_data, input_size);
    }
  }

  [[nodiscard]] uint32_t GetDataPtr() {
    return data_ptr_;
  }

  [[nodiscard]] uint32_t GetDataPtr() const {
    return data_ptr_;
  }

  [[nodiscard]] std::size_t Size() const {
    return size_;
  }

  [[nodiscard]] std::size_t DataSize() const {
    // TODO: Take care of case when size_ = 0 and has_delimiter_ = true
    return has_delimiter_? size_ - 1 : size_;
  }

  [[nodiscard]] std::size_t VariableSize() const {
    return variable_size_;
  }

  [[nodiscard]] bool HasDelimiter() const {
    return has_delimiter_;
  }

 private:
  uint32_t data_ptr_;
  std::size_t variable_size_;
  std::size_t size_;
  bool has_delimiter_;

  Transformer transformer_;
};

template <typename T, typename U, std::size_t size>
class DataProcessor {
 public:
  using CurrentTransformer = Transformer<T, U>;

  explicit DataProcessor(CurrentTransformer&& transformer) :
      has_delimiter_(false), transformer_(std::move(transformer)) {
  }

  DataProcessor(CurrentTransformer&& transformer, U delimiter) :
      has_delimiter_(true), transformer_(std::move(transformer)) {

    // TODO: Take care of the corner case when size == 0
    //  and has_delimiter_ = true
    buffer_.back() = std::move(delimiter);
  }

  DataProcessor() = delete;
  ~DataProcessor() = default;

  DataProcessor(const DataProcessor&) = delete;
  DataProcessor& operator=(const DataProcessor&) = delete;

  DataProcessor(DataProcessor&&) noexcept = default;
  DataProcessor& operator=(DataProcessor&&) noexcept = default;

  DataProcessorView<T> GetView() && {
    // TODO: Wrap reinterpret cast for the compatibility with STM library
    DataProcessorView<T> view(reinterpret_cast<uint32_t>(buffer_.data()),
                              sizeof(U), buffer_.size(),
                              has_delimiter_);
    view.SetProcessor([this](T* input_data, std::size_t input_size){
      transformer_(input_data, buffer_.data(), input_size, buffer_.size());
    });

    return view;
  }

 private:
  std::array<U, size> buffer_;
  bool has_delimiter_;

  CurrentTransformer transformer_;
};

} // namespace Utils
} // namespace SD
