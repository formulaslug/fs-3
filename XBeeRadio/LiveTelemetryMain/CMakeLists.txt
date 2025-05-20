cmake_minimum_required(VERSION 3.19)
cmake_policy(VERSION 3.19...3.22)

set(MBED_APP_JSON_PATH mbed_app.json5)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

include(../../mbed-os/tools/cmake/mbed_toolchain_setup.cmake)
project(LiveTelemetryMain
    LANGUAGES C CXX ASM) 
include(mbed_project_setup)

add_subdirectory(../../mbed-os ../../mbed-os-build)

add_library(radio STATIC ../libradio/radio.cpp)
target_link_libraries(radio mbed-os)

add_executable(${PROJECT_NAME} main.cpp)
include_directories(../libradio)
target_link_libraries(${PROJECT_NAME} mbed-os radio) # Can also link to mbed-baremetal here
mbed_set_post_build(${PROJECT_NAME})
