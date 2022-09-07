#ifndef EXCEPTION_TIMEOUT_INCLUDED
#define EXCEPTION_TIMEOUT_INCLUDED

#include "utils/exception/FeatherBaseException.hpp"

namespace feather::utils::exception
{
class FeatherTimeoutException : public FeatherBaseException<FeatherTimeoutException, std::runtime_error>
{
  public:
    template <typename D, typename T, typename... Args>
    FeatherTimeoutException(D duration, T first, Args... arg)
    {
        d_messageBuffer << "Timeout waiting, after : " << duration.count() << first << " ";
        populate_args(arg...);
    }
};
} // namespace feather::utils::exception

#endif
