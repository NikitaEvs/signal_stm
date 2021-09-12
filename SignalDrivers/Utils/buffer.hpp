#pragma once

#include <array>
#include <span>

#include "Hardware/dma.hpp"

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
    GetInput()->Init(device, reinterpret_cast<uint32_t>(buffer_.data()),
                     CalculateSize(settings.dest_data_size), settings);
  }

  template<typename DeviceT>
  void ConnectDestination(DeviceT device,
                          Hardware::DMAPort dma_port,
                          const Hardware::DMASettings& settings) {
    SetOutput(dma_port);
    GetOutput()->Init(device, reinterpret_cast<uint32_t>(buffer_.data()),
                      CalculateSize(settings.dest_data_size,
                                    /* with_delimiter = */true),
                      settings);
  }

  void SetInputCallback(std::function<void()>&& callback) {
    auto& input = GetInput();
    if (input) {
      input->SetCallback(std::move(callback));
    }
  }

  void SetOutputCallback(std::function<void()>&& callback) {
    auto& output = GetOutput();
    if (output) {
      output->SetCallback(std::move(callback));
    }
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

  auto begin() {
    return buffer_.begin();
  }

  auto end() {
    return buffer_.end();
  }

 private:
  std::array<T, size> buffer_;
  bool has_delimiter_;

  [[nodiscard]] uint32_t CalculateSize(Hardware::DMASize dma_size,
                                       bool with_delimiter = false) const {
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
    if (!with_delimiter && has_delimiter_) {
      buffer_bytes_size = sizeof(T) * (size - 1);
    } else {
      buffer_bytes_size = sizeof(T) * size;
    }

    auto chunk_number = buffer_bytes_size / chunk_size;
    return chunk_number > 0 ? chunk_number : 1;
  }
};

template <typename T>
class AsyncBufferView : public Hardware::DMAUnit {
 public:
  AsyncBufferView(Hardware::AbstractMaster& master,
                  Hardware::DMADevice device,
                  Hardware::DMAChannel channel,
                  const T* data,
                  std::size_t size)
      : Hardware::DMAUnit(master, device, channel), data_(data), size_(size) {
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
