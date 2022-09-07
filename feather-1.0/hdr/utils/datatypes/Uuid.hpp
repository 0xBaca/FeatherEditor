#pragma once

#include <random>
#include <sstream>
#include <string>

namespace feather::utils::datatypes
{
    class Uuid
    {
    private:
        std::string uuid;

    public:
        Uuid(unsigned int = 16UL);
        Uuid(std::string);
        std::string const &getString() const;
        bool operator==(Uuid const &other) const
        {
            return uuid == other.uuid;
        }

        bool operator<(Uuid const &other) const
        {
            return uuid < other.uuid;
        }

        static inline unsigned int random_char()
        {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, 255);
            return dis(gen);
        }

        static inline std::string generate_hex(const unsigned int len)
        {
            std::stringstream ss;
            for (unsigned int i = 0; i < len; i++)
            {
                const auto rc = random_char();
                std::stringstream hexstream;
                hexstream << std::hex << rc;
                auto hex = hexstream.str();
                ss << (hex.length() < 2 ? '0' + hex : hex);
            }
            return ss.str();
        }
    };
} // namespace feather::utils::datatypes
