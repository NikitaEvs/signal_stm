cmake_minimum_required(VERSION 3.16)

project(Example)

enable_language(CXX C ASM)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(Drivers)
add_subdirectory(SignalDrivers)

set(SOURCES
        Src/example.cpp
        )
set(EXECUTABLE ${PROJECT_NAME}.out)
add_executable(${EXECUTABLE} ${SOURCES})

target_link_libraries(${EXECUTABLE} Drivers)
target_link_libraries(${EXECUTABLE} SignalDrivers)
set_target_properties(${EXECUTABLE} PROPERTIES LINKER_LANGUAGE CXX)


target_include_directories(${EXECUTABLE} SYSTEM PUBLIC
        SignalDrivers
        )

target_compile_options(${EXECUTABLE} PRIVATE
        -fdata-sections
        -ffunction-sections

        $<$<COMPILE_LANGUAGE:CXX>:@${CMAKE_SOURCE_DIR}/CMake/gcc-options-cxx.txt>

        -Wall
        -Wextra
        -pedantic
        -Werror

        $<$<CONFIG:Debug>:-Og>
        )

set_target_properties(
        ${EXECUTABLE} PROPERTIES ADDITIONAL_CLEAN_FILES
        "${PROJECT_NAME}.bin;${PROJECT_NAME}.hex;${PROJECT_NAME}.map;CMakeCache.txt"
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
        COMMAND arm-none-eabi-objcopy -O ihex ${EXECUTABLE} ${PROJECT_NAME}.hex
        COMMAND arm-none-eabi-objcopy -O binary ${EXECUTABLE} ${PROJECT_NAME}.bin
        )

