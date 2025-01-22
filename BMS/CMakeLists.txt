#
# Mbed CE Hello World Project
#
cmake_minimum_required(VERSION 3.19)
cmake_policy(VERSION 3.19)

# Initialize Mbed OS build system.
# Note: This block must be before the project() call.
set(MBED_APP_JSON_PATH mbed_app.json5)
# set(CUSTOM_TARGETS_JSON_PATH custom_targets.json) # If you need a custom target, use this line to specify the custom_targets.json

include(mbed-os/tools/cmake/app.cmake) # Load Mbed CE toolchain file and basic build system

# If you need any custom upload method configuration for your target, do that here

# Configure upload method for ST-LINK
set(UPLOAD_METHOD_DEFAULT STLINK)
set(STLINK_UPLOAD_ENABLED TRUE)
set(STLINK_LOAD_ADDRESS 0x8000000)
set(STLINK_ARGS --connect-under-reset)

add_subdirectory(mbed-os) # Load Mbed OS build targets.  Must be added before any other subdirectories
# Tell CMake to build lib-mbed-ltc681x
add_subdirectory(lib-mbed-ltc681x)

project(BMS) # TODO: change this to your project name

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

)
target_link_libraries(BMS 
	mbed-os
	lib-mbed-ltc681x
) # Can also link to mbed-baremetal here
mbed_set_post_build(BMS) # Must call this for each target to set up bin file creation, code upload, etc

mbed_finalize_build() # Make sure this is the last line of the top-level buildscript