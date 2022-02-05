cmake_minimum_required(VERSION 3.16)

# Select toolchain
if(DSP)
        # DSP toolchain
        set(CMAKE_TOOLCHAIN_FILE ../Drivers/${DEVICE}/CMSIS/DSP/gcc.cmake)
else()
        # Simple toolchain
        set(CMAKE_TOOLCHAIN_FILE ../CMake/arm-none-eabi-gcc.cmake)
endif()

# Include hardware-specific compile flags
include(Drivers/${DEVICE}/CMake/specific.cmake)

project(Examples)

enable_language(CXX C ASM)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build additional libraries

# DSP library (optional)
if(DSP)
        message("Compile with DSP library")
        set(ROOT ${CMAKE_SOURCE_DIR}/Drivers/${DEVICE})
        set(DSP ${ROOT}/CMSIS/DSP)
        list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${DSP}")
        list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${DSP}/Source")
        add_subdirectory(${DSP}/Source bin_dsp)

        # Take care about other cortex processors
        if(ARM_CPU STREQUAL "Cortex-m4")
                message("Using Cortex-m4 definition")
                add_compile_definitions(ARM_MATH_CM4)
        endif()
endif()

# Wavemin library (optional)
if(WAVELIB)
        message("Compile with wavelib")
        add_subdirectory(ThirdParty/wavemin)
endif()

# Hard drivers library
add_subdirectory(Drivers)

# High-level SignalDrivers library
add_subdirectory(SignalDrivers)

set(STM32F103C8T_examples adc)
set(STM32F411CE6_examples adc dwt)

foreach(EXAMPLE IN LISTS ${DEVICE}_examples)
        # Main executable
        set(SOURCES
                Examples/${DEVICE}/${EXAMPLE}.cpp
                )
        set(EXECUTABLE ${EXAMPLE})
        add_executable(${EXECUTABLE} ${SOURCES})

        if(DSP)
                target_link_libraries(${EXECUTABLE} CMSISDSP)
                target_include_directories(${EXECUTABLE} SYSTEM PUBLIC
                        ThirdParty/CMSIS/DSP/Include
                        ThirdParty/CMSIS/DSP/Include/dsp
                        )
        endif()

        if(WAVELIB)
                target_link_libraries(${EXECUTABLE} wavemin)
                target_include_directories(${EXECUTABLE} SYSTEM PUBLIC
                        ThirdParty/wavemin
                        )
        endif()

        target_link_libraries(${EXECUTABLE} Drivers)

        target_link_libraries(${EXECUTABLE} SignalDrivers)
        target_include_directories(${EXECUTABLE} SYSTEM PUBLIC
                SignalDrivers
                )

        # Properties
        set_target_properties(
                ${EXECUTABLE} PROPERTIES
                ADDITIONAL_CLEAN_FILES "
                        ${EXAMPLE}.bin;
                        ${EXAMPLE}.hex;
                        ${EXAMPLE}.map;
                        CMakeCache.txt
                "
                LINKER_LANGUAGE CXX
        )
        set_target_properties(
                ${EXECUTABLE} PROPERTIES
                LINKER_LANGUAGE CXX
        )

        target_compile_options(${EXECUTABLE} PRIVATE
                -fdata-sections
                -ffunction-sections

                $<$<NOT:$<STREQUAL:$<TARGET_PROPERTY:SOURCE_DIR>,${DSP}/Source>>:
                        ${CMAKE_SOURCE_DIR}/CMake/gcc-options-cxx.txt>

                -Wall
                -Wextra
                -pedantic

                $<$<CONFIG:Debug>:-Og>
                )

        target_link_options(${EXECUTABLE} PRIVATE
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
                COMMAND arm-none-eabi-objcopy -O ihex ${EXECUTABLE} ${EXAMPLE}.hex
                COMMAND arm-none-eabi-objcopy -O binary ${EXECUTABLE} ${EXAMPLE}.bin
                )
endforeach()
