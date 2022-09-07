#pragma once

#include "utils/exception/FeatherBaseException.hpp"

namespace feather::utils::exception
{
  class FeatherMemoryException : public FeatherBaseException<FeatherMemoryException, std::logic_error>
  {
  public:
    template <typename T, typename... Args>
    FeatherMemoryException(T first, Args... arg)
    {
      d_messageBuffer << first << " ";
      populate_args(arg...);
    }
  };
} // namespace feather::utils::exception