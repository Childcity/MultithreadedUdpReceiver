cmake_minimum_required(VERSION 3.10)
project(TcpReceiver)

set(CMAKE_CXX_STANDARD 17)

message("Project Dir is: " ${PROJECT_SOURCE_DIR})
message("Current OS is: " ${CMAKE_SYSTEM_NAME})

if (UNIX)
    set(CMAKE_CXX_FLAGS "-pthread -Wall -Wno-reorder")
endif (UNIX)

add_executable(TcpReceiver
        main.cpp
        ../Common/Utils.hpp
        ../Common/Protocol/Message.cpp
        ../Common/Protocol/Message.h
        ../Common/Protocol/packing.cpp
        ../Common/Protocol/Packet.cpp
        ../Common/Protocol/Packet.h)