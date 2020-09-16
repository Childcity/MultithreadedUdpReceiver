#include "UdpReceiver/UdpReceiver.h"
#include "Common/ThreadSafeQueue.hpp"


#include <cstring>
#include <thread>


int main(int argc, char *argv[])
{
    if (argc != 4) {
        std::cerr << "usage: ."
                  << ((argv[0][0])
                          ? strrchr(&argv[0][0], '/')
                          : "appname")
                  << " <udp_receive_hostname> <udp_receive_port1> <udp_receive_port2>\n"
                  << "example: ./MultithreadedUdpReceiver \"::1\" 6333 6334";
        exit(1);
    }

    SafeMessageQueue messageQueue;

    std::thread receiverThreads[] = {
        std::thread([&] {
            auto port = static_cast<short>(strtol(argv[2], nullptr, 10));
            UdpReceiver(argv[1], port, messageQueue).listen();
        }),
        std::thread([&] {
            auto port = static_cast<short>(strtol(argv[3], nullptr, 10));
            UdpReceiver(argv[1], port, messageQueue).listen();
        })
    };


    for (auto &&t : receiverThreads)
        t.join();

    return 0;
}
