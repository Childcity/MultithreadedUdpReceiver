//
// Created by childcity on 14.09.20.
//

#ifndef MULTITHREADEDUDPRECEIVER_TCPSENDER_H
#define MULTITHREADEDUDPRECEIVER_TCPSENDER_H


#include "TcpServer.h"
#include "../Common/Protocol/Packet.h"

#include <cassert>
#include <string>

class TcpSender final : public TcpServer {
public:
    enum class ReadStatus
    {
        DataExist,
        DataNotExist
    };

    TcpSender(const std::string_view &address, short port, SafeMessageList &list);

    TcpSender(const TcpSender &) = delete;

    void listen();

private:
    void onAccept(Utils::unique_fd) override;

    ReadStatus readSome(int clientFd);

    void onRead(ssize_t numBytes);

    void setClientToNonBlocking(int clientFd);

private:
    SafeMessageList &list_;

    size_t receiveBufLen_;
    std::unique_ptr<char[]> receiveBuf_;
};


#endif //MULTITHREADEDUDPRECEIVER_TCPSENDER_H
