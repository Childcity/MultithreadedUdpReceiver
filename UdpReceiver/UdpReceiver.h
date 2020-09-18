//
// Created by childcity on 14.09.20.
//

#ifndef MULTITHREADEDUDPRECEIVER_UDPRECEIVER_H
#define MULTITHREADEDUDPRECEIVER_UDPRECEIVER_H


#include "UdpServer.h"
#include "../Common/Protocol/Packet.h"

#include <cassert>
#include <string>

class UdpReceiver final : public UdpServer {
public:
    UdpReceiver(const std::string_view &address, short port, SafeMessageList &list);

    UdpReceiver(const UdpReceiver &) = delete;

    void listen();

private:
    void onRead(ssize_t numBytes) override;

private:
    SafeMessageList &list_;
};


#endif //MULTITHREADEDUDPRECEIVER_UDPRECEIVER_H
