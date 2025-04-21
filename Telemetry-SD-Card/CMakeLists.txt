cmake_minimum_required(VERSION 3.19)
cmake_policy(VERSION 3.19...3.22)

set(MBED_APP_JSON_PATH mbed_app.json5)

include(mbed-os/tools/cmake/mbed_toolchain_setup.cmake)
project(MyMbedApp # here you can change your project name
        LANGUAGES C CXX ASM)
include(mbed_project_setup)

add_subdirectory(mbed-os)

add_library(FATFileSystem STATIC FATFileSystem/FATDirHandle.cpp FATFileSystem/FATFileHandle.cpp FATFileSystem/FATFileSystem.cpp FATFileSystem/ChaN/ccsbcs.cpp FATFileSystem/ChaN/diskio.cpp FATFileSystem/ChaN/ff.cpp)
target_include_directories(FATFileSystem
        INTERFACE FATFileSystem
        PUBLIC FATFileSystem/ChaN
        )
target_link_libraries(FATFileSystem PUBLIC mbed-core-flags) # This is needed so that the library can access Mbed includes

add_library(SDFileSystem STATIC SDFileSystem/SDFileSystem.cpp)
target_include_directories(SDFileSystem INTERFACE SDFileSystem)
target_link_libraries(SDFileSystem PUBLIC mbed-core-flags) # This is needed so that the library can access Mbed includes
target_link_libraries(SDFileSystem PUBLIC FATFileSystem)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} mbed-os FATFileSystem SDFileSystem) # Can also link to mbed-baremetal here
mbed_set_post_build(${PROJECT_NAME})