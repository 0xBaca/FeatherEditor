#include "config/Config.hpp"
#include "utils/datatypes/Global.hpp"
#include "utils/logger/policy/FileLogPolicy.hpp"
#include "mocks/FileLogPolicyMock.hpp"

#include <gtest/gtest.h>
#include <string>

static const std::string NON_EXISTING_FILE_NAME = "f.json";
static const std::string PARTIAL_CONFIG_FILE = "partialFeatherConfig.json";
static const std::string PROPER_CONFIG_FILE = "featherConfig.json";
static const std::string IMPROPER_CONFIG_FILE = "improperFeatherConfig.json";
static const std::string WRONG_DATA_TYPES_CONFIG_FILE = "wrongDataTypesFeatherConfig.json";

using namespace feather::utils::logger::policy;
using namespace testing::mocks;

using ::testing::_;
using ::testing::Return;

namespace feather::config
{
    class ConfigTest : public ::testing::Test
    {
    protected:
        std::shared_ptr<FileLogPolicy> log;
        virtual void SetUp()
        {
            log = std::make_shared<FileLogPolicy>("test.log");
        }
    };

    TEST_F(ConfigTest, defaultConfigurationWhenFileIsImproperJson)
    {
        // before
        // when
        // then
        std::unique_ptr<const Configuration> configuration(new Configuration(IMPROPER_CONFIG_FILE, log));
        ASSERT_TRUE(configuration);
        ASSERT_STREQ(configuration->getConfigurationFileName().c_str(), Configuration::DEFAULT_FILE_NAME.c_str());
        ASSERT_EQ(configuration->isCacheOn(), Configuration::DEFAULT_CACHE_ON);
        ASSERT_EQ(configuration->getCacheSize(), Configuration::DEFAULT_CACHE_SIZE);
        ASSERT_EQ(configuration->getMaxLogFileSize(), Configuration::DEFAULT_MAX_LOG_SIZE);
        ASSERT_EQ(configuration->getMemoryBytesRelaxed(), ONE_MEGABYTE);
        ASSERT_EQ(configuration->getStorageDirectory(), Configuration::DEFAULT_STORAGE_DIRECTORY + '/');
        ASSERT_EQ(configuration->getLocale(), Configuration::DEFAULT_LOCALE);
        ASSERT_EQ(configuration->getMode(), Configuration::DEFAULT_MODE);
        ASSERT_EQ(configuration->getTabWidth(), Configuration::DEFAULT_TAB_WIDTH);
    }

    TEST_F(ConfigTest, defaultConfigurationWhenFileDoesNotExist)
    {
        // before
        // when
        // then
        std::unique_ptr<const Configuration> configuration(new Configuration(NON_EXISTING_FILE_NAME, log));
        ASSERT_TRUE(configuration);
        ASSERT_STREQ(configuration->getConfigurationFileName().c_str(), Configuration::DEFAULT_FILE_NAME.c_str());
        ASSERT_EQ(configuration->isCacheOn(), Configuration::DEFAULT_CACHE_ON);
        ASSERT_EQ(configuration->getCacheSize(), Configuration::DEFAULT_CACHE_SIZE);
        ASSERT_EQ(configuration->getMaxLogFileSize(), Configuration::DEFAULT_MAX_LOG_SIZE);
        ASSERT_EQ(configuration->getMemoryBytesRelaxed(), ONE_MEGABYTE);
        ASSERT_EQ(configuration->getStorageDirectory(), Configuration::DEFAULT_STORAGE_DIRECTORY + '/');
        ASSERT_EQ(configuration->getLocale(), Configuration::DEFAULT_LOCALE);
        ASSERT_EQ(configuration->getMode(), Configuration::DEFAULT_MODE);
        ASSERT_EQ(configuration->getTabWidth(), Configuration::DEFAULT_TAB_WIDTH);
    }

    TEST_F(ConfigTest, partiallyDefaultConfiguration)
    {
        // before
        // when
        // then
        std::unique_ptr<const Configuration> configuration(new Configuration(PARTIAL_CONFIG_FILE, log));
        ASSERT_TRUE(configuration);
        ASSERT_STREQ(configuration->getConfigurationFileName().c_str(), std::string("./tst/ut/helpers/" + PARTIAL_CONFIG_FILE).c_str());
        ASSERT_EQ(configuration->isCacheOn(), false);
        ASSERT_EQ(configuration->getCacheSize(), 1048576UL);
        ASSERT_EQ(configuration->getMaxLogFileSize(), Configuration::DEFAULT_MAX_LOG_SIZE);
        ASSERT_EQ(configuration->getMemoryBytesRelaxed(), ONE_MEGABYTE);
        ASSERT_EQ(configuration->getStorageDirectory(), Configuration::DEFAULT_STORAGE_DIRECTORY + '/');
        ASSERT_EQ(configuration->getLocale(), Configuration::DEFAULT_LOCALE);
        ASSERT_EQ(configuration->getMode(), READ_MODE::TEXT);
        ASSERT_EQ(configuration->getTabWidth(), 4UL);
    }

    TEST_F(ConfigTest, configurationFileWithWrongDataTypes)
    {
        // before
        // when
        // then
        std::unique_ptr<const Configuration> configuration(new Configuration(WRONG_DATA_TYPES_CONFIG_FILE, log));
        ASSERT_TRUE(configuration);
        ASSERT_STREQ(configuration->getConfigurationFileName().c_str(), std::string("./tst/ut/helpers/" + WRONG_DATA_TYPES_CONFIG_FILE).c_str());
        ASSERT_EQ(configuration->isCacheOn(), false);
        ASSERT_EQ(configuration->getCacheSize(), 1048576UL);
        ASSERT_EQ(configuration->getMaxLogFileSize(), Configuration::DEFAULT_MAX_LOG_SIZE);
        ASSERT_EQ(configuration->getMemoryBytesRelaxed(), ONE_MEGABYTE);
        ASSERT_EQ(configuration->getStorageDirectory(), Configuration::DEFAULT_STORAGE_DIRECTORY + '/');
        ASSERT_EQ(configuration->getLocale(), Configuration::DEFAULT_LOCALE);
        ASSERT_EQ(configuration->getMode(), READ_MODE::TEXT);
        ASSERT_EQ(configuration->getTabWidth(), 4UL);
    }

    TEST_F(ConfigTest, validConfigurationFile)
    {
        // before
        // when
        // then
        std::unique_ptr<const Configuration> configuration(new Configuration(PROPER_CONFIG_FILE, log));
        ASSERT_TRUE(configuration);
        ASSERT_STREQ(configuration->getConfigurationFileName().c_str(), std::string("./tst/ut/helpers/" + PROPER_CONFIG_FILE).c_str());
        ASSERT_EQ(configuration->isCacheOn(), false);
        ASSERT_EQ(configuration->getCacheSize(), 1048576UL);
        ASSERT_EQ(configuration->getMaxLogFileSize(), 1024);
        ASSERT_EQ(configuration->getMemoryBytesRelaxed(), 1048576);
        ASSERT_EQ(configuration->getStorageDirectory(), "/tmp/");
        ASSERT_EQ(configuration->getLocale(), "en_CA.UTF-8");
        ASSERT_EQ(configuration->getMode(), READ_MODE::TEXT);
        ASSERT_EQ(configuration->getTabWidth(), 4UL);
    }
} // namespace feather::config
