#pragma once

#include "utils/exception/FeatherBaseException.hpp"

namespace feather::utils::exception
{
  class FeatherCacheFullException : public FeatherBaseException<FeatherCacheFullException, std::runtime_error>
  {
  public:
    template <typename T, typename... Args>
    FeatherCacheFullException(T first, Args... arg)
    {
      d_messageBuffer << first << " ";
      populate_args(arg...);
    }
  };
} // namespace feather::utils::exception
