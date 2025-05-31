cmake_minimum_required(VERSION 3.19)
cmake_policy(VERSION 3.19...3.22)

set(MBED_APP_JSON_PATH mbed_app.json5)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)


include(../mbed-os/tools/cmake/mbed_toolchain_setup.cmake)
project(SDCard
        LANGUAGES C CXX ASM)
include(mbed_project_setup)

add_subdirectory(../mbed-os ../mbed-os-build)

add_library(FATFileSystem STATIC FATFileSystem/FATDirHandle.cpp FATFileSystem/FATFileHandle.cpp FATFileSystem/FATFileSystem.cpp FATFileSystem/ChaN/ccsbcs.cpp FATFileSystem/ChaN/diskio.cpp FATFileSystem/ChaN/ff.cpp)
target_include_directories(FATFileSystem
        PUBLIC FATFileSystem
        PUBLIC FATFileSystem/ChaN)
target_link_libraries(FATFileSystem PUBLIC mbed-core-flags) # This is needed so that the library can access Mbed includes

add_library(SDFileSystem STATIC SDFileSystem/SDFileSystem.cpp)
target_include_directories(SDFileSystem INTERFACE SDFileSystem)
target_link_libraries(SDFileSystem PUBLIC mbed-core-flags) # This is needed so that the library can access Mbed includes
target_link_libraries(SDFileSystem PUBLIC FATFileSystem)

## Fetch Nanoarrow - a minimal implementation of Apache Arrow
include(FetchContent)

FetchContent_Declare(
  nanoarrow
  URL https://github.com/apache/arrow-nanoarrow/releases/download/apache-arrow-nanoarrow-0.6.0/apache-arrow-nanoarrow-0.6.0.tar.gz
  URL_HASH SHA512=25e81c119c16bfa29a9943d3904acd8f29f225ff4a1c99decde7c8235300232402106a4b5249921df773fb797bb0149a885fa3a4524c34c67c06199edd72250c
  DOWNLOAD_EXTRACT_TIMESTAMP ON)
FetchContent_MakeAvailable(nanoarrow)

set(CMAKE_CXX_STANDARD 20)
add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} mbed-os FATFileSystem SDFileSystem nanoarrow)

mbed_set_post_build(${PROJECT_NAME})
