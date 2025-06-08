cmake_minimum_required(VERSION 3.19)
cmake_policy(VERSION 3.19...3.22)

set(MBED_APP_JSON_PATH mbed_app.json5)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

include(../mbed-os/tools/cmake/mbed_toolchain_setup.cmake)
project(TelemetryMain
        LANGUAGES C CXX ASM)
include(../mbed-os/tools/cmake/mbed_project_setup.cmake)

add_subdirectory(../mbed-os ../mbed-os-build)

add_library(FATFileSystem STATIC ../FATFileSystem/FATDirHandle.cpp ../FATFileSystem/FATFileHandle.cpp ../FATFileSystem/FATFileSystem.cpp ../FATFileSystem/ChaN/ccsbcs.cpp ../FATFileSystem/ChaN/diskio.cpp ../FATFileSystem/ChaN/ff.cpp)
target_include_directories(FATFileSystem
        PUBLIC ../FATFileSystem
        PUBLIC ../FATFileSystem/ChaN)
target_link_libraries(FATFileSystem PUBLIC mbed-core-flags) # This is needed so that the library can access Mbed includes

add_library(radio STATIC ../XBeeRadio/libradio/src/radio.cpp)
target_include_directories(radio PUBLIC ../XBeeRadio/libradio/src/)
target_link_libraries(radio mbed-os)

add_library(BT817Q STATIC ../Telemetry-Dash1/BT817Q.cpp
        ../Telemetry-Dash1/ui.h
        ../Telemetry-Dash1/layouts.cpp
        ../Telemetry-Dash1/layouts.h)
target_include_directories(BT817Q
        PUBLIC ../Telemetry-Dash1/)
target_link_libraries(FATFileSystem PUBLIC mbed-core-flags) # This is needed so that the library can access Mbed includes
target_link_libraries(BT817Q mbed-os)

add_library(SDFileSystem STATIC ../SDFileSystem/SDFileSystem.cpp)
target_include_directories(SDFileSystem INTERFACE SDFileSystem)
target_link_libraries(SDFileSystem PUBLIC mbed-core-flags) # This is needed so that the library can access Mbed includes
target_link_libraries(SDFileSystem PUBLIC FATFileSystem)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} mbed-os radio FATFileSystem SDFileSystem BT817Q) # Can also link to mbed-baremetal here
mbed_set_post_build(${PROJECT_NAME})
