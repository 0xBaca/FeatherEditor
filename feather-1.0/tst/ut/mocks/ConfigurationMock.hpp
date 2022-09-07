#include "config/Config.hpp"

#include <gmock/gmock.h>

using namespace feather::config;
using namespace feather::utils::logger::policy;

namespace testing::mocks
{
  class ConfigurationMock : public Configuration
  {
  public:
    static auto constexpr LOG_FILE_NAME = "testLog.txt";
  };
} // namespace testing::mocks
