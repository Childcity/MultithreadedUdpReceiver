//
// Created by childcity on 14.09.20.
//

#include "TcpSender.h"

#include <fcntl.h>
#include <cstdint>
#include <list>

using namespace std::string_literals;

TcpSender::TcpSender(const std::string_view &address, short port, SafeMessageList &list)
    : TcpServer(address, port)
    , list_(list)
    , receiveBufLen_(64)
    , receiveBuf_(std::make_unique<char[]>(receiveBufLen_))
{}

void TcpSender::listen()
{
    createListener(1); // we support only 1 client

    DEBUG("Server listening on: " << address_ << ":" << port_);
    DEBUG("Server waiting for connections...");

    for (;;) {
        try {
            acceptClient();
        } catch (ServerError &err) {
            DEBUG("Error acceptClient(): " << err.what());
        }
    }
}

void TcpSender::onAccept(Utils::unique_fd clientFd)
{
    setClientToNonBlocking(clientFd);

    char ipStr[INET6_ADDRSTRLEN];
    inet_ntop(theirAddress_.ss_family, Utils::GetInAddr(&theirAddress_), ipStr, sizeof ipStr);

    DEBUG("Server got connection from: " << ipStr)

    try {
        // We must remember all sent Message Ids,
        // because this condition is in Task description
        // (MesssageId в сообщениях может дублироваться на любом receiver потоке, дубликаты пропускать.)
        std::list<uint64_t> sentIds; // TODO: rewrite without STL. I'm sorry about this std::list
        Packet packet;

        // Next loop will be active, while connection with client is not closed.
        // We just skip received data and close connection, if client send something.
        // For demo app I think this is Ok.
        while (readSome(clientFd) == ReadStatus::DataNotExist) {

            // Send messages with MessageId == 10 to the client socket
            list_.forEach([&](Message &msg) {
                if (msg.MessageData == 10ull) {
                    packet.setMessage(msg);
                    if (-1 == Utils::SendAll(clientFd, packet.data(), Packet::PacketSize)) {
                        throw ServerError("Error in SendAll: "s + strerror(errno));
                    }

                    sentIds.emplace_back(msg.MessageId);
                }
            });

            // Removing from list_ all messages, that we have been sent
            for (const auto &msgId : sentIds) {
                list_.removeIf([&](const Message &msg) {
                    //if(msgId == msg.MessageId) DEBUG("removed: " << msg.MessageId << " list memory (KBytes): " << (sentIds.size()*sizeof(msg.MessageId))/1024);
                    return msgId == msg.MessageId;
                });
            }
        }
    } catch (ServerError &err) {
        DEBUG("Error: " << err.what());
    }

    DEBUG("Client disconnected");
}

TcpSender::ReadStatus TcpSender::readSome(int clientFd)
{
    ssize_t numBytes;

    // non blocking recv
    if (-1 == (numBytes = recv(clientFd, receiveBuf_.get(), receiveBufLen_, 0))) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return ReadStatus::DataNotExist;
        } else {
            throw ServerError(std::string("Error from recvfrom()") + std::strerror(errno));
        }
    }

    onRead(numBytes);

    return ReadStatus::DataExist;
}

void TcpSender::onRead(ssize_t numBytes)
{}

void TcpSender::setClientToNonBlocking(int clientFd)
{
   // Set to non-blocking)
    if (-1 == fcntl(clientFd, F_SETFL, O_NONBLOCK)) {
        throw ServerError("Error from fcntl(): "s + std::strerror(errno));
    }
}
