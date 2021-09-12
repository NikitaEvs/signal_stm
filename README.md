# Signal processing library
STM32 library with high-level abstractions for signal processing (WIP).
    
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
This library provide high-level asynchronous abstractions that can be useful in the effective signal measurement and processing.

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