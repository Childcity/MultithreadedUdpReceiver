//
// Created by childcity on 14.09.20.
//

#ifndef MULTITHREADEDUDPRECEIVER_MESSAGE_H
#define MULTITHREADEDUDPRECEIVER_MESSAGE_H


#include "packing.h"
#include "Utils.hpp"


struct __attribute__((__packed__)) Message final {
    uint16_t MessageSize;
    uint8_t MessageType;
    uint64_t MessageId;
    uint64_t MessageData;

    static void Pack(Message &inMsg, unsigned char outBuff[])
    {
        unsigned messageSize = pack(outBuff, "HCQQ",
                                    (uint16_t)inMsg.MessageSize,
                                    (uint8_t)inMsg.MessageType,
                                    (uint64_t)inMsg.MessageId,
                                    (uint64_t)inMsg.MessageData);

        // put size of massage to its position
        packi16(outBuff, messageSize);
        inMsg.MessageSize = static_cast<uint16_t>(messageSize);
    }

    static void UnPack(unsigned char inBuff[], Message &outMsg)
    {
        unpack(inBuff, "HCQQ", &outMsg.MessageSize,
               &outMsg.MessageType,
               &outMsg.MessageId,
               &outMsg.MessageData);
    }
};
//https://stackoverflow.com/questions/21776579/own-c-network-protocol-how-to-define-unique-binary-telegram-values
struct Packet final {

    struct __attribute__((__packed__)) Header {
        static constexpr char ConstHeader[] = "BEGIN";
        static constexpr short HeaderSize = sizeof(ConstHeader);
        char header[HeaderSize];
        void init() { memcpy(header, ConstHeader, HeaderSize); }
        bool verify() { return 0 == memcmp(header, ConstHeader, HeaderSize); }
        char *buffer() { return reinterpret_cast<char *>(this); }
    };

    struct __attribute__((__packed__)) Footer {
        static constexpr char ConstFooter[] = "END;\n\n";
        static constexpr short FooterSize = sizeof(ConstFooter);
        char footer[FooterSize ];
        void init() { memcpy(footer, ConstFooter, FooterSize ); }
        bool verify() { return 0 == memcmp(footer, ConstFooter, FooterSize ); }
        char *buffer() { return reinterpret_cast<char *>(this); }
    };

    static constexpr short PacketSize = sizeof(Header) + sizeof(Message) + sizeof(Footer);

public:
    Packet() { init(); };
    Packet(const Packet &) = delete;

    // use next method after init() !!!
    Header *header() { return reinterpret_cast<Header *>(&buff_[0]); }
    // use next method after init() !!!
    Footer *footer() { return reinterpret_cast<Footer *>(&buff_[PacketSize - sizeof(Footer)+1]); }
    // use next method after init() !!!
    Message *body() { return reinterpret_cast<Message *>(&buff_[sizeof(Header)]); }

    const char *data() { return buff_; }

private:
    void init()
    {
        header()->init();
        footer()->init();
        //Utils::Dump((unsigned char *)&buff_[0], sizeof(buff_));
    }

private:
    char buff_[PacketSize] = {0};
};

#endif //MULTITHREADEDUDPRECEIVER_MESSAGE_H
