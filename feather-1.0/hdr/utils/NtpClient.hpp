#pragma once

#include <cstdint>
#include "utils/NtpPacket.hpp"

#include <arpa/inet.h>
#include <cstring>
#include <errno.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <string.h>

#include <optional>

namespace feather::utils
{
    class NtpClient
    {
    public:
        NtpClient();
        bool licenseExpired() const;
        ~NtpClient();

    private:
        std::optional<std::string> getIp(std::string const &) const;
        uint32_t getHostTime() const;
        uint32_t getUnixTimeInSeconds() const;
        int sockfd;
        static inline const size_t port = 123;
        struct sockaddr_in servaddr;
        static constexpr long NTP_TIMESTAMP_DELTA{2208988800ull};
        static constexpr double ONE_SECOND{4294967295ll};
        static inline const std::string server = "time.google.com";
    };
}
