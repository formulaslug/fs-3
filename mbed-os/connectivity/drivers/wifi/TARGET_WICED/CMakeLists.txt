# Copyright (c) 2020-2021 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

add_library(mbed-wiced STATIC EXCLUDE_FROM_ALL)

if(${CMAKE_CROSSCOMPILING})
    if(${MBED_TOOLCHAIN} STREQUAL "ARM")
        set(LIB_WICED_DRIVERS TOOLCHAIN_ARMC6/TARGET_WIO_EMW3166/libwiced_drivers.ar)
    elseif(${MBED_TOOLCHAIN} STREQUAL "GCC_ARM")
        set(LIB_WICED_DRIVERS TOOLCHAIN_GCC_ARM/TARGET_WIO_EMW3166/libwiced_drivers.a)
    endif()

    target_link_libraries(mbed-wiced INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/${LIB_WICED_DRIVERS})
endif()

target_include_directories(mbed-wiced
    PUBLIC
        wiced_interface
)

target_sources(mbed-wiced
    PRIVATE
        wiced_interface/default_wifi_interface.cpp
)

target_link_libraries(mbed-wifi PUBLIC mbed-wiced)

# Link override object file coming from static library anyway
#
# NOTE: This linker option is to pretend undefined symbol and won't cause
#       undefined symbol error even though the override object file actually
#       doesn't provide such symbol definition.
if(${MBED_TOOLCHAIN} STREQUAL "GCC_ARM")
    target_link_options(mbed-wifi
        INTERFACE
            LINKER:--undefined=LINK_DEFAULT_WIFI_INTERFACE_CPP
    )
endif()
