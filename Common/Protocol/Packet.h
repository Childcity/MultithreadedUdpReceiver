//
// Created by childcity on 15.09.20.
//

#ifndef MULTITHREADEDUDPRECEIVER_PACKET_H
#define MULTITHREADEDUDPRECEIVER_PACKET_H


#include "../Utils.hpp"
#include "Message.h"


struct Packet final {

    // Header of our packet
    struct __attribute__((__packed__)) Header {
        static constexpr uint8_t ConstHeader[] = { 'B', 'E', 'G', 'I', 'N', '\0' };
        static constexpr short HeaderSize = sizeof(ConstHeader);

        uint8_t header[HeaderSize];

        // clang-format off
        void init() { memcpy(header, ConstHeader, HeaderSize); }
        bool verify() { return 0 == memcmp(header, ConstHeader, HeaderSize); }
        uint8_t *buffer()  { return reinterpret_cast<uint8_t *>(this); }
        // clang-format on
    };

    // Footer of our packet
    struct __attribute__((__packed__)) Footer {
        static constexpr uint8_t ConstFooter[] = { 'E', 'N', 'D', ';', '\0' };
        static constexpr short FooterSize = sizeof(ConstFooter);

        uint8_t footer[FooterSize];

        // clang-format off
        void init() { memcpy(footer, ConstFooter, FooterSize); }
        bool verify() { return 0 == memcmp(footer, ConstFooter, FooterSize); }
        uint8_t *buffer() { return reinterpret_cast<uint8_t *>(this); }
        // clang-format on
    };

    static constexpr short PacketSize = sizeof(Header) + sizeof(Message) + sizeof(Footer);

    Packet();

    Packet(const Packet &) = delete;

    // clang-format off
    // Use next method after init() !!!
    Header *header() { return reinterpret_cast<Header *>(&buff_[0]); }
    // Use next method after init() !!!
    uint8_t *body() { return reinterpret_cast<uint8_t *>(&buff_[Header::HeaderSize]); }
    // Use next method after init() !!!
    Footer *footer() { return reinterpret_cast<Footer *>(&buff_[PacketSize - Footer::FooterSize]); }

    uint8_t *data() { return buff_; }
    // clang-format on

    void setMessage(Message &msg);

    Message getMessage();

    void dump();

private:
    void init();

private:
    uint8_t buff_[PacketSize];
};


#endif //MULTITHREADEDUDPRECEIVER_PACKET_H
