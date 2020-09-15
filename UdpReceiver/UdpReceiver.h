//
// Created by childcity on 14.09.20.
//

#ifndef MULTITHREADEDUDPRECEIVER_UDPRECEIVER_H
#define MULTITHREADEDUDPRECEIVER_UDPRECEIVER_H


#include "../Common/UdpServer.h"

class UdpReceiver : public UdpServer {
public:
    UdpReceiver(const std::string_view &address, short port)
        : UdpServer(address, port, 256)
    {}
};


#endif //MULTITHREADEDUDPRECEIVER_UDPRECEIVER_H
