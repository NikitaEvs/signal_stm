#pragma once

#include "abstract_master.hpp"
#include "dma.hpp"
#include "hardware_layout.hpp"
#include "port.hpp"

namespace SD {

namespace Hardware {

template <bool Blocking>
class ADCUnit;

template<>
class ADCUnit<true> {
 public:
  ADCUnit(Hardware::AbstractMaster& master, ADCDevice device, Port port)
    : master_(master), device_(device) {
    master_.ConfigureADCSingleChannelBlocking(device, port.gpio, port.pin);
  }

  void Calibrate() {
    master_.CalibrateADC(device_);
  }

  uint16_t Read() {
    return master_.ReadADC(device_);
  }

 private:
  AbstractMaster& master_;
  ADCDevice device_;
};


template<>
class ADCUnit<false> {
 public:
  ADCUnit(Hardware::AbstractMaster& master, ADCDevice device)
      : master_(master), device_(device) {
    master_.ClockADC(device);
  }

  void AddChannel(Port port) {
    ++channels_counter_;
    master_.AddADCChannel(device_, channels_counter_, port.gpio, port.pin);
  }

  template<class Buffer>
  void ChainTo(const Buffer& buffer) const {
    buffer.ConnectSource(device_, GetDMASettings());
  }

  [[nodiscard]] Hardware::DMASettings GetDMAOutSettings() const {
    return GetDMASettings();
  }

  void Init() {
    if (channels_counter_ == 1) {
      master_.ConfigureADCDataSingleChannel(device_);
    } else if (channels_counter_ > 1) {
      master_.ConfigureADCDataScanning(device_);
    } else {
      return; // TODO: Insert an error failure
    }

    master_.ConfigureADC(device_, channels_counter_, /* isDMA = */ true);
  }

  void Calibrate() {
    master_.CalibrateADC(device_);
  }

  void Read() {
    master_.ReadADCAsync(device_);
  }

 private:
  AbstractMaster& master_;
  ADCDevice device_;

  uint8_t channels_counter_ = 0;

  [[nodiscard]] Hardware::DMASettings GetDMASettings() const {
    return {
        .direction = Hardware::DMADirection::PeriphToMemory,
        .mode = Hardware::DMAMode::Circular,
        .source_data_size = Hardware::DMASize::PeriphHalfword,
        .dest_data_size = Hardware::DMASize::MemoryHalfword,
        .priority = Hardware::DMAPriority::High
    };
  }
};

} // namespace Hardware
} // namespace SD
