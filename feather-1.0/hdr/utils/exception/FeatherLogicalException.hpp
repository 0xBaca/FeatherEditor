#pragma once

#include "utils/exception/FeatherBaseException.hpp"

namespace feather::utils::exception
{
class FeatherLogicalException : public FeatherBaseException<FeatherLogicalException, std::logic_error>
{
  public:
    template <typename T, typename... Args>
    FeatherLogicalException(T first, Args... arg)
    {
        d_messageBuffer << first << " ";
        populate_args(arg...);
    }
};
} // namespace feather::utils::feather
