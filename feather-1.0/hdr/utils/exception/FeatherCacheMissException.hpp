#pragma once

#include "utils/exception/FeatherBaseException.hpp"

namespace feather::utils::exception
{
class FeatherCacheMissException : public FeatherBaseException<FeatherCacheMissException, std::runtime_error>
{
  public:
    template <typename T, typename... Args>
    FeatherCacheMissException(T first, Args... arg)
    {
        d_messageBuffer << first << " ";
        populate_args(arg...);
    }
};
} // namespace feather::utils::exception
