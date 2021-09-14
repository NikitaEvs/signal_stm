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

### Background ADC measurements
See [example.cpp](Src/example.cpp).

## Details
This library provides high-level asynchronous abstractions that can be useful in effective signal measurement and processing.

### Main features
| Name                                   | Description                                                                                                                                               | Status      |
|----------------------------------------|-----------------------------------------------------------------------------------------------------------------------------------------------------------|-------------|
| ```SD::Utils::AsyncBuffer<T, std::size_t>``` | Storage with asynchronous support functions. Main features: chaining with asynchronous source/destination like AsyncUART or ADC, callbacks controlling | Implemented |
| ```SD::Hardware::ADC<bool>```                | Analog-digital converter abstraction with asynchronous/synchronous versions. Main features: support for the scan mode, chaining with buffers           | Implemented |
| ```SD::Hardware::AsyncUART```                | UART abstraction with asynchronous/synchronous versions. Main features: logger abstraction, chaining with buffers                                      | In progress |
| ```SD::Utils::DataPipe```                    | Asynchronous connector for the data processing between source like ADC and  destination like UART.                                                        | Planning    |
### Support devices
| Name      | Status   |
|-----------|----------|
| STM32F103 | Support  |
| STM32F411 | Planning |

Guide for other devices support coming soon.
### Documentation
[Link to the documentation](https://nikitaevs.github.io/signal_stm/)

## Build
### Prerequisites
- An arm-none-eabi toolchain.
- gcc 9.3.0 with stdlib C++20.
### Example compilation and execution
Firstly, compile the project:
```shell
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../CMake/arm-none-eabi-gcc.cmake ..
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
And you can see ```Example.bin```. You can use 
this file for booting with programs like ```st-link``` or ```Open OCD```.
Example for ```st-link```:
```shell
st-flash write Example.bin 0x08000000
```
