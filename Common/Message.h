//
// Created by childcity on 14.09.20.
//

#ifndef MULTITHREADEDUDPRECEIVER_MESSAGE_H
#define MULTITHREADEDUDPRECEIVER_MESSAGE_H


#include "packing.h"
#include "Utils.hpp"


struct Message final {
    uint16_t MessageSize;
    uint8_t MessageType;
    uint64_t MessageId;
    uint64_t MessageData;

    static void Pack(const Message &msg)
    {
        unsigned char buf[32];
        size_t messageSize = pack(buf, "HCQQ",
                                  (uint16_t)msg.MessageSize,
                                  (uint8_t)msg.MessageType,
                                  (uint64_t)msg.MessageId,
                                  (uint64_t)msg.MessageData);

        DEBUG("messageSize (hex): " << std::hex << messageSize);
        packi16(buf, messageSize);
    }
};


#endif //MULTITHREADEDUDPRECEIVER_MESSAGE_H
