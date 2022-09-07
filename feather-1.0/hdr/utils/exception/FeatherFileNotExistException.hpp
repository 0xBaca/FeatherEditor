#pragma once

#include "utils/exception/FeatherBaseException.hpp"

#include <filesystem>

namespace feather::utils::exception
{
  class FeatherFileNotExistException : public FeatherBaseException<FeatherFileNotExistException, std::filesystem::filesystem_error>
  {
  public:
    template <typename T, typename... Args>
    FeatherFileNotExistException(std::fstream const &stream, T first, Args... arg)
        : FeatherBaseException(std::make_error_code(std::io_errc::stream))
    {
      d_messageBuffer << first << " ";
      populate_args(arg...);
    }
  };
} // namespace feather::utils::exception