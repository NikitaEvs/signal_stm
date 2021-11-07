cmake_minimum_required(VERSION 3.16)

set(LL_DRIVERS_DIR STM32F1xx_HAL_Driver)
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

set(SYSTEM_DIR System)
set(SYSTEM_FILES
        ${SYSTEM_DIR}/system_stm32f1xx.c
        ${SYSTEM_DIR}/startup_stm32f103xb.s
        )

set(LIBRARY Drivers)
add_library(
        ${LIBRARY} STATIC
        ${LL_DRIVERS}
        ${SYSTEM_FILES}
        )

set_target_properties(${LIBRARY} PROPERTIES LINKER_LANGUAGE CXX)

target_compile_definitions(${LIBRARY} PUBLIC
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

target_include_directories(${LIBRARY} SYSTEM PUBLIC
        STM32F1xx_HAL_Driver/Inc
        CMSIS/Device/ST/STM32F1xx/Include
        CMSIS/Include
        )

target_compile_options(${LIBRARY} PUBLIC
        -mcpu=cortex-m3
        -mthumb
        )

set_source_files_properties(
        ${LL_DRIVERS}
        PROPERTIES
        COMPILE_FLAGS -Wno-unused-parameter
        )

target_link_options(${LIBRARY} PUBLIC
        -T${CMAKE_SOURCE_DIR}/Drivers/STM32F103C8T/System/STM32F103C8Tx_FLASH.ld
        -mcpu=cortex-m3
        -mthumb
        -specs=nano.specs
        -specs=nosys.specs
        )
