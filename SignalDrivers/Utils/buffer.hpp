#pragma once

#include <array>

#include "Hardware/dma.hpp"

#include "Utils/data.hpp"

namespace SD {

namespace Utils {

template <typename T, std::size_t size>
class AsyncBuffer : public Hardware::DMAPipe {
 public:
  explicit AsyncBuffer(Hardware::AbstractMaster& master) :
    Hardware::DMAPipe(master), has_delimiter_(false) {
  }

  AsyncBuffer(Hardware::AbstractMaster& master, T delimiter) :
    Hardware::DMAPipe(master), has_delimiter_(true) {
    buffer_.back() = delimiter;
  }

  template<typename DeviceT>
  void ConnectSource(DeviceT device,
                     Hardware::DMAPort dma_port,
                     const Hardware::DMASettings& settings) {
    SetInput(dma_port);
    GetInput()->Init(device,
                     reinterpret_cast<uint32_t>(buffer_.data()),
                     CalculateSize(settings.dest_data_size,
                                   size,
                                   has_delimiter_),
                     settings);
  }

  template<typename DeviceT>
  void ConnectDestination(DeviceT device,
                          Hardware::DMAPort dma_port,
                          const Hardware::DMASettings& settings) {
    SetOutput(dma_port);

    uint32_t data_ptr = 0;
    std::size_t data_size = 0;
    std::size_t variable_size = 0;
    bool data_has_delimiter = false;

    if (processor_view_.IsValid()) {
      data_ptr = processor_view_.GetDataPtr();
      data_size = processor_view_.Size();
      variable_size = processor_view_.VariableSize();
      data_has_delimiter = processor_view_.HasDelimiter();
    } else {
      data_ptr = reinterpret_cast<uint32_t>(GetDataPointer());
      data_size = Size();
      variable_size = sizeof(T);
      data_has_delimiter = has_delimiter_;
    }

    GetOutput()->Init(device,
                      data_ptr,
                      CalculateSize(settings.dest_data_size,
                                    data_size,
                                    variable_size,
                                    data_has_delimiter,
                                    /*with_delimiter=*/true),
                      settings);
  }

  void SetInputCallback(std::function<void()>&& callback) {
    auto& input = GetInput();
    if (input) {
      if (processor_view_.IsValid()) {
        input->SetCallback([this, callback = std::move(callback)](){
          this->DisableInput();

          this->processor_view_.Process(GetDataPointer(), DataSize());
          callback();
        });
      } else {
        input->SetCallback([this, callback = std::move(callback)](){
          this->DisableInput();

          callback();
        });
      }
    }
  }

  void SetOutputCallback(std::function<void()>&& callback) {
    auto& output = GetOutput();
    if (output) {
      output->SetCallback([this, callback = std::move(callback)](){
        this->DisableOutput();

        callback();
      });
    }
  }

  void SetDataProcessor(DataProcessorView<T>&& view) {
    processor_view_ = std::move(view);
  }

  void EnableInput() {
    auto& input = GetInput();
    if (input) {
      input->Enable();
    }
  }

  void DisableInput() {
    auto& input = GetInput();
    if (input) {
      input->Disable();
    }
  }

  void WaitInput() {
    auto& input = GetInput();
    if (input) {
      input->Wait();
    }
  }

  void EnableOutput() {
    auto& output = GetOutput();
    if (output) {
      output->Enable();
    }
  }

  void DisableOutput() {
    auto& output = GetOutput();
    if (output) {
      output->Disable();
    }
  }

  void WaitOutput() {
    auto& output = GetOutput();
    if (output) {
      output->Wait();
    }
  }

  T& operator[](std::size_t index) {
    return buffer_[index];
  }

  [[nodiscard]] T* GetDataPointer() const {
    return buffer_.data();
  }

  [[nodiscard]] T* GetDataPointer() {
    return buffer_.data();
  }

  [[nodiscard]] std::size_t Size() const {
    return size;
  }

  [[nodiscard]] std::size_t DataSize() const {
    return has_delimiter_? size - 1 : size;
  }

  auto begin() {
    return buffer_.begin();
  }

  auto end() {
    return buffer_.end();
  }

 private:
  std::array<T, size> buffer_;
  bool has_delimiter_;

  DataProcessorView<T> processor_view_ = DataProcessorView<T>::Invalid();

  [[nodiscard]] static uint32_t CalculateSize(Hardware::DMASize dma_size,
                                              std::size_t data_size,
                                              std::size_t variable_size,
                                              bool has_delimiter,
                                              bool with_delimiter = false) {
    // TODO: Check that it isn't implementation defined
    uint32_t chunk_size = 0;

    switch (dma_size) {
      case Hardware::DMASize::MemoryByte:
        chunk_size = 1;
        break;
      case Hardware::DMASize::MemoryHalfword:
        chunk_size = 2;
        break;
      case Hardware::DMASize::MemoryWord:
        chunk_size = 4;
        break;
      default:
        // TODO: Insert assert
        return {};
    }

    std::size_t buffer_bytes_size;
    if (!with_delimiter && has_delimiter) {
      buffer_bytes_size = variable_size * (data_size - 1);
    } else {
      buffer_bytes_size = variable_size * data_size;
    }

    auto chunk_number = buffer_bytes_size / chunk_size;
    return chunk_number > 0 ? chunk_number : 1;
  }

  [[nodiscard]] static uint32_t CalculateSize(Hardware::DMASize dma_size,
                                       std::size_t data_size,
                                       bool has_delimiter,
                                       bool with_delimiter = false) {
    return CalculateSize(dma_size, data_size, sizeof(T),
                         has_delimiter, with_delimiter);
  }
};

template <typename T>
class AsyncBufferView : public Hardware::DMAUnit {
 public:
  AsyncBufferView(Hardware::AbstractMaster& master,
                  Hardware::DMAPort dma_port,
                  const T* data,
                  std::size_t size)
      : Hardware::DMAUnit(master, dma_port), data_(data), size_(size) {
  }

  void ConnectSource(uint32_t from_address,
                     const Hardware::DMASettings& settings) const {
    Init(from_address, reinterpret_cast<uint32_t>(data_), size_,
         settings);
  }

  void ConnectSource(Hardware::ADCDevice device,
                     const Hardware::DMASettings& settings) const {
    Init(device, reinterpret_cast<uint32_t>(data_), size_,
         settings);
  }

  void ConnectSource(Hardware::UART device,
                     const Hardware::DMASettings& settings) const {
    Init(device, reinterpret_cast<uint32_t>(data_), size_,
         settings);
  }

  void ConnectDestination(uint32_t to_address,
                          const Hardware::DMASettings& settings) const {
    Init(reinterpret_cast<uint32_t>(data_), to_address, size_,
         settings);
  }

  T& operator[](std::size_t index) {
    return data_[index];
  }

  [[nodiscard]] T* GetDataPointer() const {
    return data_;
  }

  [[nodiscard]] T* GetDataPointer() {
    return data_;
  }

  [[nodiscard]] std::size_t Size() const {
    return size_;
  }

 private:
  const T* data_;
  std::size_t size_;
};

} // namespace Utils
} // namespace SD
