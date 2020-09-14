cmake_minimum_required(VERSION 3.10)
project(UdpSender)

set(CMAKE_CXX_STANDARD 17)

message("Project Dir is: " ${PROJECT_SOURCE_DIR})
message("Current OS is: " ${CMAKE_SYSTEM_NAME})

if (UNIX)
    set(CMAKE_CXX_FLAGS "-pthread -Wall -Wno-reorder")
endif (UNIX)

add_executable(UdpSender main.cpp ../Common/Utils.hpp ../Common/Message.cpp ../Common/Message.h ../Common/packing.cpp)