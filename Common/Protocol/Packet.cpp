//
// Created by childcity on 15.09.20.
//

#include "Packet.h"

Packet::Packet() {
    init();
}

void Packet::setMessage(Message &msg) {
    // Pack massage to the Network Endian
    Message::Pack(msg, body());
}

Message Packet::getMessage() {
    // UnPack massage to the current System Endian
    Message msg;
    Message::UnPack(body(), msg);
    return msg;
}

void Packet::dump() {
    DEBUG("\nPacket HEX:\n\t"
                  << Utils::DumpToStr(data(), PacketSize) << "\n"
                  << "Packet:\n"
                  << "\tHeader: \t" << header()->buffer() << "\n"
                  << "\tMessage:\t" << Utils::DumpToStr(body(), sizeof(Message)) << "\n"
                  << "\tFooter: \t" << footer()->buffer());
}

void Packet::init() {
    memset((buff_), 0xFF, (PacketSize)); //TODO: delete initialization when finished testing
    header()->init();
    footer()->init();
}
