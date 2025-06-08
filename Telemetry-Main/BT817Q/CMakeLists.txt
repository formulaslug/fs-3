cmake_minimum_required(VERSION 3.19)

project(bt817q LANGUAGES C CXX ASM)

add_library(bt817q STATIC BT817Q.cpp layouts.cpp)

target_link_libraries(bt817q mbed-core-flags)

target_include_directories(bt817q PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
