#pragma once

#include "utils/exception/FeatherBaseException.hpp"

namespace feather::utils::exception
{
    class FeatherFileConcurrentlyModifiedException : public FeatherBaseException<FeatherFileConcurrentlyModifiedException, std::logic_error>
    {
    public:
        template <typename T, typename... Args>
        FeatherFileConcurrentlyModifiedException(T first, Args... arg)
        {
            d_messageBuffer << first << " ";
            populate_args(arg...);
        }
    };
} // namespace feather::utils::exception