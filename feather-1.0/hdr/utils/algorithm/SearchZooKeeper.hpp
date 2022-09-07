#pragma once

#ifdef _FEATHER_TEST_
#include "googletest/include/gtest/gtest_prod.h"
#endif

#include <utils/datatypes/Global.hpp>

#include <optional>

namespace feather::utils::algorithm
{
    class SearchZooKeeper
    {
#ifdef _FEATHER_TEST_
        FRIEND_TEST(SearchZooKeeperTest, getFirstIntervals);
#endif
    private:
        size_t maxPos;
        size_t minIntervalSize;
        size_t nextPos, prevPos, middlePos;
        bool firstPrevIntervalFetch, nextIntervalHasRewinded, prevIntervalHasRewinded;
        static bool prevIntervalAleadyHasRewinded, nextIntervalAleadyHasRewinded;
        static constexpr size_t DELTA = 500;

    public:
        SearchZooKeeper();
        SearchZooKeeper(size_t, size_t);
        SearchZooKeeper &operator=(SearchZooKeeper const &);
        std::optional<pair> getNextInterval();
        std::optional<pair> getPrevInterval();
        size_t bytesTaken() const;
        bool hasNextIntervalRewinded() const;
        bool hasPrevIntervalRewinded() const;
    };
}
