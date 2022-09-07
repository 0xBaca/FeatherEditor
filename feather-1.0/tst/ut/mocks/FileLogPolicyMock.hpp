#include "utils/logger/policy/FileLogPolicy.hpp"

#include <gmock/gmock.h>

using namespace feather::utils::logger::policy;

namespace testing::mocks
{
  class FileLogPolicyMock : public FileLogPolicy
  {
  public:
    MOCK_METHOD1(write, void(const std::string &));
  };
} // namespace testing::mocks
