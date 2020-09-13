//
// Created by childcity on 12.09.20.
//

#ifndef MULTITHREADEDUDPRECEIVER_UTILS_H
#define MULTITHREADEDUDPRECEIVER_UTILS_H

#include <memory>

#define ZeroMem(Destination, Length) memset((Destination),0,(Length))

#include <iostream>
#include <arpa/inet.h>
#include <netdb.h>

void PrintIpAdresses(const struct addrinfo *serviceInfo) {
    const struct addrinfo *p;
    char ipstr[INET6_ADDRSTRLEN];

    for (p = serviceInfo; p != nullptr; p = p->ai_next) {
        void *addr;
        char *ipver;

        // get the pointer to the address itself,
        // different fields in IPv4 and IPv6:
        if (p->ai_family == AF_INET) { // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *) p->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = const_cast<char *>("IPv4");
        }
        else { // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *) p->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = const_cast<char *>("IPv6");
        }

        // convert the IP to a string and print it:
        inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
        std::cout << ipver << ": " << ipstr << "\n";
    }
    std::cout << std::endl;
}

#include <unistd.h>

template<typename Functor>
size_t __SendAllHelper(const char *buf, size_t len, const Functor &sendSome) {
    size_t total = 0;        // how many bytes we've sent
    size_t bytesLeft = len;  // how many we have left to send
    ssize_t n;

    while (total < len) {
        if ((n = sendSome(buf + total, bytesLeft)) == -1)
            break;

        total += n;
        bytesLeft -= n;
    }

    // return -1 on failure, number actually sent on success
    return n == -1 ? -1 : total;
}

size_t SendAll(int sockFd, const char *buf, size_t len, const struct addrinfo *addr = nullptr) {

    // send is for TCP
    const auto sendFunctor = [sockFd](const void *buf, size_t n) {
        return send(sockFd, buf, n, 0);
    };

    // sendto is for UDP
    const auto sendToFunctor = [sockFd, addr](const void *buf, size_t n) {
        return sendto(sockFd, buf, n, 0, addr->ai_addr, addr->ai_addrlen);
    };

    // choose method for sending
    return addr ? __SendAllHelper(buf, len, sendToFunctor)
                : __SendAllHelper(buf, len, sendFunctor);
}

// get sockaddr, IPv4 or IPv6:
void *GetInAddr(struct sockaddr_storage *saV4OrV6) {
    const sockaddr *sa = (struct sockaddr *) saV4OrV6;

    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *) sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *) sa)->sin6_addr);
}

#endif //MULTITHREADEDUDPRECEIVER_UTILS_H
