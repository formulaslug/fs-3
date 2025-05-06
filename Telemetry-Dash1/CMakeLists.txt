cmake_minimum_required(VERSION 3.19)
cmake_policy(VERSION 3.19...3.22)

set(MBED_APP_JSON_PATH mbed_app.json5)
set(MBED_TARGET NUCLEO_F446RE)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

#execute_process(COMMAND sh -c "asdf which python" OUTPUT_VARIABLE PYTHON_PATH OUTPUT_STRIP_TRAILING_WHITESPACE)
#set(Python3_EXECUTABLE ${PYTHON_PATH} CACHE PATH "" FORCE)

include(../mbed-os/tools/cmake/mbed_toolchain_setup.cmake)
project(fs-3-dash LANGUAGES C CXX ASM)
include(../mbed-os/tools/cmake/mbed_project_setup.cmake)

add_subdirectory(../mbed-os ../mbed-os-build)

add_library(BT817Q STATIC BT817Q.cpp
        ui.h)
target_link_libraries(BT817Q mbed-os)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} BT817Q mbed-os) # Can also link to mbed-baremetal here
mbed_set_post_build(${PROJECT_NAME})
