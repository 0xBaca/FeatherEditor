#pragma once

#include <sstream>
#include <system_error>

namespace feather::utils
{
template <typename derivedExType, typename baseExType>
class FeatherBaseException : public baseExType
{
  public:
    FeatherBaseException() : baseExType("")
    {
    }
    FeatherBaseException(std::error_code const &code) : baseExType("", code)
    {
    }
    FeatherBaseException(const FeatherBaseException &other) : baseExType("")
    {
        this->d_message = other.d_message;
        this->d_messageBuffer << other.d_messageBuffer.rdbuf();
    };
    const char *what() const noexcept
    {
        if (d_message.empty())
        {
            d_message = d_messageBuffer.str();
        }

        return d_message.c_str();
    }
    ~FeatherBaseException() noexcept
    {
        d_messageBuffer.str("");
    }

  protected:
    template <typename T, typename... Args>
    void populate_args(T first, Args... args)
    {
        d_messageBuffer << first << " ";
        populate_args(args...);
    }
    void populate_args()
    {
        d_message = d_messageBuffer.str();
        d_messageBuffer.flush();
    }
    std::stringstream d_messageBuffer;
    mutable std::string d_message;
};
} // namespace feather::utils
