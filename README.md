# Signal Processing library
*STM32 library with high-level abstractions for Signal Processing (WIP).*

[![build](https://github.com/NikitaEvs/signal_stm/actions/workflows/build.yml/badge.svg)](https://github.com/NikitaEvs/signal_stm/actions/workflows/build.yml)
[![docs](https://github.com/NikitaEvs/signal_stm/actions/workflows/docs.yml/badge.svg)](https://github.com/NikitaEvs/signal_stm/actions/workflows/docs.yml)
---    
## Examples
### Led blinking
```cpp
#include "Specific/STM32G103C8/master.hpp"
#include "Utils/led.hpp"

int main() {
    auto& master = SD::Specific::GetMasterInstance();
    master.EnableClocking();
    
    SD::Utils::LED led(master);
    
    while (true) {
        led.On();
        master.Delay(1000);
        led.Off();
        master.Delay(1000);
    }
}
```  
### Hello world!
```cpp
#include "Specific/STM32G103C8/master.hpp"
#include "Utils/logger.hpp"

int main() {
    auto& master = SD::Specific::GetMasterInstance();
    master.EnableClocking();
    
    SD::Utils::Logger logger(master);
    logger.Log("Hello world!");
    
    return 0;
}
```

### Background ADC measurements and data transformation
See [examples](Examples/STM32F411CE6).

## Details
This library provides high-level asynchronous abstractions that can be useful in effective signal measurement and processing.

### Main features
| Name                                         | Description                                                                                                                                                 | Status      |
|----------------------------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------|-------------|
| ```SD::Utils::AsyncBuffer<T, std::size_t>``` | Storage with asynchronous support functions. Main features: chaining with asynchronous source/destination like AsyncUART or ADC, callbacks controlling      | Implemented |
| ```SD::Hardware::ADC<bool>```                | Analog-digital converter abstraction with asynchronous/synchronous versions. Main features: support for the scan mode, chaining with buffers                | Implemented |
| ```SD::Utils::DataProcessor```               | Connector for the data processing between source like ADC and  destination like UART while using `SD::Utils::AsyncBuffer`.                                  | Implemented |
| ```SD::Utils::TransformerFactory```          | Base class for working with `SD::Utils::DataProcessor` and creating own data transformation pipelines.                                                      | Implemented |
| ```SD::Utils::WaveletTransformerFactory```   | Integration of DWT (Discrete Wavelet Transform) pipeline using [wavelib](https://github.com/rafat/wavemin) for working with `SD::Utils::DataProcessor`      | Implemented |
| ```SD::Utils::RealFFTFactory```              | Integration of RFFT (Real Fast Fourier Transform) pipeline using [DSP](https://github.com/ARM-software/CMSIS_5) for working with `SD::Utils::DataProcessor` | In progress |
| ```SD::Hardware::AsyncUART```                | UART abstraction with asynchronous/synchronous versions. Main features: logger abstraction, chaining with buffers                                           | In progress |
### Support devices
| Name      | Core | DWT | RFFT        |
|-----------|------|-----|-------------|
| STM32F103C8T | Yes  | No  | No          |
| STM32F411CE6 | Yes  | Yes | In progress |

Guide for other devices support coming soon.
### Documentation
[Link to the documentation](https://nikitaevs.github.io/signal_stm/)

### Library structure
| Directory              | Description                                |
|------------------------|--------------------------------------------|
| Drivers                | Third-party drivers (CMSIS and LL)         |
| SignalDrivers/Hardware | Hardware abstractions                      |
| SignalDrivers/Specific | Platform-specific functions implementation |
| SignalDrivers/Utils    | Useful abstractions                        |

## Build
### Prerequisites
- An arm-none-eabi toolchain.
- gcc 9.3.0 with stdlib C++20.
- CMake 3.16
### Example compilation and execution
Choose the proper CMake arguments:

| Parameter | Description                                                                                    |
|-----------|------------------------------------------------------------------------------------------------|
| DEVICE    | Name of the microcontroller. (example: STM32F411CE6)                                           |
| DSP       | Build and link CMSIS DSP library. Obligatory for the FFT usage. Support only for STM32F411CE6. |
| WAVELIB   | Build and link wavelib. Obligatory for the DWT usage. Support only for STM32F411CE6.           |
| ARM_CPU   | Specify using CPU. Obligatory only for DSP=ON. (example: Cortex-m4 for STM32F411CE6)           |
| no-dev    | Turn off CMake warnings from DSP library.                                                      |

Firstly, compile the project:
```shell
mkdir build
cd build
cmake -DDEVICE=<name-of-device> -DDSP=ON -DWAVELIB=ON -DARM_CPU=Cortex-m4 -Wno-dev  ..
make
```
In case of success, you see a message like this:
```shell
Memory region         Used Size  Region Size  %age Used
             RAM:        1752 B        20 KB      8.55%
           FLASH:       18472 B        64 KB     28.19%
   text	   data	    bss	    dec	    hex	filename
  18348	    124	   1640	  20112	   4e90	Example.out
```
And you can see ```<name-of-example>.bin```. You can use 
this file for booting with programs like ```st-link``` or ```Open OCD```.
Example for ```st-link```:
```shell
st-flash write <name-of-example>.bin 0x08000000
```
