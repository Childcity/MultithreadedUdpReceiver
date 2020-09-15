//
// Created by childcity on 14.09.20.
//

#include "UdpServer.h"

UdpServer::UdpServer(std::string_view address, short port, size_t receiveBufLength)
        : address_(address)
        , port_(port)
        , receiveBufLen_(receiveBufLength)
        , receiveBuf_(std::make_unique<char[]>(receiveBufLength))
{}

void UdpServer::createListener() {
    int rv;
    int yes = 1; // For setsockopt() SO_REUSEADDR, below
    struct addrinfo hints = {};
    struct addrinfo *servInfo, *p;

    ZeroMem(&hints, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(address_.data(), std::to_string(port_).c_str(), &hints, &servInfo)) != 0) {
        throw ServerError(std::string("Error from getaddrinfo: ") + gai_strerror(rv));
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
        if (setsockopt(listenerFd_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1) {
            DEBUG("Error from setsockopt(): " << std::strerror(errno));
            continue;
        }

        // Set to non-blocking)
        if (fcntl(listenerFd_, F_SETFL, O_NONBLOCK) == -1) {
            DEBUG("Error from fcntl(): " << std::strerror(errno));
            continue;
        }

        if (bind(listenerFd_, p->ai_addr, p->ai_addrlen) == -1) {
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
            throw ServerError("Failed to create socket");
        }
    }
}

UdpServer::ReadStatus UdpServer::readSome() {
    ssize_t numBytes;

    // non blocking recv
    if ((numBytes = recvfrom(listenerFd_, receiveBuf_.get(), receiveBufLen_, 0, (struct sockaddr *)&theirAddress_, &theirAddressLen_)) == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return ReadStatus::DataNotExist;
        } else {
            throw ServerError(std::string("Error from recvfrom()") + std::strerror(errno));
        }
    }

    onRead(numBytes);

    return ReadStatus::DataExist;
}
