cmake_minimum_required(VERSION 3.10)
project(MultithreadedUdpReceiver)

set(CMAKE_CXX_STANDARD 17)

message("Project Dir is: " ${PROJECT_SOURCE_DIR})
message("Current OS is: " ${CMAKE_SYSTEM_NAME})

option(BUILD_UdpSender "Enable building UdpSender" ON)
option(BUILD_TcpReceiver "Enable building TcpReceiver" ON)

if(BUILD_UdpSender)
    add_subdirectory(UdpSender)
endif()
if(BUILD_TcpReceiver)
    add_subdirectory(TcpReceiver)
endif()

if (UNIX)
    set(CMAKE_CXX_FLAGS "-pthread -Wall -Wno-reorder")
endif (UNIX)

add_executable(MultithreadedUdpReceiver
        main.cpp
        Common/Protocol/packing.h
        Common/Protocol/packing.cpp
        Common/Protocol/Message.cpp
        Common/Protocol/Message.h
        Common/Protocol/Packet.cpp
        Common/Protocol/Packet.h
        Common/ThreadSafeList.hpp
        Common/Utils.hpp
        Common/ServerError.hpp
        UdpReceiver/UdpServer.h
        UdpReceiver/UdpServer.cpp
        TcpSender/TcpServer.h
        TcpSender/TcpServer.cpp
        UdpReceiver/UdpReceiver.cpp
        UdpReceiver/UdpReceiver.h
        TcpSender/TcpSender.cpp
        TcpSender/TcpSender.h)