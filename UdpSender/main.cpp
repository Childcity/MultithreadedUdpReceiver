#include "../Common/Utils.hpp"
#include "../Common/Protocol/Packet.h"

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>


void CheckArgsAndPrintUsage(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    CheckArgsAndPrintUsage(argc, argv);

    Utils::unique_fd sockFd;
    struct addrinfo hints, *servInfo, *p;
    int rv;

    ZeroMem(&hints, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if (0 != (rv = getaddrinfo(argv[1], argv[2], &hints, &servInfo))) {
        std::cerr << "Error from getaddrinfo(): " << gai_strerror(rv) << std::endl;
        return 1;
    }

    std::cout << "IP addresses for: " << argv[1] << "\n"
              << Utils::PrintIpAddresses(servInfo) << std::endl;

    // loop through all the results and make a socket
    for (p = servInfo; p != nullptr; p = p->ai_next) {
        if (-1 == (sockFd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))) {
            perror("Error from socket()");
            continue;
        }

        break;
    }

    if (p == nullptr) {
        std::cerr << "Error: failed to create socket" << std::endl;
        return 1;
    }


    {
        // Prepare system for test
        std::ostream::sync_with_stdio(false);
        srand(time(NULL));
    }

    {
        // Run simulation

        // Will return 10 with some Probability
        const auto rndData = [] { return static_cast<uint64_t>(rand() % 12); };
        const struct addrinfo *const serverAddr = p;

        ssize_t numBytes;
        Packet packet;
        Message msg { 0, 11, 0, 0 };
        size_t msgId = 1;

        while (true) {
            msg.MessageData = rndData();
            msg.MessageId = msgId++;

            packet.setMessage(msg);
            if (-1 == (numBytes = Utils::SendAll(sockFd, packet.data(), packet.PacketSize, serverAddr))) {
                perror("talker: SendAll");
                return 1;
            }

            std::cout << "talker: sent " << numBytes
                      << " {msgId: " << msgId-1 << " msgData: " << msg.MessageData
                      << "} bytes to " << argv[1] << ":" << argv[2]
                      << "\n"; // we needn't std::endl

            if (msgId % 10 == 0)
                std::cout << std::flush; // flush stream 1/10 sent messages

            usleep(10 * 1000); // sleep 10ms, for other processes on CPU
        }
    }


    freeaddrinfo(servInfo);
    close(sockFd);

    return 0;
}


void CheckArgsAndPrintUsage(int argc, char **argv)
{
    if (argc != 3) {
        const char *appName = ((argv[0][0])
                                   ? strrchr(&argv[0][0], '/')
                                   : "appname");
        std::cerr << "usage: ."
                  << appName << " <udp_receive_hostname> <udp_receive_port>\n"
                  << "example: ." << appName << " \"::1\" 6333" << std::endl;
        exit(1);
    }
}