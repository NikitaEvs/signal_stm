#pragma once

#include <cstdint>
#include <optional>

#include "abstract_master.hpp"
#include "hardware_layout.hpp"

namespace SD {

namespace Hardware {

class DMAUnit {
 public:
  DMAUnit(AbstractMaster& master, DMAPort dma_port) :
    master_(master), dma_port_(dma_port) {
    master.SetDMACallback(dma_port_, [this](){
      is_prepared_ = true;
    });
  }

  DMAUnit(const DMAUnit& other) = default;

  DMAUnit(DMAUnit&& other) = delete;

  DMAUnit& operator=(const DMAUnit& other) {
    dma_port_ = other.dma_port_;
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
    master_.ConfigureDMAInterruption(dma_port_);
    master_.ConfigureDMA(dma_port_, settings,
                         from_address, to_address, size);
  }

  void Init(Hardware::ADCDevice device,
            uint32_t mem_address,
            uint32_t size,
            const Hardware::DMASettings& settings) const {
    master_.ConfigureDMAInterruption(dma_port_);
    master_.ConfigureDMA(dma_port_, settings,
                         device, mem_address, size);
  }

  void Init(Hardware::UART device,
            uint32_t mem_address,
            uint32_t size,
            const Hardware::DMASettings& settings) const {
    master_.ConfigureDMAInterruption(dma_port_);
    master_.ConfigureDMA(dma_port_, settings,
                         device, mem_address, size);
  }

  void Enable() {
    is_prepared_ = false;
    master_.EnableDMA(dma_port_);
  }

  void Disable() {
    master_.DisableDMA(dma_port_);
  }

  void SetCallback(std::function<void()>&& callback) {
    master_.SetDMACallback(dma_port_, [this, callback = std::move(callback)]{
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

  DMAPort dma_port_;
  bool is_prepared_ = false;
};


class DMAPipe {
 public:
  explicit DMAPipe(AbstractMaster& master) : master_(master) {
  }

  void SetInput(Hardware::DMAPort dma_port) {
    in_pipe_.emplace(master_, dma_port);
  }

  void SetOutput(Hardware::DMAPort dma_port) {
    out_pipe_.emplace(master_, dma_port);
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
