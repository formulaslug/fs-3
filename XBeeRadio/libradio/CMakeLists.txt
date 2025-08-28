cmake_minimum_required(VERSION 3.19)

project(libradio LANGUAGES C CXX ASM)

add_library(libradio STATIC src/radio.cpp src/packetize.cpp)

target_link_libraries(libradio mbed-core-flags)

target_include_directories(libradio PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

