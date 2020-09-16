#include "UdpReceiver/UdpReceiver.h"
#include "Common/ThreadSafeQueue.hpp"


#include <cstring>
#include <thread>


int main(int argc, char *argv[])
{
    if (argc != 3) {
        std::cerr << "usage: ."
                  << ((argv[0][0])
                      ? strrchr(&argv[0][0], '/')
                      : "appname")
                  << " <hostname> <port>\n";
        exit(1);
    }

    SafeMessageQueue messageQueue;

    std::thread (([&]{
        auto port = static_cast<short>(strtol(argv[2], nullptr, 10));
        UdpReceiver receiver(argv[1], port, messageQueue);
        receiver.listen();
    })).join();

    return 0;
}
