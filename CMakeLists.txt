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

add_executable(MultithreadedUdpReceiver
        main.cpp
        Common/packing.h
        Common/Utils.hpp
        Common/ServerError.hpp
        Common/UdpServer.h
        Common/UdpServer.cpp
        UdpReceiver/UdpReceiver.cpp
        UdpReceiver/UdpReceiver.h
        Common/Message.cpp
        Common/Message.h Common/packing.cpp)