cmake_minimum_required(VERSION 3.16)

include(FetchContent)
# Wavemin lib
FetchContent_Declare(
        wavelib
        GIT_REPOSITORY https://github.com/rafat/wavemin.git
)

message("Fetch wavelib")
FetchContent_MakeAvailable(wavelib)

set(WAVELIB_PATH ${wavelib_SOURCE_DIR} PARENT_SCOPE)

set(INCLUDE_PATH ${wavelib_SOURCE_DIR})
set(SOURCE_PATH ${wavelib_SOURCE_DIR})

set(SOURCES
        ${INCLUDE_PATH}/waveaux.h
        ${INCLUDE_PATH}/wavemin.h
        ${SOURCE_PATH}/waveaux.c
        ${SOURCE_PATH}/wavemin.c
        )

set(LIBRARY wavemin)

add_library(
        ${LIBRARY} STATIC
        ${SOURCES}
)

set_target_properties(wavemin PROPERTIES LINKER_LANGUAGE C)

target_compile_options(${LIBRARY} PUBLIC
        ${SPECIFIC_COMPILE_OPTIONS}
        )

target_link_options(${LIBRARY} PUBLIC
        ${SPECIFIC_LINK_OPTIONS}
        )

set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
