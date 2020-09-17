//
// Created by childcity on 14.09.20.
//

#include "UdpReceiver.h"

UdpReceiver::UdpReceiver(const std::string_view &address, short port, SafeMessageList &list)
    : UdpServer(address, port, 64)
    , list_(list)
{}

void UdpReceiver::listen()
{
    createListener();

    DEBUG("Server listening on: " << address_ << ":" << port_);

    try {
        for (;;)
        {
            readSome();
        }
    } catch (ServerError &err) {
        DEBUG("readSome: " << err.what());
        throw;
    }
}

void UdpReceiver::onRead(ssize_t numBytes)
{
    if (numBytes < Packet::PacketSize) {
        DEBUG("incorrect numBytes: " << numBytes);
        return;
    }

    Packet packet(receiveBuf_.get());
    Message msg = packet.getMessage();

    if (msg.MessageSize != Packet::BodySize) {
        DEBUG("incorrect msg.MessageSize: " << msg.MessageSize);
        return;
    }

    //msg.dump();
    list_.pushFront(msg);
}
