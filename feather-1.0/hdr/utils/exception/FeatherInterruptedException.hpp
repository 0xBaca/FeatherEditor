#pragma once

#include "utils/exception/FeatherBaseException.hpp"

namespace feather::utils::exception
{
    class FeatherInterruptedException : public FeatherBaseException<FeatherInterruptedException, std::runtime_error>
    {
    public:
        template <typename T, typename... Args>
        FeatherInterruptedException(T first, Args... arg)
        {
            d_messageBuffer << first << " ";
            populate_args(arg...);
        }
    };
} // namespace feather::utils::exception