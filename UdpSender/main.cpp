#include "../Common/Protocol/packing.h"
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


int main(int argc, char *argv[])
{
    if (argc != 3) {
        const char *appName = ((argv[0][0])
                                   ? strrchr(&argv[0][0], '/')
                                   : "appname");
        std::cerr << "usage: ."
                  << appName << " <udp_receive_hostname> <udp_receive_port>\n"
                  << "example: ." << appName << " \"::1\" 6333" << std::endl;
        ;
        exit(1);
    }

    auto port = static_cast<uint64_t>(strtol(argv[2], nullptr, 10));

    int sockfd {};
    struct addrinfo hints {
    }, *servinfo, *p;
    int rv;

    ZeroMem(&hints, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0) {
        std::cerr << "Error from getaddrinfo: " << gai_strerror(rv) << std::endl;
        return 1;
    }

    std::cout << "IP addresses for: " << argv[1] << "\n"
              << Utils::PrintIpAddresses(servinfo) << std::endl;

    // loop through all the results and make a socket
    for (p = servinfo; p != nullptr; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("Error from socket");
            continue;
        }

        break;
    }

    if (p == nullptr) {
        std::cerr << "Error: failed to create socket" << std::endl;
        return 1;
    }

    struct addrinfo *serverAddr = p;

    ssize_t numBytes;
    Packet packet;
    Message msg { 0, 11, 22, port };
    packet.setMessage(msg);

    while (true) {
        if ((numBytes = Utils::SendAll(sockfd, packet.data(), packet.PacketSize, serverAddr)) == -1) {
            perror("talker: SendAll");
            exit(1);
        }
        std::cout << "talker: sent " << numBytes << " bytes to " << argv[1] << ":" << port << std::endl;
        usleep(100 * 1000);
    }


    freeaddrinfo(servinfo);
    close(sockfd);

    return 0;
}