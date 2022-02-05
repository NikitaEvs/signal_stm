cmake_minimum_required(VERSION 3.16)

set(SPECIFIC_COMPILE_DEFINITIONS
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

set(SPECIFIC_COMPILE_OPTIONS
        -mcpu=cortex-m3
        -mthumb
        )

set(SPECIFIC_LINK_OPTIONS
        -mcpu=cortex-m3
        -mthumb
        -specs=nano.specs
        -specs=nosys.specs
        )
