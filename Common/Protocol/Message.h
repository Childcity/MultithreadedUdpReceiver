//
// Created by childcity on 14.09.20.
//

#ifndef MULTITHREADEDUDPRECEIVER_MESSAGE_H
#define MULTITHREADEDUDPRECEIVER_MESSAGE_H


#include "../ThreadSafeList.hpp"
#include <cstdint>

#pragma pack(push, 1)
struct Message final {
    uint16_t MessageSize;
    uint8_t MessageType;
    uint64_t MessageId;
    uint64_t MessageData;

    static void Pack(Message &inMsg, unsigned char outBuff[]);

    static void UnPack(unsigned char inBuff[], Message &outMsg);

    void dump() const;
};
#pragma pack(pop)

using SafeMessageList = childcity::threadsafelist::ThreadSafeList<Message>;

#endif //MULTITHREADEDUDPRECEIVER_MESSAGE_H
