#include "helpers/TestBase.hpp"
#include "utils/ProgramOptionsParser.hpp"
#include "utils/algorithm/SearchZooKeeper.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace feather::utils::algorithm
{
    class SearchZooKeeperTest : public ::testing::Test, public ::feather::test::TestBase
    {
    public:
        void SetUp() override
        {
            const char *commandLineArguments[] = {"./feather", "file", "--relax=5", "--mode=text"};
            utils::ProgramOptionsParser::parseArguments(4UL, commandLineArguments);
            configuration->overrideConfiguration();
        }

        void TearDown() override
        {
        }
    };

    TEST_F(SearchZooKeeperTest, getFirstIntervals)
    {
        SearchZooKeeper zooKeeper = SearchZooKeeper(10UL, 20UL);
        ASSERT_EQ(zooKeeper.getNextInterval().value(), pair(10UL, 10UL));
        ASSERT_EQ(zooKeeper.getPrevInterval().value(), pair(15UL, 5UL));
        ASSERT_EQ(zooKeeper.bytesTaken(), 10UL);

        ASSERT_EQ(zooKeeper.getNextInterval().value(), pair(5UL, 15UL));
        ASSERT_EQ(zooKeeper.getPrevInterval().value(), pair(20UL, 0UL));
        ASSERT_EQ(zooKeeper.bytesTaken(), 20UL);

        ASSERT_EQ(zooKeeper.getNextInterval(), std::nullopt);
        ASSERT_EQ(zooKeeper.getPrevInterval(), std::nullopt);
    }

    TEST_F(SearchZooKeeperTest, nextWarpsFirst)
    {
        SearchZooKeeper zooKeeper = SearchZooKeeper(10UL, 20UL);
        ASSERT_EQ(zooKeeper.getNextInterval().value(), pair(10UL, 10UL));
        ASSERT_EQ(zooKeeper.getPrevInterval().value(), pair(15UL, 5UL));
        ASSERT_EQ(zooKeeper.bytesTaken(), 10UL);

        ASSERT_EQ(zooKeeper.getNextInterval(), pair(5UL, 15UL));
        ASSERT_EQ(zooKeeper.bytesTaken(), 15UL);

        ASSERT_EQ(zooKeeper.getNextInterval(), pair(10UL, 0UL));
        ASSERT_EQ(zooKeeper.bytesTaken(), 20UL);

        ASSERT_EQ(zooKeeper.getPrevInterval(), std::nullopt);
        ASSERT_EQ(zooKeeper.getNextInterval(), std::nullopt);
        ASSERT_EQ(zooKeeper.bytesTaken(), 20UL);
    }

    TEST_F(SearchZooKeeperTest, prevWarpsFirst)
    {
        SearchZooKeeper zooKeeper = SearchZooKeeper(10UL, 20UL);
        ASSERT_EQ(zooKeeper.getPrevInterval().value(), pair(15UL, 5UL));
        ASSERT_EQ(zooKeeper.getPrevInterval().value(), pair(20UL, 0UL));
        ASSERT_EQ(zooKeeper.bytesTaken(), 10UL);

        ASSERT_EQ(zooKeeper.getPrevInterval(), pair(5UL, 15UL));
        ASSERT_EQ(zooKeeper.bytesTaken(), 15UL);

        ASSERT_EQ(zooKeeper.getPrevInterval(), pair(10UL, 10UL));
        ASSERT_EQ(zooKeeper.getNextInterval(), std::nullopt);
        ASSERT_EQ(zooKeeper.getPrevInterval(), std::nullopt);
        ASSERT_EQ(zooKeeper.bytesTaken(), 20UL);
    }

    TEST_F(SearchZooKeeperTest, prevWarpsFirstStartingAtFirstPos)
    {
        SearchZooKeeper zooKeeper = SearchZooKeeper(10UL, 20UL);
        zooKeeper = SearchZooKeeper(0, 20UL);

        ASSERT_EQ(zooKeeper.getPrevInterval().value(), pair(5UL, 15UL));
        ASSERT_EQ(zooKeeper.getNextInterval().value(), pair(20UL, 0));
        ASSERT_EQ(zooKeeper.bytesTaken(), 10UL);
    }

    TEST_F(SearchZooKeeperTest, getNextAndPrevIntervals)
    {
        SearchZooKeeper zooKeeper = SearchZooKeeper(8UL, 11UL);

        ASSERT_EQ(zooKeeper.getPrevInterval().value(), pair(8UL, 3UL));
        ASSERT_EQ(zooKeeper.getNextInterval().value(), pair(3UL, 8UL));
        ASSERT_EQ(zooKeeper.bytesTaken(), 8UL);
    }
}