//
// Created by childcity on 14.09.20.
//

#include "Message.h"

#include "packing.h"
#include "../Utils.hpp"

void Message::Pack(Message &inMsg, unsigned char *outBuff)
{
    inMsg.MessageSize = sizeof(Message);

    packi16(outBuff, (uint16_t)inMsg.MessageSize);
    *(outBuff + 2) = (uint8_t)inMsg.MessageType;
    packi64(outBuff + 2 + 1, (uint64_t)inMsg.MessageId);
    packi64(outBuff + 2 + 1 + 8, (uint64_t)inMsg.MessageData);

    //unsigned messageSize = pack(outBuff, "HCQQ",
    //                            (uint16_t)inMsg.MessageSize,
    //                            (uint8_t)inMsg.MessageType,
    //                            (uint64_t)inMsg.MessageId,
    //                            (uint64_t)inMsg.MessageData);

    // put size of massage to its position
    //packi16(outBuff, messageSize);
    //inMsg.MessageSize = static_cast<uint16_t>(messageSize);
}

void Message::UnPack(unsigned char *inBuff, Message &outMsg)
{
    outMsg.MessageSize = (uint16_t)unpacku16(inBuff);
    outMsg.MessageType = (uint8_t) * (inBuff + 2);
    outMsg.MessageId = (uint64_t)unpacku64(inBuff + 2 + 1);
    outMsg.MessageData = (uint64_t)unpacku64(inBuff + 2 + 1 + 8);

    //unpack(inBuff, "HCQQ", &outMsg.MessageSize,
    //       &outMsg.MessageType,
    //       &outMsg.MessageId,
    //       &outMsg.MessageData);
}

void Message::dump() const
{
    DEBUG("Message:\n"
          << "\tMessageSize:\t" << MessageSize << "\n"
          << "\tMessageType:\t" << (int)MessageType << "\n"
          << "\tMessageId:  \t" << MessageId << "\n"
          << "\tMessageData:\t" << MessageData);
}
