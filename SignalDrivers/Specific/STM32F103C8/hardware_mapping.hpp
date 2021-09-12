#pragma once

#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_usart.h"

#include "Hardware/hardware_layout.hpp"

namespace SD {

namespace Specific {

static constexpr uint32_t kUART_SPEED = 115200;

/**
 * @brief Util function for converting constants from the Hardware::GPIO type
 * to the LL macros
 * @param port
 * @return corresponding LL macros for the port
 */
static inline GPIO_TypeDef* ConvertGPIO(Hardware::GPIO port) {
  switch (port) {
    case Hardware::GPIO::A: return GPIOA;
    case Hardware::GPIO::B: return GPIOB;
    case Hardware::GPIO::C: return GPIOC;
    case Hardware::GPIO::D: return GPIOD;
    default: return {}; // TODO: Insert assert
  }
}

/**
 * @brief Util function for converting constants from the Hardware::GPIO type
 * to the LL macros
 * @param port
 * @return corresponding LL macros for the port clocking line
 */
static inline constexpr uint32_t ConvertGPIOClock(Hardware::GPIO port) {
  switch (port) {
    case Hardware::GPIO::A: return LL_APB2_GRP1_PERIPH_GPIOA;
    case Hardware::GPIO::B: return LL_APB2_GRP1_PERIPH_GPIOB;
    case Hardware::GPIO::C: return LL_APB2_GRP1_PERIPH_GPIOC;
    case Hardware::GPIO::D: return LL_APB2_GRP1_PERIPH_GPIOD;
    default: return {}; // TODO: Insert assert
  }
}

static inline constexpr uint32_t ConvertADCClock(Hardware::ADCDevice adc) {
  switch (adc) {
    case Hardware::ADCDevice::ADC_1: return LL_APB2_GRP1_PERIPH_ADC1;
    case Hardware::ADCDevice::ADC_2: return LL_APB2_GRP1_PERIPH_ADC2;
    default: return {}; // TODO: Insert assert
  }
}

static inline constexpr uint32_t ConvertDMAClock(Hardware::DMADevice device) {
  switch (device) {
    case Hardware::DMADevice::DMA_1: return LL_AHB1_GRP1_PERIPH_DMA1;
    default:
      return {}; // TODO: Insert assert
  }
}

/**
 * @brief Util function for converting constants from the Hardware::Pin type
 * to the LL macros
 * @param pin
 * @return corresponding LL macros for the GPIO pin
 */
static inline constexpr uint32_t ConvertPin(Hardware::Pin pin) {
  switch (pin) {
    case Hardware::Pin::Pin0: return LL_GPIO_PIN_0;
    case Hardware::Pin::Pin1: return LL_GPIO_PIN_1;
    case Hardware::Pin::Pin2: return LL_GPIO_PIN_2;
    case Hardware::Pin::Pin3: return LL_GPIO_PIN_3;
    case Hardware::Pin::Pin4: return LL_GPIO_PIN_4;
    case Hardware::Pin::Pin5: return LL_GPIO_PIN_5;
    case Hardware::Pin::Pin6: return LL_GPIO_PIN_6;
    case Hardware::Pin::Pin7: return LL_GPIO_PIN_7;
    case Hardware::Pin::Pin8: return LL_GPIO_PIN_8;
    case Hardware::Pin::Pin9: return LL_GPIO_PIN_9;
    case Hardware::Pin::Pin10: return LL_GPIO_PIN_10;
    case Hardware::Pin::Pin11: return LL_GPIO_PIN_11;
    case Hardware::Pin::Pin12: return LL_GPIO_PIN_12;
    case Hardware::Pin::Pin13: return LL_GPIO_PIN_13;
    case Hardware::Pin::Pin14: return LL_GPIO_PIN_14;
    case Hardware::Pin::Pin15: return LL_GPIO_PIN_15;
    default: return {}; // TODO: Insert assert
  }
}

static inline USART_TypeDef* ConvertUART(Hardware::UART uart) {
  switch (uart) {
    case Hardware::UART::UART1: return USART1;
    default: return {}; // TODO: Insert assert
  }
}

static inline ADC_TypeDef* ConvertADCDevice(Hardware::ADCDevice adc) {
  switch (adc) {
    case Hardware::ADCDevice::ADC_1: return ADC1;
    case Hardware::ADCDevice::ADC_2: return ADC2;
    default: return {}; // TODO: Insert assert
  }
}

static inline constexpr uint32_t ConvertADCChannel(Hardware::GPIO gpio,
                                                   Hardware::Pin pin) {
  switch (gpio) {
    case Hardware::GPIO::A:
      switch (pin) {
        case Hardware::Pin::Pin0: return LL_ADC_CHANNEL_0;
        case Hardware::Pin::Pin1: return LL_ADC_CHANNEL_1;
        case Hardware::Pin::Pin2: return LL_ADC_CHANNEL_2;
        case Hardware::Pin::Pin3: return LL_ADC_CHANNEL_3;
        case Hardware::Pin::Pin4: return LL_ADC_CHANNEL_4;
        case Hardware::Pin::Pin5: return LL_ADC_CHANNEL_5;
        case Hardware::Pin::Pin6: return LL_ADC_CHANNEL_6;
        case Hardware::Pin::Pin7: return LL_ADC_CHANNEL_7;
        default:
          return {}; // TODO: Insert assert
      }
    case Hardware::GPIO::B:
      switch (pin) {
        case Hardware::Pin::Pin0: return LL_ADC_CHANNEL_8;
        case Hardware::Pin::Pin1: return LL_ADC_CHANNEL_9;
        default:
          return {}; // TODO: Insert assert
      }
    default:
      return {}; // TODO: Insert assert
  }
}

static inline DMA_TypeDef* ConvertDMADevice(Hardware::DMADevice device) {
  switch (device) {
    case Hardware::DMADevice::DMA_1: return DMA1;
    default:
      return {}; // TODO: Insert assert
  }
}

static inline constexpr uint32_t ConvertDMAChannel(Hardware::DMAChannel channel) {
  switch (channel) {
    case Hardware::DMAChannel::Channel1: return LL_DMA_CHANNEL_1;
    case Hardware::DMAChannel::Channel2: return LL_DMA_CHANNEL_2;
    case Hardware::DMAChannel::Channel3: return LL_DMA_CHANNEL_3;
    case Hardware::DMAChannel::Channel4: return LL_DMA_CHANNEL_4;
    case Hardware::DMAChannel::Channel5: return LL_DMA_CHANNEL_5;
    case Hardware::DMAChannel::Channel6: return LL_DMA_CHANNEL_6;
    case Hardware::DMAChannel::Channel7: return LL_DMA_CHANNEL_7;
    default:
      return {}; // TODO: Insert assert
  }
}

static inline
constexpr IRQn_Type ConvertDMAInterruption(Hardware::DMADevice device,
                                           Hardware::DMAChannel channel) {
  switch (device) {
    case Hardware::DMADevice::DMA_1:
      switch (channel) {
        case Hardware::DMAChannel::Channel1: return DMA1_Channel1_IRQn;
        case Hardware::DMAChannel::Channel2: return DMA1_Channel2_IRQn;
        case Hardware::DMAChannel::Channel3: return DMA1_Channel3_IRQn;
        case Hardware::DMAChannel::Channel4: return DMA1_Channel4_IRQn;
        case Hardware::DMAChannel::Channel5: return DMA1_Channel5_IRQn;
        case Hardware::DMAChannel::Channel6: return DMA1_Channel6_IRQn;
        case Hardware::DMAChannel::Channel7: return DMA1_Channel7_IRQn;
        default:
          return {}; // TODO: Insert assert
      }
    default:
      return {}; // TODO: Insert assert
  }
}

static inline LL_DMA_InitTypeDef ConvertDMASettings(uint32_t periph_or_src_address,
                                                    uint32_t mem_or_dst_address,
                                                    uint32_t size,
                                                    const Hardware::DMASettings& settings) {
  LL_DMA_InitTypeDef init = {};
  init.PeriphOrM2MSrcAddress = periph_or_src_address;
  init.MemoryOrM2MDstAddress = mem_or_dst_address;

  switch (settings.direction) {
    case Hardware::DMADirection::MemoryToPeriph:
      init.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
      break;
    case Hardware::DMADirection::PeriphToMemory:
      init.Direction = LL_DMA_DIRECTION_PERIPH_TO_MEMORY;
      break;
    case Hardware::DMADirection::MemoryToMemory:
      init.Direction = LL_DMA_DIRECTION_MEMORY_TO_MEMORY;
      break;
    default:
      return {}; // TODO: Insert assert
  }

  switch (settings.mode) {
    case Hardware::DMAMode::Normal:
      init.Mode = LL_DMA_MODE_NORMAL;
      break;
    case Hardware::DMAMode::Circular:
      init.Mode = LL_DMA_MODE_CIRCULAR;
      break;
    default:
      return {}; // TODO: Insert assert
  }

  init.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT; // TODO: Add support to the increment format
  init.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;

  switch (settings.source_data_size) {
    case Hardware::DMASize::PeriphByte:
      init.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_BYTE;
      break;
    case Hardware::DMASize::PeriphHalfword:
      init.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_HALFWORD;
      break;
    case Hardware::DMASize::PeriphWord:
      init.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_WORD;
      break;
    default:
      return {}; // TODO: Insert assert
  }

  switch (settings.dest_data_size) {
    case Hardware::DMASize::MemoryByte:
      init.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_BYTE;
      break;
    case Hardware::DMASize::MemoryHalfword:
      init.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_HALFWORD;
      break;
    case Hardware::DMASize::MemoryWord:
      init.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_WORD;
      break;
    default:
      return {}; // TODO: Insert assert
  }

  init.NbData = size;

  switch (settings.priority) {
    case Hardware::DMAPriority::Low:
      init.Priority = LL_DMA_PRIORITY_LOW;
      break;
    case Hardware::DMAPriority::Medium:
      init.Priority = LL_DMA_PRIORITY_MEDIUM;
      break;
    case Hardware::DMAPriority::High:
      init.Priority = LL_DMA_PRIORITY_HIGH;
      break;
    case Hardware::DMAPriority::VeryHigh:
      init.Priority = LL_DMA_PRIORITY_VERYHIGH;
      break;
    default:
      return {}; // TODO: Insert assert
  }

  return init;
}

static inline constexpr uint32_t ConvertADCRank(uint8_t rank) {
  switch (rank) {
    case 1: return LL_ADC_REG_RANK_1;
    case 2: return LL_ADC_REG_RANK_2;
    case 3: return LL_ADC_REG_RANK_3;
    case 4: return LL_ADC_REG_RANK_4;
    case 5: return LL_ADC_REG_RANK_5;
    case 6: return LL_ADC_REG_RANK_6;
    case 7: return LL_ADC_REG_RANK_7;
    case 8: return LL_ADC_REG_RANK_8;
    case 9: return LL_ADC_REG_RANK_9;
    case 10: return LL_ADC_REG_RANK_10;
    case 11: return LL_ADC_REG_RANK_11;
    case 12: return LL_ADC_REG_RANK_12;
    case 13: return LL_ADC_REG_RANK_13;
    case 14: return LL_ADC_REG_RANK_14;
    case 15: return LL_ADC_REG_RANK_15;
    case 16: return LL_ADC_REG_RANK_16;
    default:
      return {}; // TODO: Insert assert
  }
}

static inline constexpr uint32_t ConvertADCScan(uint8_t number) {
  switch (number) {
    case 1: return {}; // TODO: Insert assert
    case 2: return LL_ADC_REG_SEQ_SCAN_ENABLE_2RANKS;
    case 3: return LL_ADC_REG_SEQ_SCAN_ENABLE_3RANKS;
    case 4: return LL_ADC_REG_SEQ_SCAN_ENABLE_4RANKS;
    case 5: return LL_ADC_REG_SEQ_SCAN_ENABLE_5RANKS;
    case 6: return LL_ADC_REG_SEQ_SCAN_ENABLE_6RANKS;
    case 7: return LL_ADC_REG_SEQ_SCAN_ENABLE_7RANKS;
    case 8: return LL_ADC_REG_SEQ_SCAN_ENABLE_8RANKS;
    case 9: return LL_ADC_REG_SEQ_SCAN_ENABLE_9RANKS;
    case 10: return LL_ADC_REG_SEQ_SCAN_ENABLE_10RANKS;
    case 11: return LL_ADC_REG_SEQ_SCAN_ENABLE_11RANKS;
    case 12: return LL_ADC_REG_SEQ_SCAN_ENABLE_12RANKS;
    case 13: return LL_ADC_REG_SEQ_SCAN_ENABLE_13RANKS;
    case 14: return LL_ADC_REG_SEQ_SCAN_ENABLE_14RANKS;
    case 15: return LL_ADC_REG_SEQ_SCAN_ENABLE_15RANKS;
    case 16: return LL_ADC_REG_SEQ_SCAN_ENABLE_16RANKS;
    default:
      return {}; // TODO: Insert assert
  }
}

} // namespace Specific
} // namespace SD
