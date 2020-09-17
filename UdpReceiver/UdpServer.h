//
// Created by childcity on 14.09.20.
//

#ifndef MULTITHREADEDUDPRECEIVER_UDPSERVER_H
#define MULTITHREADEDUDPRECEIVER_UDPSERVER_H


#include "../Common/Utils.hpp"
#include "../Common/ServerError.hpp"

#include <sys/fcntl.h>


class UdpServer {
protected:
    enum class ReadStatus
    {
        DataExist,
        DataNotExist
    };

    UdpServer(std::string_view address, short port, size_t receiveBufLength);

    UdpServer(const UdpServer &) = delete;

    virtual ~UdpServer() = default;

    void createListener();

    ReadStatus readSome();

    virtual void onRead(ssize_t numBytes) = 0;

protected:
    std::string_view address_;
    short port_;

    Utils::unique_fd listenerFd_;

    size_t receiveBufLen_;
    std::unique_ptr<char[]> receiveBuf_;

    struct sockaddr_storage theirAddress_ = {};

private:
    socklen_t theirAddressLen_ = sizeof theirAddress_;
};


#endif //MULTITHREADEDUDPRECEIVER_UDPSERVER_H
