cmake_minimum_required(VERSION 3.19)
cmake_policy(VERSION 3.19...3.22)

set(MBED_APP_JSON_PATH mbed_app.json5)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

include(../mbed-os/tools/cmake/mbed_toolchain_setup.cmake)
project(TelemetryMain
        LANGUAGES C CXX ASM)
include(../mbed-os/tools/cmake/mbed_project_setup.cmake)

add_subdirectory(../mbed-os ../mbed-os-build)

add_library(libradio STATIC ../XBeeRadio/libradio/src/radio.cpp)
target_include_directories(libradio PUBLIC ../XBeeRadio/libradio/src/)
target_link_libraries(libradio mbed-os)

add_library(BT817Q STATIC ./BT817Q/BT817Q.cpp ./BT817Q/layouts.cpp)
target_include_directories(BT817Q PUBLIC ./BT817Q)
target_link_libraries(BT817Q mbed-core-flags)

set(CMAKE_CXX_STANDARD 20)
add_executable(${PROJECT_NAME} main.cpp VehicleStateManager.cpp)
target_link_libraries(${PROJECT_NAME} mbed-os libradio mbed-storage-sd mbed-storage-fat BT817Q)
mbed_set_post_build(${PROJECT_NAME})
