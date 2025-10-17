#
# Mbed CE Hello World Project
#
cmake_minimum_required(VERSION 3.19)
cmake_policy(VERSION 3.19)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Initialize Mbed OS build system.
# Note: This block must be before the project() call.
set(MBED_APP_JSON_PATH mbed_app.json5)
# set(CUSTOM_TARGETS_JSON_PATH custom_targets.json) # If you need a custom target, use this line to specify the custom_targets.json

add_library(onewire-mbed-os STATIC ./onewire-mbed-os/OneWire.cpp)
target_include_directories(onewire-mbed-os PUBLIC ./onewire-mbed-os)
target_link_libraries(onewire-mbed-os mbed-core-flags)

# Configure upload method for ST-LINK
set(UPLOAD_METHOD_DEFAULT STLINK)
set(STLINK_UPLOAD_ENABLED TRUE)
set(STLINK_LOAD_ADDRESS 0x8000000)
set(STLINK_ARGS --connect-under-reset)

include(../mbed-os/tools/cmake/mbed_toolchain_setup.cmake)
project(BMS)
include(mbed_project_setup)

add_subdirectory(../mbed-os mbed-os)

add_subdirectory(lib-mbed-ltc681x)

add_executable(BMS src/main.cpp
        src/BmsThread.h
        src/BmsThread.cpp
        src/Can.h
        src/Can.cpp
        src/BmsConfig.h
        src/EnergusTempSensor.cpp
        src/EnergusTempSensor.h
        src/Event.h
        src/LTC6811.h
        src/LTC6811.cpp
        src/StateOfCharge.cpp
)
target_link_libraries(BMS mbed-os lib-mbed-ltc681x onewire-mbed-os)
mbed_set_post_build(BMS)
