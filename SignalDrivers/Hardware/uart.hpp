#pragma once

#include <cstdint>

#include "abstract_master.hpp"
#include "dma.hpp"
#include "hardware_layout.hpp"

#include "Utils/buffer.hpp"

namespace SD {

namespace Hardware {

class BaseUART {
 public:
  BaseUART(Hardware::AbstractMaster& master, UART uart, bool async = false)
    : master_(master), uart_(uart) {
    master_.ConfigureUART(uart, async);
  }

  virtual void SendBytes(const uint8_t* buffer, std::size_t size) const = 0;

 protected:
  Hardware::AbstractMaster& master_;
  Hardware::UART uart_;
};

class BlockingUART : public BaseUART {
 public:
  BlockingUART(Hardware::AbstractMaster& master, UART uart)
    : BaseUART(master, uart) {
  }

  void SendBytes(const uint8_t* buffer, std::size_t size) const override {
    std::size_t pos = 0;

    while (pos < size) {
      SendByte(buffer[pos]);
      ++pos;
    }
  }

 private:
  void SendByte(uint8_t byte) const {
    master_.SendByteUARTBlocking(uart_, byte);
  }

  [[nodiscard]] uint8_t ReceiveByte() const {
    return master_.ReceiveByteUARTBlocking(uart_);
  }
};

class AsyncUART : public BaseUART {
 public:
  AsyncUART(Hardware::AbstractMaster& master, UART uart) :
    BaseUART(master, uart, /* async = */ true) {
  }

  template<class Buffer>
  void ChainTo(const Buffer& buffer) const {
    buffer.ConnectSource(uart_, GetDMARXSettings());
  }

  template<class Buffer>
  void ChainFrom(const Buffer& buffer) const {
    buffer.ConnectSource(uart_, GetDMATXSettings());
  }

  [[nodiscard]] Hardware::DMASettings GetDMAOutSettings() const {
    return GetDMARXSettings();
  }

  [[nodiscard]] Hardware::DMASettings GetDMAInSettings() const {
    return GetDMATXSettings();
  }

  // TODO: Make sure that this blocking method is needed
  void SendBytes(const uint8_t* buffer, std::size_t size) const override {
    auto using_dma = master_.GetDMAMapping(uart_, /* isTx= */ true);
    Utils::AsyncBufferView<uint8_t> view(master_,
                                         using_dma,
                                         buffer, size);
    ChainFrom(view);
    view.Enable();
    view.Wait();
  }

 private:
  [[nodiscard]] Hardware::DMASettings GetDMARXSettings() const {
    return {
        .direction = Hardware::DMADirection::PeriphToMemory,
        .mode = Hardware::DMAMode::Circular,
        .source_data_size = Hardware::DMASize::PeriphByte,
        .dest_data_size = Hardware::DMASize::MemoryByte,
        .priority = Hardware::DMAPriority::High
    };
  }

  [[nodiscard]] Hardware::DMASettings GetDMATXSettings() const {
    return {
        .direction = Hardware::DMADirection::MemoryToPeriph,
        .mode = Hardware::DMAMode::Circular,
        .source_data_size = Hardware::DMASize::PeriphByte,
        .dest_data_size = Hardware::DMASize::MemoryByte,
        .priority = Hardware::DMAPriority::High
    };
  }
};

} // namespace Hardware
} // namespace SD
