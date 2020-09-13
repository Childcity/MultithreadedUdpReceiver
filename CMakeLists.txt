cmake_minimum_required(VERSION 3.10)
project(MultithreadedUdpReceiver)

set(CMAKE_CXX_STANDARD 17)

message("Project Dir is: " ${PROJECT_SOURCE_DIR})
message("Current OS is: " ${CMAKE_SYSTEM_NAME})

option(BUILD_UdpSender "Enable building UdpSender" ON)

if(BUILD_UdpSender)
    add_subdirectory(UdpSender)
endif()

#execute_process(COMMAND echo "assssssss")

if (UNIX)
    set(CMAKE_CXX_FLAGS "-pthread -Wall -Wno-reorder")
endif (UNIX)

add_executable(MultithreadedUdpReceiver main.cpp packing.hpp utils.h)