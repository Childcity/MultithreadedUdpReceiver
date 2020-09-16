//
// Created by childcity on 14.09.20.
//

#include "Message.h"

#include "packing.h"
#include "../Utils.hpp"

void Message::Pack(Message &inMsg, unsigned char *outBuff) {
    unsigned messageSize = pack(outBuff, "HCQQ",
                                (uint16_t)inMsg.MessageSize,
                                (uint8_t)inMsg.MessageType,
                                (uint64_t)inMsg.MessageId,
                                (uint64_t)inMsg.MessageData);

    // put size of massage to its position
    packi16(outBuff, messageSize);
    inMsg.MessageSize = static_cast<uint16_t>(messageSize);
}

void Message::UnPack(unsigned char *inBuff, Message &outMsg) {
    unpack(inBuff, "HCQQ", &outMsg.MessageSize,
           &outMsg.MessageType,
           &outMsg.MessageId,
           &outMsg.MessageData);
}

void Message::dump() const {
    DEBUG("Message:\n"
                  << "\tMessageSize:\t" << MessageSize << "\n"
                  << "\tMessageType:\t" << (int)MessageType << "\n"
                  << "\tMessageId:  \t" << MessageId << "\n"
                  << "\tMessageData:\t" << MessageData);
}
