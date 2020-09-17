//
// Created by childcity on 14.09.20.
//

#include <sys/socket.h>
#include "TcpServer.h"
#include "../Common/Utils.hpp"

using namespace std::string_literals;

TcpServer::TcpServer(std::string_view address, short port)
    : address_(address)
    , port_(port)
{}

void TcpServer::createListener(size_t backLog)
{
    int rv;
    int yes = 1; // For setsockopt() SO_REUSEADDR, below
    struct addrinfo hints = {};
    struct addrinfo *servInfo, *p;

    ZeroMem(&hints, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(address_.data(), std::to_string(port_).c_str(), &hints, &servInfo)) != 0) {
        throw ServerError("Error from getaddrinfo(): "s + gai_strerror(rv));
    }

    DEBUG("\nIP addresses for: " << address_ << "\n"
                                 << Utils::PrintIpAddresses(servInfo));

    // loop through all the results and bind to the first we can
    for (p = servInfo; p != nullptr; p = p->ai_next) {
        listenerFd_ = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (! listenerFd_) {
            DEBUG("Error from socket(): " << std::strerror(errno));
            continue;
        }

        // Lose the pesky "address already in use" error message
        if (-1 == setsockopt(listenerFd_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes)) {
            DEBUG("Error from setsockopt(): " << std::strerror(errno));
            continue;
        }

        if (-1 == bind(listenerFd_, p->ai_addr, p->ai_addrlen)) {
            DEBUG("Error from bind(): " << std::strerror(errno));
            continue;
        }

        break;
    }

    {
        // remember if p is valid to safely delete servInfo
        bool isPInValid = (p == nullptr);

        // we don't need more servInfo
        freeaddrinfo(servInfo);

        if (isPInValid) {
            throw ServerError("Failed to create socket"s + std::strerror(errno));
        }
    }

    if (-1 == listen(listenerFd_, backLog)) {
        throw ServerError("Error from listen(): "s + std::strerror(errno));
    }
}

void TcpServer::acceptClient() {
    Utils::unique_fd clientFd(accept(listenerFd_, (struct sockaddr *)&theirAddress_, &theirAddressLen_));

    if (! clientFd) {
        throw ServerError("Error from accept(): "s + std::strerror(errno));
    }

    onAccept(std::move(clientFd));
}
