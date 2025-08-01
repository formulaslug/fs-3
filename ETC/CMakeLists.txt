cmake_minimum_required(VERSION 3.19)
cmake_policy(VERSION 3.19)

# Define the MBed app configuration. This must be before the `project()` call.
set(MBED_APP_JSON_PATH mbed_app.json5)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

include(../mbed-os/tools/cmake/mbed_toolchain_setup.cmake)
project(ETC)
include(mbed_project_setup)

# Configure upload method for ST-LINK
set(UPLOAD_METHOD_DEFAULT STLINK)
set(STLINK_UPLOAD_ENABLED TRUE)
set(STLINK_LOAD_ADDRESS 0x8000000)
set(STLINK_ARGS --connect-under-reset)

add_subdirectory(../mbed-os ../mbed-os-build)

# Main ETC executable target
set(SRC_CPP_FILES src/can_wrapper.cpp src/etc_controller.cpp)
add_executable(ETC main.cpp ${SRC_CPP_FILES})
target_include_directories(ETC PRIVATE mbed-os)
target_link_libraries(ETC mbed-os)
mbed_set_post_build(ETC)


# ETC unit tests target
include(FetchContent)
FetchContent_Declare(
  unity
  GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
  GIT_TAG master
)
FetchContent_MakeAvailable(unity)

add_executable(ETC-unittests tests/test_main.cpp ${SRC_CPP_FILES})
target_include_directories(ETC-unittests PRIVATE
  src
  tests
  mbed-os
  ${unity_SOURCE_DIR}/src
)
target_link_libraries(ETC-unittests mbed-os unity)
mbed_set_post_build(ETC-unittests)

