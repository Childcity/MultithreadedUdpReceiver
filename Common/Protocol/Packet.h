//
// Created by childcity on 15.09.20.
//

#ifndef MULTITHREADEDUDPRECEIVER_PACKET_H
#define MULTITHREADEDUDPRECEIVER_PACKET_H


#include "../Utils.hpp"
#include "Message.h"


struct Packet final {

#pragma pack(push, 1)
    // Header of our packet
    struct Header {
    private:
        static constexpr char ConstHeader[] = { 'B', 'E', 'G', 'I', 'N', '\0' };
        static constexpr size_t HeaderSize = sizeof(ConstHeader);

    public:
        char header[HeaderSize];

        // clang-format off
        void init() { memcpy(header, ConstHeader, HeaderSize); }
        bool verify() const { return verify(header); }
        char *buffer() { return reinterpret_cast<char *>(this); }
        static bool verify(const char *header_) { return 0 == memcmp(header_, Header::ConstHeader, Header::HeaderSize); }
        // clang-format on
    };

    // Footer of our packet
    struct Footer {
    private:
        static constexpr char ConstFooter[] = { 'E', 'N', 'D', ';', '\n', '\0' };
        static constexpr size_t FooterSize = sizeof(ConstFooter);

    public:
        char footer[FooterSize];

        // clang-format off
        void init() { memcpy(footer, ConstFooter, FooterSize); }
        bool verify() const { return Verify(footer); }
        char *buffer() { return reinterpret_cast<char *>(this); }
        static bool Verify(const char *footer_) { return 0 == memcmp(footer_, Footer::ConstFooter, Footer::FooterSize); }
        // clang-format on
    };
#pragma pack(pop)

    static constexpr ssize_t HeaderSize = sizeof(Header);
    static constexpr ssize_t BodySize = sizeof(Message);
    static constexpr ssize_t FooterSize = sizeof(Footer);
    static constexpr ssize_t PacketSize = HeaderSize + BodySize + FooterSize;

    explicit Packet(char *buff = nullptr);

    Packet(const Packet &) = delete;

    ~Packet();

    // clang-format off
    // Use next method after init() !!!
    Header *header() { return reinterpret_cast<Header *>(&buff_[0]); }
    const Header *header() const { return reinterpret_cast<const Header *>(&buff_[0]); }
    // Use next method after init() !!!
    Footer *footer() { return reinterpret_cast<Footer *>(&buff_[PacketSize - FooterSize]); }
    const Footer *footer() const { return reinterpret_cast<Footer *>(&buff_[PacketSize - FooterSize]); }

    char *bodyBuffer() { return reinterpret_cast<char *>(&buff_[HeaderSize]); }
    const char *bodyBuffer() const { return reinterpret_cast<char *>(&buff_[HeaderSize]); }

    const char *data() const { return buff_; }
    // clang-format on

    bool verify() const;

    void setMessage(Message &msg);

    Message getMessage();

    void dump();

private:
    void init();

private:
    char *buff_;
    bool amIOwner;
};


#endif //MULTITHREADEDUDPRECEIVER_PACKET_H
