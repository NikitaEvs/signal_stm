/**
 * @brief Example of the library usage with led blinking
 * while data is processed in the background
 */
// Choose the using driver (the corresponding DEVICE variable
// should be set as the CMake argument)
#include "Specific/STM32F411CE6/master.hpp"

#include "Utils/buffer.hpp"
#include "Utils/led.hpp"
#include "Utils/logger.hpp"

#include "Hardware/adc.hpp"
#include "Hardware/hardware_layout.hpp"

int main() {
  // Get main supervisor instance and enable default clocking
  auto& master = SD::Specific::GetMasterInstance();
  master.EnableClocking();

  // Create a system LED instance
  SD::Utils::LED led(master);

  // Create non-blocking UART class
  SD::Hardware::AsyncUART uart(master, SD::Hardware::UART::UART1);

  // Create non-blocking ADC class, add one channel and configure it
  SD::Hardware::ADCUnit</*Blocking=*/false> adc(master,
                                            SD::Hardware::ADCDevice::ADC_1);
  adc.AddChannel({SD::Hardware::GPIO::A, SD::Hardware::Pin::Pin0});
  adc.Init();

  // Create storage with the asynchronous support
  SD::Utils::AsyncBuffer<uint16_t, /*size=*/64 + 1> buffer(master,
                      /* delimiter=*/ std::numeric_limits<uint16_t>::max());

  // Connect UART1 as a destination point for the buffer
  buffer.ConnectDestination(SD::Hardware::UART::UART1,
                            {.device = SD::Hardware::DMADevice::DMA_2,
                             .channel = SD::Hardware::DMAChannel::Channel4,
                             .stream = SD::Hardware::DMAStream::Stream7},
                            uart.GetDMAInSettings());

  // Connect ADC1 as a source point for the buffer
  buffer.ConnectSource(SD::Hardware::ADCDevice::ADC_1,
                       {.device = SD::Hardware::DMADevice::DMA_2,
                        .channel = SD::Hardware::DMAChannel::Channel0,
                        .stream = SD::Hardware::DMAStream::Stream4},
                       adc.GetDMAOutSettings());

  // Set input and output callbacks
  // which will be executed in interruption handlers
  buffer.SetInputCallback([&buffer]{
    buffer.DisableInput();
    buffer.EnableOutput();
  });

  buffer.SetOutputCallback([&buffer, &adc]{
    buffer.DisableOutput();
    adc.Read();
    buffer.EnableInput();
  });

  // Finalize ADC initialization
  adc.Calibrate();
  adc.Read();

  // Enable DMA for the ADC
  buffer.EnableInput();

  // Do something in the main loop while data is processed in the background
  while (true) {
    led.On();
    master.Delay(1000);
    led.Off();
    master.Delay(1000);
  }
}
