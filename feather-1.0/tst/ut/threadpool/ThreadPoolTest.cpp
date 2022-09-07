#include "helpers/Timeout.hpp"
#include "mocks/FileLogPolicyMock.hpp"
#include "utils/exception/FeatherTimeoutException.hpp"
#include "utils/logger/policy/FileLogPolicy.hpp"
#include "threadpool/ThreadPool.hpp"

#include <cstdio>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>

int main(int argc, char **argv) __attribute__((weak));

using namespace feather::threadPool;
using namespace feather::utils::logger::policy;
using namespace testing::mocks;

using ::testing::Throw;

class ThreadPoolTest : public ::testing::Test
{
    static const char *LOG_FILE_NAME;

  protected:
    std::shared_ptr<FileLogPolicy> logMock;
    virtual void SetUp()
    {
        logMock = std::make_shared<FileLogPolicy>(LOG_FILE_NAME);
    }
};

TEST_F(ThreadPoolTest, ifNoJobEnquedEndThreadPoolSuccesfully)
{
    //before
    std::unique_ptr<Timeout> timeout = std::make_unique<Timeout>();
    std::shared_ptr<ThreadPool> threadPool;

    //when
    ASSERT_NO_THROW(threadPool = ThreadPool::getPool(64, logMock));

    //then
    Timeout::wait(timeout.get(), std::chrono::duration<int, std::ratio<1, 100>>(std::chrono::seconds(1)));
    threadPool->setEndOfConsumption();
    threadPool->waitForThreads();
    timeout->breakTimeout();
}

TEST_F(ThreadPoolTest, DISABLED_shouldThrowTimeoutException)
{
    //before
    std::unique_ptr<Timeout> timeout = std::make_unique<Timeout>();
    std::shared_ptr<ThreadPool> threadPool;

    //when
    ASSERT_NO_THROW(threadPool = ThreadPool::getPool(64, logMock));

    //then
    try
    {
        Timeout::wait(timeout.get(), std::chrono::duration<int, std::ratio<1, 100>>(std::chrono::seconds(1)));
        threadPool->waitForThreads();
    }
    catch (boost::exception_detail::error_info_injector<feather::utils::exception::FeatherTimeoutException> &)
    {
        return;
    }
    ASSERT_TRUE(0);
}

const char *ThreadPoolTest::LOG_FILE_NAME = "testLog.txt";

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
