cmake_minimum_required(VERSION 3.19)
project(libradio)

add_library(libradio STATIC
        src/radio.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/radio.hpp)

target_include_directories(libradio PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(libradio mbed-os)
