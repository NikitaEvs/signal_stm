cmake_minimum_required(VERSION 3.16)

project(Example)

enable_language(CXX C ASM)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(LL_DRIVERS_DIR Drivers/STM32F1xx_HAL_Driver)
set(LL_DRIVERS
        ${LL_DRIVERS_DIR}/Src/stm32f1xx_ll_adc.c
        ${LL_DRIVERS_DIR}/Src/stm32f1xx_ll_gpio.c
        ${LL_DRIVERS_DIR}/Src/stm32f1xx_ll_pwr.c
        ${LL_DRIVERS_DIR}/Src/stm32f1xx_ll_exti.c
        ${LL_DRIVERS_DIR}/Src/stm32f1xx_ll_rcc.c
        ${LL_DRIVERS_DIR}/Src/stm32f1xx_ll_utils.c
        ${LL_DRIVERS_DIR}/Src/stm32f1xx_ll_usart.c
        ${LL_DRIVERS_DIR}/Src/stm32f1xx_ll_dma.c
        )

set(SYSTEM_DIR Drivers/System)
set(SYSTEM_FILES
        ${SYSTEM_DIR}/system_stm32f1xx.c
        ${SYSTEM_DIR}/startup_stm32f103xb.s
        )

set(SOURCES
        ${LL_DRIVERS}
        ${SYSTEM_FILES}

        Src/example.cpp
        )

add_subdirectory(SignalDrivers)

set(EXECUTABLE ${PROJECT_NAME}.out)
add_executable(${EXECUTABLE} ${SOURCES})
target_link_libraries(${EXECUTABLE} SignalDrivers)
set_target_properties(${EXECUTABLE} PROPERTIES LINKER_LANGUAGE CXX)

target_compile_definitions(${EXECUTABLE} PRIVATE
        -DUSE_FULL_LL_DRIVER
        -DHSE_VALUE=8000000
        -DHSE_STARTUP_TIMEOUT=100
        -DLSE_STARTUP_TIMEOUT=5000
        -DLSE_VALUE=32768
        -DHSI_VALUE=8000000
        -DLSI_VALUE=40000
        -DVDD_VALUE=3300
        -DPREFETCH_ENABLE=1
        -DSTM32F103xB
        )

target_include_directories(${EXECUTABLE} SYSTEM PRIVATE
        SignalDrivers
        Drivers/STM32F1xx_HAL_Driver/Inc
        Drivers/CMSIS/Device/ST/STM32F1xx/Include
        Drivers/CMSIS/Include
        )

target_include_directories(${EXECUTABLE} PRIVATE
        SignalDrivers
        )

target_compile_options(${EXECUTABLE} PRIVATE
        -mcpu=cortex-m3
        -mthumb

        -fdata-sections
        -ffunction-sections

        $<$<COMPILE_LANGUAGE:CXX>:@${CMAKE_SOURCE_DIR}/CMake/gcc-options-cxx.txt>

        -Wall
        -Wextra
        -pedantic
        -Werror

        $<$<CONFIG:Debug>:-Og>
        )

set_source_files_properties(
        ${LL_DRIVERS}
        PROPERTIES
        COMPILE_FLAGS -Wno-unused-parameter)

set_target_properties(
        ${EXECUTABLE} PROPERTIES ADDITIONAL_CLEAN_FILES
        "${PROJECT_NAME}.bin;${PROJECT_NAME}.hex;${PROJECT_NAME}.map;CMakeCache.txt")

target_link_options(${EXECUTABLE} PRIVATE
        -T${CMAKE_SOURCE_DIR}/Drivers/System/STM32F103C8Tx_FLASH.ld
        -mcpu=cortex-m3
        -mthumb
        -specs=nano.specs
        -specs=nosys.specs
        -lc
        -lm
        -lnosys
        -Wl,-Map=${PROJECT_NAME}.map,--cref
        -Wl,--gc-sections
        -Wl,--print-memory-usage
        )

# Print executable size
add_custom_command(TARGET ${EXECUTABLE}
        POST_BUILD
        COMMAND arm-none-eabi-size ${EXECUTABLE}
        )

# Create hex file
add_custom_command(TARGET ${EXECUTABLE}
        POST_BUILD
        COMMAND arm-none-eabi-objcopy -O ihex ${EXECUTABLE} ${PROJECT_NAME}.hex
        COMMAND arm-none-eabi-objcopy -O binary ${EXECUTABLE} ${PROJECT_NAME}.bin
        )

