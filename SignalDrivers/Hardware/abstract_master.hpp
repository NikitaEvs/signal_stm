#pragma once

#include "port.hpp"

namespace SD {

namespace Hardware {
  /**
   * @brief Abstract base class
   * for a hardware-specific implementation of a main control class
   */
  class AbstractMaster {
   public:
    /**
     * @brief Enable RCC clocking, set dividers and configure FLASH latency
     */
    virtual void EnableClocking() const = 0;

    /**
     * @brief Stop execution for the certain time
     * @param millis - stopping time in milliseconds
     */
    virtual void Delay(uint32_t millis) const = 0;

    /**
     * @brief Enable clocking on the certain port
     * @param port
     */
    virtual void ClockPort(GPIO port) const = 0;

    /**
     * @brief Configure the GPIO certain pin for using as a I/O port
     * @param port
     * @param pin
     */
    virtual void ConfigureIOPort(GPIO port, Pin pin) const = 0;

    /**
     * @brief Set the certain pin in the certain port
     * @param port
     * @param pin
     */
    virtual void SetIOPort(GPIO port, Pin pin) const = 0;

    /**
     * @brief Reset the certain pin in the certain port
     * @param port
     * @param pin
     */
    virtual void ResetIOPort(GPIO port, Pin pin) const = 0;

    virtual void ConfigureUART(UART uart, bool async) const = 0;

    virtual void SendByteUARTBlocking(UART uart, uint8_t byte) const = 0;

    virtual uint8_t ReceiveByteUARTBlocking(UART uart) const = 0;

    virtual void ConfigureADCSingleChannelBlocking(ADCDevice device,
                                                   GPIO port, Pin pin) const = 0;

    virtual void CalibrateADC(ADCDevice device) const = 0;

    virtual uint16_t ReadADC(ADCDevice device) const = 0;

    virtual void ClockADC(Hardware::ADCDevice device) const = 0;

    virtual void AddADCChannel(Hardware::ADCDevice device,
                              uint8_t rank,
                              Hardware::GPIO port,
                              Hardware::Pin pin) const = 0;

    virtual
    void ConfigureADCDataSingleChannel(Hardware::ADCDevice device) const = 0;

    virtual void ConfigureADCDataScanning(Hardware::ADCDevice device) const = 0;

    virtual void ConfigureADC(Hardware::ADCDevice device,
                             uint8_t channels_number,
                             bool isDMA) const = 0;

    virtual Hardware::DMAPort GetDMAMapping(Hardware::ADCDevice device) const = 0;

    virtual Hardware::DMAPort GetDMAMapping(Hardware::UART device, bool isTX) const = 0;

    virtual void ConfigureDMAInterruption(Hardware::DMAPort dma_port) const = 0;

    virtual void ConfigureDMA(Hardware::DMAPort dma_port,
                             const Hardware::DMASettings& settings,
                             uint32_t from_address,
                             uint32_t to_address,
                             uint32_t size) const = 0;

    virtual void ConfigureDMA(Hardware::DMAPort dma_port,
                              const Hardware::DMASettings& settings,
                              Hardware::ADCDevice adc_device,
                              uint32_t to_address,
                              uint32_t size) const = 0;

    virtual void ConfigureDMA(Hardware::DMAPort dma_port,
                              const Hardware::DMASettings& settings,
                              Hardware::UART uart_device,
                              uint32_t to_address,
                              uint32_t size) const = 0;

    virtual void EnableDMA(Hardware::DMAPort dma_port) const = 0;

    virtual void DisableDMA(Hardware::DMAPort dma_port) const = 0;

    virtual void SetDMACallback(Hardware::DMAPort,
                                std::function<void()>&& func) = 0;

    virtual void ReadADCAsync(ADCDevice device) const = 0;
  };

} // namespace Hardware
} // namespace SD
