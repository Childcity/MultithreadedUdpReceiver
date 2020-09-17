//
// Created by childcity on 12.09.20.
//

#ifndef MULTITHREADEDUDPRECEIVER_UTILS_H
#define MULTITHREADEDUDPRECEIVER_UTILS_H


#include <memory>
#include <chrono>
#include <iomanip>
#include <cstring>
#include <iostream>

#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>


#define ZeroMem(Destination, Length) memset((Destination), 0, (Length))

//#define __FILENAME__  (strrchr("/" __FILE__, '/') + 1)
#define DEBUG(msg_) std::cout << Utils::DebugPrinter(__FILE__, __LINE__) << msg_ << std::endl;

class Utils {
public:

    // RAII for unix file descriotors
    struct unique_fd final {
    public:
        explicit unique_fd()
                : fd_(-1)
        {}

        explicit unique_fd(int fd) noexcept
                : fd_(fd)
        {}

        unique_fd(const unique_fd &) = delete;

        unique_fd &operator=(const unique_fd &) = delete;

        unique_fd(unique_fd &&other) noexcept
                : fd_(other.fd_)
        {
            other.fd_ = -1;
        }

        unique_fd &operator=(unique_fd &&other) noexcept
        {
            fd_ = other.fd_;
            other.fd_ = -1;
            return *this;
        }

        unique_fd &operator=(int fd) noexcept
        {
            if (fd_ != -1)
                close(fd_);
            fd_ = fd;
            return *this;
        }

        explicit operator bool() const
        {
            return fd_ != -1;
        }

        operator int() const
        {
            return fd_;
        }

        ~unique_fd()
        {
            if (fd_ != -1)
                close(fd_);
        }

    private:
        int fd_;
    };

    struct DebugPrinter {
        const char *file;
        int line;

        DebugPrinter(const char *file_, int line_)
                : file(file_)
                , line(line_)
        {}

        friend std::ostream &operator<<(std::ostream &stream, const DebugPrinter printer)
        {
            using std::chrono::system_clock;
            const auto now = system_clock::to_time_t(system_clock::now());

            stream << std::put_time(std::localtime(&now), "%T ")
                   << std::string_view(printer.file, std::strlen(printer.file))
                   << " [" << printer.line << "]> ";

            return stream;
        }
    };

    static std::string PrintIpAddresses(const struct addrinfo *serviceInfo)
    {
        const struct addrinfo *p;
        char ipstr[INET6_ADDRSTRLEN];
        std::ostringstream stream;

        for (p = serviceInfo; p != nullptr; p = p->ai_next) {
            void *addr;
            char *ipver;

            // Get the pointer to the address itself,
            // Different fields in IPv4 and IPv6:
            if (p->ai_family == AF_INET) { // IPv4
                auto *ipv4 = (struct sockaddr_in *)p->ai_addr;
                addr = &(ipv4->sin_addr);
                ipver = const_cast<char *>("IPv4");
            } else { // IPv6
                auto *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
                addr = &(ipv6->sin6_addr);
                ipver = const_cast<char *>("IPv6");
            }

            // convert the IP to a string and print it:
            inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
            stream << ipver << ": " << ipstr << "\n";
        }

        return stream.str();
    }

    static ssize_t SendAll(int sockFd, const char *buf, ssize_t len, const struct addrinfo *addr = nullptr)
    {
        // system call send is for TCP
        const auto sendFunctor = [sockFd](const void *buf, ssize_t n) {
            return send(sockFd, buf, n, 0);
        };

        // System call sendto is for UDP
        const auto sendToFunctor = [sockFd, addr](const void *buf, ssize_t n) {
            return sendto(sockFd, buf, n, 0, addr->ai_addr, addr->ai_addrlen);
        };

        // Choose method for sending
        return addr ? SendAllHelper(buf, len, sendToFunctor)
                    : SendAllHelper(buf, len, sendFunctor);
    }

    // Get sockaddr, IPv4 or IPv6:
    static void *GetInAddr(struct sockaddr_storage *saV4OorV6)
    {
        const sockaddr *sa = (struct sockaddr *)saV4OorV6;

        if (sa->sa_family == AF_INET) {
            return &(((struct sockaddr_in *)sa)->sin_addr);
        }

        return &(((struct sockaddr_in6 *)sa)->sin6_addr);
    }

    static void Dump(const char buf[], size_t size)
    {
        Dump((const uint8_t *)&buf[0], size);
    }

    static void Dump(const uint8_t buf[], size_t size)
    {
        std::cout << DumpToStr(buf, size) << std::endl;
    }

    static std::string DumpToStr(const char buf[], size_t size)
    {
        return DumpToStr((const uint8_t *)&buf[0], size);
    }

    static std::string DumpToStr(const uint8_t buf[], size_t size)
    {
        std::stringstream stream;
        for (size_t i = 0; i < size; ++i)
            stream << std::hex
                   << std::setfill('0')
                   << std::setw(2)
                   << (int)buf[i] << " ";
        return stream.str();
    }

private:
    template<typename Functor>
    static ssize_t SendAllHelper(const char *buf, ssize_t len, const Functor &sendSome)
    {
        ssize_t total = 0;       // how many bytes we've sent
        ssize_t bytesLeft = len; // how many we have left to send
        ssize_t n;

        while (total < len) {
            if (-1 == (n = sendSome(buf + total, bytesLeft)))
                break;

            total += n;
            bytesLeft -= n;
        }

        // return -1 on failure, number actually sent on success
        return n == -1 ? -1 : total;
    }
};

#endif //MULTITHREADEDUDPRECEIVER_UTILS_H
