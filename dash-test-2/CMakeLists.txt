cmake_minimum_required(VERSION 3.19)
cmake_policy(VERSION 3.19...3.22)

set(MBED_APP_JSON_PATH mbed_app.json5)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

include(../mbed-os/tools/cmake/mbed_toolchain_setup.cmake)
project(TelemetryMain
        LANGUAGES C CXX ASM)
include(../mbed-os/tools/cmake/mbed_project_setup.cmake)

add_subdirectory(../mbed-os ../mbed-os-build)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(../XBeeRadio/libradio libradio)

add_library(dash STATIC ./dash/BT817Q.cpp ./dash/layouts.cpp)
target_include_directories(dash PUBLIC ./dash)
target_link_libraries(dash mbed-core-flags)


add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} mbed-os dash)
mbed_set_post_build(${PROJECT_NAME})
