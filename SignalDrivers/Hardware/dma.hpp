#pragma once

#include <cstdint>
#include <optional>

#include "abstract_master.hpp"
#include "hardware_layout.hpp"

namespace SD {

namespace Hardware {

struct DMAPort {
  DMADevice device;
  DMAChannel channel;
};

class DMAUnit {
 public:
  DMAUnit(AbstractMaster& master, DMADevice device, DMAChannel channel) :
    master_(master), device_(device), channel_(channel) {
    master.SetDMACallback(device, channel, [this](){
      is_prepared_ = true;
    });
  }

  DMAUnit(AbstractMaster& master, DMAPort dma_port) :
    DMAUnit(master, dma_port.device, dma_port.channel) {
  }

  DMAUnit(const DMAUnit& other) = default;

  DMAUnit(DMAUnit&& other) = delete;

  DMAUnit& operator=(const DMAUnit& other) {
    device_ = other.device_;
    channel_ = other.channel_;
    is_prepared_ = other.is_prepared_;

    return *this;
  }

  DMAUnit& operator=(DMAUnit&& other) = delete;

  ~DMAUnit() {
    Disable();
  }

  void Init(uint32_t from_address,
            uint32_t to_address,
            uint32_t size,
            const Hardware::DMASettings& settings) const {
    master_.ConfigureDMAInterruption(device_, channel_);
    master_.ConfigureDMA(device_, channel_, settings,
                         from_address, to_address, size);
  }

  void Init(Hardware::ADCDevice device,
            uint32_t mem_address,
            uint32_t size,
            const Hardware::DMASettings& settings) const {
    master_.ConfigureDMAInterruption(device_, channel_);
    master_.ConfigureDMA(device_, channel_, settings,
                         device, mem_address, size);
  }

  void Init(Hardware::UART device,
            uint32_t mem_address,
            uint32_t size,
            const Hardware::DMASettings& settings) const {
    master_.ConfigureDMAInterruption(device_, channel_);
    master_.ConfigureDMA(device_, channel_, settings,
                         device, mem_address, size);
  }

  void Enable() {
    is_prepared_ = false;
    master_.EnableDMA(device_, channel_);
  }

  void Disable() {
    is_prepared_ = false;
    master_.DisableDMA(device_, channel_);
  }

  void SetCallback(std::function<void()>&& callback) {
    master_.SetDMACallback(device_, channel_, [this, callback = std::move(callback)]{
      is_prepared_ = true;
      callback();
    });
  }

  void Wait() {
    while (!is_prepared_);
    is_prepared_ = false;
  }

 private:
  AbstractMaster& master_;

  DMADevice device_;
  DMAChannel channel_;

  bool is_prepared_ = false;
};


class DMAPipe {
 public:
  DMAPipe(AbstractMaster& master) : master_(master) {
  }

  [[deprecated]] void SetInput(Hardware::DMADevice device, Hardware::DMAChannel channel) {
    in_pipe_.emplace(master_, device, channel);
  }

  void SetInput(Hardware::DMAPort dma_port) {
    in_pipe_.emplace(master_, dma_port.device, dma_port.channel);
  }

  [[deprecated]] void SetOutput(Hardware::DMADevice device, Hardware::DMAChannel channel) {
    out_pipe_.emplace(master_, device, channel);
  }

  void SetOutput(Hardware::DMAPort dma_port) {
    out_pipe_.emplace(master_, dma_port.device, dma_port.channel);
  }

  [[nodiscard]] std::optional<DMAUnit>& GetInput() {
    return in_pipe_;
  }

  [[nodiscard]] std::optional<DMAUnit>& GetOutput() {
    return out_pipe_;
  }

 private:
  AbstractMaster& master_;

  std::optional<DMAUnit> in_pipe_;
  std::optional<DMAUnit> out_pipe_;
};

} // namespace Hardware
} // namespace SD
