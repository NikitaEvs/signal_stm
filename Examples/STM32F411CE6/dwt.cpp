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
#include "Utils/processor.hpp"
#include "Utils/wavelet.hpp"

#include "Hardware/adc.hpp"
#include "Hardware/hardware_layout.hpp"

int main() {
  // Configure peripherals usage
  const auto kUsingUART = SD::Hardware::UART::UART1;
  const auto kUsingADC = SD::Hardware::ADCDevice::ADC_1;

  // Sizes for buffers
  const std::size_t kBufferSize = 2048;
  const std::size_t kDelimiterSize = 1;
  const std::size_t kDWTBufferSize = 2149; // From properties of using DWT

  // Get main supervisor instance and enable default clocking
  auto& master = SD::Specific::GetMasterInstance();
  master.EnableClocking();

  // Create a system LED instance
  SD::Utils::LED led(master);

  // Create the non-blocking UART class
  SD::Hardware::AsyncUART uart(master, kUsingUART);

  // Create the non-blocking ADC class, add one channel and configure it
  SD::Hardware::ADCUnit</*Blocking=*/false> adc(master,
                                                kUsingADC);
  adc.AddChannel({SD::Hardware::GPIO::A, SD::Hardware::Pin::Pin0});
  adc.Init();

  // Create the storage with the asynchronous support
  SD::Utils::AsyncBuffer<uint16_t, /*size=*/kBufferSize + kDelimiterSize>
      buffer(master,
             /*delimiter=*/std::numeric_limits<uint16_t>::max());

  // Create the factory for DWT with parameters
  SD::Utils::WaveletTransformerFactory<uint16_t,
                                      /*sig_size=*/kBufferSize + kDelimiterSize>
      factory(SD::Utils::WaveName::db8,
              /*decomposition_iter=*/7,
              SD::Utils::DWTExtension::Sym,
              /*doAddOriginalSignal=*/true);

  // Configure the data processor with a transformer from the factory
  SD::Utils::DataProcessor<uint16_t,
                          double,
                          kDWTBufferSize + kBufferSize + kDelimiterSize>
      processor(factory.CreateTransformer(),
                /*delimiter=*/std::numeric_limits<double>::infinity());
  buffer.SetDataProcessor(std::move(processor).GetView());

  // Connect UART1 as a destination point for the buffer
  buffer.ConnectDestination(SD::Hardware::UART::UART1,
                            master.GetDMAMapping(kUsingUART, /*isTX=*/true),
                            uart.GetDMAInSettings());

  // Connect ADC1 as a source point for the buffer
  buffer.ConnectSource(SD::Hardware::ADCDevice::ADC_1,
                       master.GetDMAMapping(kUsingADC),
                       adc.GetDMAOutSettings());

  // Set input and output callbacks
  // which will be executed in interruption handlers
  buffer.SetInputCallback([&buffer]{
    buffer.EnableOutput();
  });

  buffer.SetOutputCallback([&buffer, &adc]{
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
