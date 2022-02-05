/**
 * @brief Example of the library usage with led blinking
 * while data is processed in the background
 */

// Choose the using driver (the corresponding DEVICE variable
// should be set as the CMake argument)
#include "Specific/STM32F103C8T/master.hpp"

#include "Utils/buffer.hpp"
#include "Utils/led.hpp"
#include "Utils/logger.hpp"

#include "Hardware/adc.hpp"
#include "Hardware/hardware_layout.hpp"


int main() {
  // Configure peripherals usage
  const auto kUsingUART = SD::Hardware::UART::UART1;
  const auto kUsingADC = SD::Hardware::ADCDevice::ADC_1;

  // Get main supervisor instance and enable default clocking
  auto& master = SD::Specific::GetMasterInstance();
  master.EnableClocking();

  // Create a system LED instance
  SD::Utils::LED led(master);

  // Create non-blocking UART class
  SD::Hardware::AsyncUART uart(master, kUsingUART);

  // Create non-blocking ADC class, add one channel and configure it
  SD::Hardware::ADCUnit</*Blocking=*/false> adc(master,
                                                kUsingADC);
  adc.AddChannel({SD::Hardware::GPIO::A, SD::Hardware::Pin::Pin1});
  adc.Init();

  // Create storage with the asynchronous support
  SD::Utils::AsyncBuffer<uint16_t, /*size=*/16> buffer(master);

  // Connect UART1 as a destination point for the buffer
  buffer.ConnectDestination(kUsingUART,
                            master.GetDMAMapping(kUsingUART, /*isTX=*/true),
                            uart.GetDMAInSettings());

  // Connect ADC1 as a source point for the buffer
  buffer.ConnectSource(kUsingADC,
                       master.GetDMAMapping(kUsingADC),
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
