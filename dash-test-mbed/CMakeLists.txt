cmake_minimum_required(VERSION 3.19)
cmake_policy(VERSION 3.19...3.22)

set(MBED_APP_JSON_PATH mbed_app.json5)

include(mbed-os/tools/cmake/mbed_toolchain_setup.cmake)
project(dash-mbed # here you can change your project name
    LANGUAGES C CXX ASM) 
include(mbed_project_setup)

add_subdirectory(mbed-os)

add_executable(${PROJECT_NAME} main.cpp Inc/User/main.h)
target_link_libraries(${PROJECT_NAME} mbed-os) # Can also link to mbed-baremetal here
target_sources(${PROJECT_NAME} PRIVATE
        #user sources
        Src/User/bt817/bt817_api.cpp
        Src/User/bt817/bt817_init.cpp
)
target_include_directories(${PROJECT_NAME} PRIVATE
    #user includes
        Inc/User
)
mbed_set_post_build(${PROJECT_NAME})
