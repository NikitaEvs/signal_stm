#pragma once

#include <functional>

#include "stm32f4xx_ll_adc.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_cortex.h"
#include "stm32f4xx_ll_dma.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_pwr.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx_ll_utils.h"

#include "Hardware/abstract_master.hpp"
#include "Hardware/hardware_layout.hpp"

#include "hardware_mapping.hpp"

namespace SD {

namespace Specific {

/**
 * @brief Master-class implementation for a certain controller
 */
class Master : public Hardware::AbstractMaster {
 public:
  /**
   * @brief System clock configuration
   * The system clock is configured as follow:
   * - System clock source  = PLL (HSE / 1)
   * - SYSCLK(MHz)          = 72
   * - HCLK(MHz)            = 72
   * - AHB prescaler        = 1
   * - APB1 prescaler       = 2
   * - APB2 prescaler       = 1
   * - HSE frequency(MHz)   = 8
   * - PLLMUL               = 9
   * - Flash latency        = 2
   */
  inline void EnableClocking() const override {
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_3);
    while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_3) {
    }

    LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
    LL_RCC_HSE_Enable();

    /* Wait till HSE is ready */
    while(LL_RCC_HSE_IsReady() != 1) {
    }

    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_12, 96, LL_RCC_PLLP_DIV_2);
    LL_RCC_PLL_Enable();

    /* Wait till PLL is ready */
    while(LL_RCC_PLL_IsReady() != 1) {
    }

    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

    /* Wait till System clock is ready */
    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) {
    }

    static constexpr uint32_t kTickTime = 100000000;
    static constexpr uint32_t kClockFrequency = 100000000;

    LL_Init1msTick(kTickTime);
    LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
    LL_SetSystemCoreClock(kClockFrequency);
    LL_RCC_SetTIMPrescaler(LL_RCC_TIM_PRESCALER_TWICE);
  }

  /**
   * @warning This implementation is a simple wrapper for LL_mDelay function
   * So there is a bug with extra millisecond
   * https://community.st.com/s/question/0D50X00009XkXQA/llmdelay-taking-an-extra-millisecond
   */
  inline void Delay(uint32_t millis) const override {
    LL_mDelay(millis);
  }

  inline void ClockPort(Hardware::GPIO port) const override {
    auto GPIOClock = ConvertGPIOClock(port);
    if (!LL_AHB1_GRP1_IsEnabledClock(GPIOClock)) {
      LL_AHB1_GRP1_EnableClock(GPIOClock);
    }
  }

  /**
   * @warning Currently configure the port as output
   */
  inline void ConfigureIOPort(Hardware::GPIO port,
                              Hardware::Pin pin) const override {
    ClockPort(port);

    ResetIOPort(port, pin);

    LL_GPIO_InitTypeDef GPIO_InitStruct = {};
    GPIO_InitStruct.Pin = ConvertPin(pin);
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT; // TODO: Add different modes
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;

    LL_GPIO_Init(ConvertGPIO(port), &GPIO_InitStruct);
    // TODO: Check return ErrorStatus
  }

  inline void SetIOPort(Hardware::GPIO port,
                        Hardware::Pin pin) const override {
    LL_GPIO_SetOutputPin(ConvertGPIO(port), ConvertPin(pin));
  }

  inline void ResetIOPort(Hardware::GPIO port,
                          Hardware::Pin pin) const override {
    LL_GPIO_ResetOutputPin(ConvertGPIO(port), ConvertPin(pin));
  }

  inline void ConfigureUART(Hardware::UART uart, bool async) const override {
    switch (uart) {
      case Hardware::UART::UART1:
        ConfigureUART1(kUART_SPEED, async);
        break;
      default:
        return; // TODO: Insert assert
    }
  }

  inline void SendByteUARTBlocking(Hardware::UART uart,
                                   uint8_t byte) const override {
    switch (uart) {
      case Hardware::UART::UART1:
        SendByteUART1Blocking(byte);
        break;
      default:
        return; // TODO: Insert assert
    }
  }

  inline uint8_t ReceiveByteUARTBlocking(Hardware::UART uart) const override {
    switch (uart) {
      case Hardware::UART::UART1:
        return ReceiveByteUART1Blocking();
      default:
        return {}; // TODO: Insert assert
    }
  }

  inline void ConfigureADCSingleChannelBlocking(Hardware::ADCDevice device,
                                         Hardware::GPIO port,
                                         Hardware::Pin pin) const override {
    // TODO: Catch errors
    LL_APB2_GRP1_EnableClock(ConvertADCClock(device));
    LL_AHB1_GRP1_EnableClock(ConvertGPIOClock(port));

    LL_GPIO_InitTypeDef GPIO_init = {};
    GPIO_init.Pin = ConvertPin(pin);
    GPIO_init.Mode = LL_GPIO_MODE_ANALOG;
    GPIO_init.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(ConvertGPIO(port), &GPIO_init);

    LL_ADC_InitTypeDef ADC_init = {};
    ADC_init.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
    ADC_init.SequencersScanMode = LL_ADC_SEQ_SCAN_DISABLE;
    ADC_init.Resolution = LL_ADC_RESOLUTION_12B;
    LL_ADC_Init(ConvertADCDevice(device), &ADC_init);

    LL_ADC_CommonInitTypeDef ADC_common_init = {};
    ADC_common_init.CommonClock = LL_ADC_CLOCK_SYNC_PCLK_DIV4;
    LL_ADC_CommonInit(__LL_ADC_COMMON_INSTANCE(ConvertADCDevice(device)),
                      &ADC_common_init);

    LL_ADC_REG_InitTypeDef ADC_reg_init = {};
    ADC_reg_init.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;
    ADC_reg_init.SequencerLength = LL_ADC_REG_SEQ_SCAN_DISABLE;
    ADC_reg_init.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
    ADC_reg_init.ContinuousMode = LL_ADC_REG_CONV_SINGLE;
    ADC_reg_init.DMATransfer = LL_ADC_REG_DMA_TRANSFER_NONE;
    LL_ADC_REG_Init(ConvertADCDevice(device), &ADC_reg_init);

    LL_ADC_REG_SetSequencerRanks(ConvertADCDevice(device),
                                 LL_ADC_REG_RANK_1,
                                 ConvertADCChannel(port, pin));
    LL_ADC_SetChannelSamplingTime(ConvertADCDevice(device),
                                  ConvertADCChannel(port, pin),
                                  LL_ADC_SAMPLINGTIME_3CYCLES);
  }

  inline void CalibrateADC(Hardware::ADCDevice device) const override {
    auto converted_ADC = ConvertADCDevice(device);

    LL_ADC_Enable(converted_ADC);

    // automated calibration
  }

  inline uint16_t ReadADC(Hardware::ADCDevice device) const override {
    auto converted_ADC = ConvertADCDevice(device);

    LL_ADC_REG_StartConversionSWStart(converted_ADC);
    while (!LL_ADC_IsActiveFlag_EOCS(converted_ADC));
    LL_ADC_ClearFlag_EOCS(converted_ADC);

    return LL_ADC_REG_ReadConversionData12(converted_ADC);
  }

  inline void ClockADC(Hardware::ADCDevice device) const override {
    auto ADCClock = ConvertADCClock(device);
    if (!LL_APB2_GRP1_IsEnabledClock(ADCClock)) {
      LL_APB2_GRP1_EnableClock(ADCClock);
    }
  }

  inline void AddADCChannel(Hardware::ADCDevice device,
                            uint8_t rank,
                            Hardware::GPIO port,
                            Hardware::Pin pin) const override {
    ClockPort(port);

    LL_ADC_REG_SetSequencerRanks(ConvertADCDevice(device),
                                 ConvertADCRank(rank),
                                 ConvertADCChannel(port, pin));
    LL_ADC_SetChannelSamplingTime(ConvertADCDevice(device),
                                  ConvertADCChannel(port, pin),
                                  LL_ADC_SAMPLINGTIME_480CYCLES);

    LL_GPIO_InitTypeDef GPIO_init = {};
    GPIO_init.Pin = ConvertPin(pin);
    GPIO_init.Mode = LL_GPIO_MODE_ANALOG;
    GPIO_init.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(ConvertGPIO(port), &GPIO_init);
  }

  inline
  void ConfigureADCDataSingleChannel(Hardware::ADCDevice device) const override {
    LL_ADC_InitTypeDef ADC_init = {};
    ADC_init.Resolution = LL_ADC_RESOLUTION_12B;
    ADC_init.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
    ADC_init.SequencersScanMode = LL_ADC_SEQ_SCAN_DISABLE;
    LL_ADC_Init(ConvertADCDevice(device), &ADC_init);
  }

  inline
  void ConfigureADCDataScanning(Hardware::ADCDevice device) const override {
    LL_ADC_InitTypeDef ADC_init = {};
    ADC_init.Resolution = LL_ADC_RESOLUTION_12B;
    ADC_init.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
    ADC_init.SequencersScanMode = LL_ADC_SEQ_SCAN_ENABLE;
    LL_ADC_Init(ConvertADCDevice(device), &ADC_init);
  }

  inline void ConfigureADC(Hardware::ADCDevice device,
                           uint8_t channels_number,
                           bool isDMA) const override {
    LL_ADC_CommonInitTypeDef ADC_common_init = {};
    ADC_common_init.CommonClock = LL_ADC_CLOCK_SYNC_PCLK_DIV4;
    LL_ADC_CommonInit(__LL_ADC_COMMON_INSTANCE(ConvertADCDevice(device)),
                      &ADC_common_init);

    LL_ADC_REG_InitTypeDef ADC_reg_init = {};
    ADC_reg_init.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;
    ADC_reg_init.SequencerLength = ConvertADCScan(channels_number);
    ADC_reg_init.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
    ADC_reg_init.ContinuousMode = LL_ADC_REG_CONV_CONTINUOUS;

    if (isDMA) {
      ADC_reg_init.DMATransfer = LL_ADC_REG_DMA_TRANSFER_UNLIMITED;
    } else {
      ADC_reg_init.DMATransfer = LL_ADC_REG_DMA_TRANSFER_NONE;
    }

    LL_ADC_REG_Init(ConvertADCDevice(device), &ADC_reg_init);
  }

  [[nodiscard]] inline
  Hardware::DMAPort GetDMAMapping(Hardware::ADCDevice device) const override {
    switch (device) {
      case Hardware::ADCDevice::ADC_1:
        return {
          .device = Hardware::DMADevice::DMA_2,
          .channel = Hardware::DMAChannel::Channel0,
          .stream = Hardware::DMAStream::Stream4
        };
      default:
        return {}; // TODO: Insert assert
    }
  }

  [[nodiscard]] inline
  Hardware::DMAPort GetDMAMapping(Hardware::UART device,
                                  bool isTX) const override {
    switch (device) {
      case Hardware::UART::UART1:
        if (isTX) {
          return {
              .device = Hardware::DMADevice::DMA_2,
              .channel = Hardware::DMAChannel::Channel4,
              .stream = Hardware::DMAStream::Stream7
          };
        } else {
          return {
              .device = Hardware::DMADevice::DMA_2,
              .channel = Hardware::DMAChannel::Channel4,
              .stream = Hardware::DMAStream::Stream5
          };
        }
      default:
        return {}; // TODO: Insert assert
    }
  }

  inline
  void ConfigureDMAInterruption(Hardware::DMAPort dma_port) const override {
    LL_AHB1_GRP1_EnableClock(ConvertDMAClock(dma_port.device));

    NVIC_SetPriority(ConvertDMAInterruption(dma_port),
                     NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
    NVIC_EnableIRQ(ConvertDMAInterruption(dma_port));
  }

  inline void ConfigureDMA(Hardware::DMAPort dma_port,
                           const Hardware::DMASettings& settings,
                           uint32_t periph_or_src_address,
                           uint32_t mem_or_dst_address,
                           uint32_t size) const override {
    auto dma = ConvertDMADevice(dma_port.device);
    auto stream = ConvertDMAStream(dma_port.stream);

    auto DMA_init = ConvertDMASettings(periph_or_src_address,
                                       mem_or_dst_address,
                                       size,
                                       settings);
    DMA_init.Channel = ConvertDMAChannel(dma_port.channel);
    DMA_init.FIFOMode = LL_DMA_FIFOMODE_DISABLE;

    LL_DMA_Init(dma, stream, &DMA_init);
    LL_DMA_DisableStream(dma, stream);
    LL_DMA_EnableIT_TC(dma, stream);
    LL_DMA_EnableIT_TE(dma, stream);
  }

  inline void ConfigureDMA(Hardware::DMAPort dma_port,
                            const Hardware::DMASettings& settings,
                            Hardware::ADCDevice adc_device,
                            uint32_t mem_address,
                            uint32_t size) const override {
    auto adc = ConvertADCDevice(adc_device);
    auto periph_address = LL_ADC_DMA_GetRegAddr(adc, LL_ADC_DMA_REG_REGULAR_DATA);

    ConfigureDMA(dma_port, settings,
                 periph_address, mem_address, size);
  }

  inline void ConfigureDMA(Hardware::DMAPort dma_port,
                           const Hardware::DMASettings& settings,
                           Hardware::UART uart_device,
                           uint32_t mem_address,
                           uint32_t size) const override {
    auto uart = ConvertUART(uart_device);
    auto periph_address = LL_USART_DMA_GetRegAddr(uart);

    ConfigureDMA(dma_port, settings,
                 periph_address, mem_address, size);
  }

  inline void EnableDMA(Hardware::DMAPort dma_port) const override {
    LL_DMA_EnableStream(ConvertDMADevice(dma_port.device),
                        ConvertDMAStream(dma_port.stream));
  }

  inline void DisableDMA(Hardware::DMAPort dma_port) const override {
    LL_DMA_DisableStream(ConvertDMADevice(dma_port.device),
                        ConvertDMAStream(dma_port.stream));
  }

  inline void SetDMACallback(Hardware::DMAPort dma_port,
                             std::function<void()>&& func) override {
    switch (dma_port.device) {
      case Hardware::DMADevice::DMA_2:
        switch (dma_port.stream) {
          case Hardware::DMAStream::Stream4:
            dma2_stream4_callback_ = std::move(func);
            break;
          case Hardware::DMAStream::Stream7:
            dma2_stream7_callback_ = std::move(func);
            break;
          default:
            // TODO: Insert assert
            return;
        }
        break;
      default:
        // TODO: Insert assert
        return;
    }
  }

  void DMACallback(Hardware::DMAPort dma_port) const {
    switch (dma_port.device) {
      case Hardware::DMADevice::DMA_2:
        switch (dma_port.stream) {
          case Hardware::DMAStream::Stream4:
            ExecuteCallback(dma2_stream4_callback_);
            break;
          case Hardware::DMAStream::Stream7:
            ExecuteCallback(dma2_stream7_callback_);
            break;
          default:
            // TODO: Insert assert
            return;
        }
        break;
      default:
        // TODO: Insert assert
        return;
    }
  }

  void ReadADCAsync(Hardware::ADCDevice device) const override {
    auto adc = ConvertADCDevice(device);

    // Reference manual 11.8.1: we need to clear OVR bit in order to
    // continue ADC sampling while using DMA after one transaction
    LL_ADC_ClearFlag_OVR(adc);

    LL_ADC_REG_StartConversionSWStart(adc);
  }

 private:
  std::function<void()> dma2_stream4_callback_;
  std::function<void()> dma2_stream7_callback_;

  static inline void ConfigureUART1(uint32_t speed, bool async) {
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

    LL_USART_InitTypeDef USART_init = {};
    LL_GPIO_InitTypeDef GPIO_init = {};

    GPIO_init.Pin = LL_GPIO_PIN_9 | LL_GPIO_PIN_10;
    GPIO_init.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_init.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_init.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_init.Pull = LL_GPIO_PULL_NO;
    GPIO_init.Alternate = LL_GPIO_AF_7;
    LL_GPIO_Init(GPIOA, &GPIO_init);

    USART_init.BaudRate = speed;
    USART_init.DataWidth = LL_USART_DATAWIDTH_8B;
    USART_init.StopBits = LL_USART_STOPBITS_1;
    USART_init.Parity = LL_USART_PARITY_NONE;
    USART_init.TransferDirection = LL_USART_DIRECTION_TX_RX;
    USART_init.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    USART_init.OverSampling = LL_USART_OVERSAMPLING_16;
    LL_USART_Init(USART1, &USART_init); // TODO: Check ErrorStatus
    LL_USART_ConfigAsyncMode(USART1);
    LL_USART_Enable(USART1);

    if (async) {
      LL_USART_EnableDMAReq_TX(USART1);
    }
  }

  static inline void SendByteUART1Blocking(uint8_t byte) {
    static const auto using_uart = ConvertUART(Hardware::UART::UART1);

    while (!LL_USART_IsActiveFlag_TXE(using_uart)){
    }

    LL_USART_TransmitData8(using_uart, byte);
  }

  static inline uint8_t ReceiveByteUART1Blocking() {
    static const auto using_uart = ConvertUART(Hardware::UART::UART1);

    while (!LL_USART_IsActiveFlag_RXNE(using_uart)){
    }

    return LL_USART_ReceiveData8(using_uart);
  }

  static void ExecuteCallback(const std::function<void()>& func) {
    if (func) {
      func();
    }
  }
};

static Master master_instance;

Master& GetMasterInstance() {
  return master_instance;
}


} // namespace Specific
} // namespace SD

#include "handlers.ipp"
