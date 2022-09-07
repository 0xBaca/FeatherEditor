#include "mocks/FileLogPolicyMock.hpp"
#include "utils/logger/Logger.hpp"
#include "utils/logger/policy/FileLogPolicy.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace feather::utils::logger::policy;
using namespace testing::mocks;

using ::testing::Eq;
using ::testing::StrEq;
using ::testing::Throw;

class FileLogPolicyTest : public ::testing::Test
{
protected:
  static const char *LOG_FILE_NAME;

public:
  void SetUp() override
  {
  }

  void TearDown() override
  {
  }
};

TEST_F(FileLogPolicyTest, createLoggerWthLogPolicy)
{
  //before
  std::unique_ptr<FileLogPolicy> fileLogPolicy;

  //when
  ASSERT_NO_THROW(fileLogPolicy.reset(new FileLogPolicy(LOG_FILE_NAME)));

  //then
  ASSERT_NO_THROW((*fileLogPolicy)("test", "file", "write"));
}

const char *FileLogPolicyTest::LOG_FILE_NAME = "testLog.txt";
