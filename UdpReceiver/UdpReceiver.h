//
// Created by childcity on 14.09.20.
//

#ifndef MULTITHREADEDUDPRECEIVER_UDPRECEIVER_H
#define MULTITHREADEDUDPRECEIVER_UDPRECEIVER_H


#include "../Common/UdpServer.h"
#include "../Common/Protocol/Packet.h"

#include <cassert>
#include <string>

class UdpReceiver : public UdpServer {
public:
    UdpReceiver(const std::string_view &address, short port, SafeMessageQueue &queue);

    UdpReceiver(const UdpReceiver &) = delete;

    void listen();

private:
    void onRead(ssize_t numBytes) override;

private:
    SafeMessageQueue &queue_;
};


#endif //MULTITHREADEDUDPRECEIVER_UDPRECEIVER_H
