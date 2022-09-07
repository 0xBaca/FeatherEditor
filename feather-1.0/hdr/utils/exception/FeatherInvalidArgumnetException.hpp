#pragma once

#include "utils/exception/FeatherBaseException.hpp"

namespace feather::utils::exception
{
    class FeatherInvalidArgumentException : public FeatherBaseException<FeatherInvalidArgumentException, std::logic_error>
    {
    public:
        template <typename T, typename... Args>
        FeatherInvalidArgumentException(T first, Args... arg)
        {
            d_messageBuffer << first << " ";
            populate_args(arg...);
        }
    };
} // namespace feather::utils::exception