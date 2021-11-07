#pragma once

namespace SD {

namespace Hardware {

/**
 * @brief Abstraction for a GPIO port
 */
enum class GPIO {
  A,
  B,
  C,
  D
};

/**
 * @brief Abstraction for a GPIO pin
 */
enum class Pin {
  Pin0,
  Pin1,
  Pin2,
  Pin3,
  Pin4,
  Pin5,
  Pin6,
  Pin7,
  Pin8,
  Pin9,
  Pin10,
  Pin11,
  Pin12,
  Pin13,
  Pin14,
  Pin15
};

enum class UART {
  UART1
};

enum class ADCDevice {
  ADC_1,
  ADC_2
};

enum class DMADevice {
  DMA_1,
  DMA_2
};

enum class DMAChannel {
  Channel0,
  Channel1,
  Channel2,
  Channel3,
  Channel4,
  Channel5,
  Channel6,
  Channel7
};

enum class DMAStream {
  Stream0,
  Stream1,
  Stream2,
  Stream3,
  Stream4,
  Stream5,
  Stream6,
  Stream7
};

// Default initialization is needed because some STM models don't use
// DMAStream, but we want list initialization support
struct DMAPort {
  DMADevice device = DMADevice::DMA_1;
  DMAChannel channel = DMAChannel::Channel1;
  DMAStream stream = DMAStream::Stream0;
};

enum class DMADirection {
  PeriphToMemory,
  MemoryToPeriph,
  MemoryToMemory
};

enum class DMAMode {
  Normal,
  Circular
};

enum class DMASize {
  PeriphByte,
  PeriphHalfword,
  PeriphWord,
  MemoryByte,
  MemoryHalfword,
  MemoryWord
};

enum class DMAPriority {
  Low,
  Medium,
  High,
  VeryHigh
};

// TODO: Better naming for source_data_size and dest_data_size
struct DMASettings {
  DMADirection direction;
  DMAMode mode;
  DMASize source_data_size;
  DMASize dest_data_size;
  DMAPriority priority;
};

} // namespace Hardware
} // namespace SD
