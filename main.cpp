#include "packing.hpp"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>

#include <iostream>

#define SERVER_PORT "6333"
#define SERVER_ADR "localhost"

int main(int argc, char *argv[]) {
    int yes=1;        // For setsockopt() SO_REUSEADDR, below
    int sockfd{};
    struct addrinfo hints{}, *servinfo, *p;
    int rv;
    ssize_t numbytes;

    ZeroMem(&hints, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(SERVER_ADR, SERVER_PORT, &hints, &servinfo)) != 0) {
        std::cerr << "Error from getaddrinfo: " << gai_strerror(rv) << std::endl;
        return 1;
    }

    std::cout << "IP addresses for: " << SERVER_ADR << std::endl;
    PrintIpAdresses(servinfo);

    // loop through all the results and bind to the first we can
    for (p = servinfo; p != nullptr; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("Error from listening socket");
            continue;
        }

        // Lose the pesky "address already in use" error message
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("Error from bind");
            continue;
        }

        break;
    }

    if (p == nullptr) {
        std::cerr << "Error: failed to create socket" << std::endl;
        return 1;
    }

    freeaddrinfo(servinfo);

#define MAXBUFLEN 256
#define POLL_SOCK_NUM 1
#define POLL_TIMEOUT 10

    char buf[MAXBUFLEN];
    struct sockaddr_storage their_addr{};
    socklen_t addr_len = sizeof their_addr;
    char ipStr[INET6_ADDRSTRLEN];

    struct pollfd pfds[POLL_SOCK_NUM];  // Monitoring one socket
    pfds[0].fd = sockfd;          // Standard input
    pfds[0].events = POLLIN; // Tell me when ready to read

    while (true) {

        int poll_count = poll(pfds, POLL_SOCK_NUM, POLL_TIMEOUT);

        if (poll_count == -1) {
            perror("Error from poll");
            exit(1);
        }

        if (poll_count == 0) {
            // timeout on poll
            std::cout << "timeout..." << std::endl;
            continue;
        }

        // non blocking recv

        if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN - 1, 0, (struct sockaddr *) &their_addr, &addr_len)) == -1) {
            perror("recvfrom");
            exit(1);
        }

        printf("listener: got packet from %s\n",
               inet_ntop(their_addr.ss_family, GetInAddr(&their_addr), ipStr, sizeof ipStr));

        printf("listener: packet is %d bytes long\n", numbytes);
        buf[numbytes] = '\0';
        printf("listener: packet contains \"%s\"\n", buf);

        //if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,
        //                         (struct sockaddr *)&their_addr, &addr_len)) == -1) {
        //    perror("recvfrom");
        //    exit(1);
        //}
//
        //std::cout << "talker: sent " << numbytes << " bytes to " << SERVER_PORT << std::endl;
        //sleep(1);
    }

    close(sockfd);

    return 0;
}
