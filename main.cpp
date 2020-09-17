#include "Common/ThreadSafeList.hpp"
#include "UdpReceiver/UdpReceiver.h"
#include "TcpSender/TcpSender.h"

#include <cstring>
#include <thread>


void CheckArgsAndPrintUsage(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    CheckArgsAndPrintUsage(argc, argv);

    SafeMessageList messages;
    auto toShort = [](char *str) { return static_cast<short>(strtol(str, nullptr, 10)); };

    std::thread workerThreads[] = {
        std::thread([&] {
            UdpReceiver(argv[1], toShort(argv[2]), messages).listen();
        }),
        std::thread([&] {
            UdpReceiver(argv[1], toShort(argv[3]), messages).listen();
        }),
        std::thread([&] {
            TcpSender(argv[1], toShort(argv[4]), messages).listen();
        }),
        std::thread([&] {
            while (true) {
                messages.removeIf([&](const Message &msg) {
                    //if(10ull != msg.MessageData) DEBUG("removed: " <<msg.MessageId << " " <<msg.MessageData);
                    return 10ull != msg.MessageData; // removing all unnecessary messages from list
                });
                usleep(100 * 1000); // give a chance for other tasks
            }
        })
    };


    for (auto &&t : workerThreads)
        t.join();

    return 0;
}


void CheckArgsAndPrintUsage(int argc, char **argv)
{
    if (argc != 5) {
        const char *appName = ((argv[0][0])
                                   ? strrchr(&argv[0][0], '/')
                                   : "appname");
        std::cerr << "usage: ."
                  << appName << " <hostname> <udp_receiver_port1> <udp_receiver_port2> <tcp_sender_port>\n"
                  << "example: ." << appName << " \"::1\" 6333 6334 6335" << std::endl;
        exit(1);
    }
}