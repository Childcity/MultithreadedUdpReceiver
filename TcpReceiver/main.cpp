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


using namespace std::string_literals;

void CheckArgsAndPrintUsage(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    CheckArgsAndPrintUsage(argc, argv);

    Utils::unique_fd sockFd;
    struct addrinfo hints, *servInfo, *p;
    int rv;

    ZeroMem(&hints, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

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

        if (-1 == connect(sockFd, p->ai_addr, p->ai_addrlen)) {
            close(sockFd);
            perror(("Error from connect() to host "s + argv[1] + ":" + argv[2]).c_str());
            continue;
        }

        break;
    }

    if (p == nullptr) {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }

    freeaddrinfo(servInfo);

    {
        // Prepare system for test
        std::ostream::sync_with_stdio(false);
    }

    {
        // Run simulation
        ssize_t numBytes;
        char receiveBuf[64];

        while (true) {
            // non blocking recv
            if (-1 == (numBytes = recv(sockFd, receiveBuf, sizeof(receiveBuf), 0)))
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    continue;
                } else {
                    perror("Error recv()");
                    return 1;
                }
            }

            if (numBytes < Packet::PacketSize) {
                DEBUG("incorrect numBytes: " << numBytes);
                continue;
            }

            Packet packet(receiveBuf);
            Message msg = packet.getMessage();

            if (msg.MessageSize != Packet::BodySize) {
                DEBUG("incorrect msg.MessageSize: " << msg.MessageSize);
                continue;
            }

            msg.dump();
        }
    }


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
                  << appName << " <tcp_sender_hostname> <tcp_sender_port>\n"
                  << "example: ." << appName << " \"::1\" 6335" << std::endl;
        exit(1);
    }
}