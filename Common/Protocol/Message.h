//
// Created by childcity on 14.09.20.
//

#ifndef MULTITHREADEDUDPRECEIVER_MESSAGE_H
#define MULTITHREADEDUDPRECEIVER_MESSAGE_H


#include "packing.h"
#include "../Utils.hpp"


struct __attribute__((__packed__)) Message final {
    uint16_t MessageSize;
    uint8_t MessageType;
    uint64_t MessageId;
    uint64_t MessageData;

    static void Pack(Message &inMsg, unsigned char outBuff[]);

    static void UnPack(unsigned char inBuff[], Message &outMsg);

    void dump() const;
};

#endif //MULTITHREADEDUDPRECEIVER_MESSAGE_H
