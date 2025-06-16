cmake_minimum_required(VERSION 3.19)
cmake_policy(VERSION 3.19...3.22)

set(MBED_APP_JSON_PATH mbed_app.json5)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)


# Main project definition
include(../mbed-os/tools/cmake/mbed_toolchain_setup.cmake)
project(SDCard LANGUAGES C CXX ASM)
include(mbed_project_setup)

# Include MBED OS
add_subdirectory(../mbed-os ../mbed-os-build)

# Fetch Nanoarrow - a minimal implementation of Apache Arrow
include(FetchContent)
# This enables the nanoarrow IPC extension (required to use Arrow IPC functions)
# (note: the nanoarrow_ipc library is created conditionally based on this flag)
set(NANOARROW_IPC ON CACHE BOOL "Build IPC extension")
FetchContent_Declare(
  nanoarrow
  URL https://github.com/apache/arrow-nanoarrow/releases/download/apache-arrow-nanoarrow-0.6.0/apache-arrow-nanoarrow-0.6.0.tar.gz
  URL_HASH SHA512=25e81c119c16bfa29a9943d3904acd8f29f225ff4a1c99decde7c8235300232402106a4b5249921df773fb797bb0149a885fa3a4524c34c67c06199edd72250c
  DOWNLOAD_EXTRACT_TIMESTAMP ON
)
FetchContent_MakeAvailable(nanoarrow)


# Add our main executable, and link all the above libraries to it
set(CMAKE_CXX_STANDARD 20)
add_executable(${PROJECT_NAME} main.cpp)

# "SYSTEM" should remove warnings when compiling the generated flatbuffers code
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC include)
target_link_libraries(${PROJECT_NAME} mbed-os mbed-storage-sd mbed-storage-fat nanoarrow nanoarrow_ipc)
# target_link_libraries(${PROJECT_NAME} mbed-baremetal mbed-storage-sd mbed-storage-fat)
# target_link_libraries(${PROJECT_NAME} mbed-baremetal mbed-storage-sd mbed-storage-fat nanoarrow nanoarrow_ipc)

mbed_set_post_build(${PROJECT_NAME})
