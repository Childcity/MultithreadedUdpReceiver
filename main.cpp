#include "UdpReceiver/UdpReceiver.h"
#include "Common/ThreadSafeList.hpp"
#include "TcpSender/TcpSender.h"


#include <cstring>
#include <thread>


int main(int argc, char *argv[])
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

    SafeMessageList messages;

    std::thread workerThreads[] = {
        std::thread([&] {
            auto port = static_cast<short>(strtol(argv[2], nullptr, 10));
            UdpReceiver(argv[1], port, messages).listen();
        }),
        std::thread([&] {
            auto port = static_cast<short>(strtol(argv[3], nullptr, 10));
            UdpReceiver(argv[1], port, messages).listen();
        }),
        std::thread([&] {
            auto port = static_cast<short>(strtol(argv[4], nullptr, 10));
            TcpSender(argv[1], port, messages).listen();
        }),
        std::thread([&] {
            while (true) {
                messages.removeIf([&](const Message &msg) {
                    //if(10ull != msg.MessageData) DEBUG("removed: " <<msg.MessageId << " " <<msg.MessageData);
                    return 10ull != msg.MessageData; // removing all unnecessary messages from list
                });
                usleep(10 * 1000); // give a chance for others tasks
            }
        })
    };


    for (auto &&t : workerThreads)
        t.join();

    return 0;
}
