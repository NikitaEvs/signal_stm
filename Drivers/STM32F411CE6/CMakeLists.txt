cmake_minimum_required(VERSION 3.16)

set(LL_DRIVERS_DIR STM32F4xx_HAL_Driver)
set(LL_DRIVERS
        ${LL_DRIVERS_DIR}/Src/stm32f4xx_ll_adc.c
        ${LL_DRIVERS_DIR}/Src/stm32f4xx_ll_gpio.c
        ${LL_DRIVERS_DIR}/Src/stm32f4xx_ll_exti.c
        ${LL_DRIVERS_DIR}/Src/stm32f4xx_ll_rcc.c
        ${LL_DRIVERS_DIR}/Src/stm32f4xx_ll_utils.c
        ${LL_DRIVERS_DIR}/Src/stm32f4xx_ll_usart.c
        ${LL_DRIVERS_DIR}/Src/stm32f4xx_ll_dma.c
        )

set(SYSTEM_DIR System)
set(SYSTEM_FILES
        ${SYSTEM_DIR}/system_stm32f4xx.c
        ${SYSTEM_DIR}/startup_stm32f411xe.s
        )

set(LIBRARY Drivers)
add_library(
        ${LIBRARY} STATIC
        ${LL_DRIVERS}
        ${SYSTEM_FILES}
        )

set_target_properties(${LIBRARY} PROPERTIES LINKER_LANGUAGE CXX)

target_compile_definitions(${LIBRARY} PUBLIC
        ${SPECIFIC_COMPILE_DEFINITIONS}
        )

target_include_directories(${LIBRARY} SYSTEM PUBLIC
        STM32F4xx_HAL_Driver/Inc
        CMSIS/Device/ST/STM32F4xx/Include
        CMSIS/Include
        )

target_compile_options(${LIBRARY} PUBLIC
        ${SPECIFIC_COMPILE_OPTIONS}
        )

set_source_files_properties(
        ${LL_DRIVERS}
        PROPERTIES
        COMPILE_FLAGS -Wno-unused-parameter
        )

target_link_options(${LIBRARY} PUBLIC
        -T${CMAKE_SOURCE_DIR}/Drivers/STM32F411CE6/System/STM32F411CEUx_FLASH.ld
        ${SPECIFIC_LINK_OPTIONS}
        )
