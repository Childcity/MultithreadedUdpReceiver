//
// Created by childcity on 14.09.20.
//

#ifndef MULTITHREADEDUDPRECEIVER_TCPSERVER_H
#define MULTITHREADEDUDPRECEIVER_TCPSERVER_H


#include "../Common/Utils.hpp"
#include "../Common/ServerError.hpp"

#include <sys/fcntl.h>
#include <sys/socket.h>


class TcpServer {
protected:

    TcpServer(std::string_view address, short port);

    TcpServer(const TcpServer &) = delete;

    virtual ~TcpServer() = default;

    void createListener(size_t backLog);

    void acceptClient();

    virtual void onAccept(Utils::unique_fd) = 0;

protected:
    std::string_view address_;
    short port_;

    Utils::unique_fd listenerFd_;

    struct sockaddr_storage theirAddress_ = {};

private:
    socklen_t theirAddressLen_ = sizeof theirAddress_;
};


#endif //MULTITHREADEDUDPRECEIVER_TCPSERVER_H
