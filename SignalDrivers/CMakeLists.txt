cmake_minimum_required(VERSION 3.16)

set(HARDWARE_PATH Hardware)

set(HARDWARE_SOURCES
        ${HARDWARE_PATH}/port.hpp
        ${HARDWARE_PATH}/ioport.hpp
        ${HARDWARE_PATH}/abstract_master.hpp
        ${HARDWARE_PATH}/hardware_layout.hpp
        ${HARDWARE_PATH}/uart.hpp
        ${HARDWARE_PATH}/adc.hpp
        ${HARDWARE_PATH}/dma.hpp
        )

set(SPECIFIC_PATH Specific/${DEVICE})

set(SPECIFIC_SOURCES
        ${SPECIFIC_PATH}/master.hpp
        ${SPECIFIC_PATH}/hardware_mapping.hpp
        )

set(UTIL_PATH Utils)

set(UTIL_SOURCES
        ${UTIL_PATH}/buffer.hpp
        ${UTIL_PATH}/led.hpp
        ${UTIL_PATH}/logger.hpp
        )

add_library(
        SignalDrivers STATIC
        ${HARDWARE_SOURCES}
        ${SPECIFIC_SOURCES}
        ${UTIL_SOURCES}
        )

set_target_properties(SignalDrivers PROPERTIES LINKER_LANGUAGE CXX)

set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)