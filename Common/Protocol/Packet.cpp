//
// Created by childcity on 15.09.20.
//

#include "Packet.h"

#include <cstdint>


Packet::Packet(char *buff)
{
    if (buff) {
        buff_ = buff;
        amIOwner = false;
    } else {
        buff_ = new char[PacketSize];
        amIOwner = true;
        init();
    }
}

Packet::~Packet()
{
    if (amIOwner && buff_) {
        delete[] buff_;
    }
}

bool Packet::verify() const
{
    return header()->verify() &&
           footer()->verify();
}

void Packet::setMessage(Message &msg)
{
    // Pack massage to the Network Endian
    Message::Pack(msg, reinterpret_cast<uint8_t *>(bodyBuffer()));
}

Message Packet::getMessage()
{
    // UnPack massage to the current System Endian
    Message msg;
    Message::UnPack(reinterpret_cast<uint8_t *>(bodyBuffer()), msg);
    return msg;
}

void Packet::dump()
{
    DEBUG("\nPacket HEX:\n\t"
                  << Utils::DumpToStr(data(), PacketSize) << "\n"
                  << "Packet:\n"
                  << "\tHeader: \t" << header()->buffer() << "\n"
                  << "\tMessage:\t" << Utils::DumpToStr(bodyBuffer(), sizeof(Message)) << "\n"
                  << "\tFooter: \t" << footer()->buffer());
}

void Packet::init()
{
    header()->init();
    footer()->init();
}
