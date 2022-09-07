#include "cache/Cache.hpp"
#include "helpers/TestBase.hpp"
#include "utils/exception/FeatherCacheMissException.hpp"

#include <gtest/gtest.h>

using namespace feather::cache;

namespace feather::cache
{
  class CacheTest : public ::testing::Test, public ::feather::test::TestBase
  {
  };

  TEST_F(CacheTest, throwIfNotInCache)
  {
    // before
    std::unique_ptr<Cache> cache = std::make_unique<Cache>();

    // then
    ASSERT_THROW(cache->getChanges(TEST_UUID, 1, 0, 1), feather::utils::exception::FeatherCacheMissException);
  }

  TEST_F(CacheTest, addChanges)
  {
    // before
    std::unique_ptr<Cache> cache = std::make_unique<Cache>();

    // when
    // then
    ASSERT_NO_THROW(cache->addChanges(TEST_UUID, pair(1, 0), TEN_BYTE_UTF8_CHARACTER, 10UL));

    // when
    ASSERT_NO_THROW(cache->addChanges(TEST_UUID, pair(1, 0), std::vector<char>{'t'}, 1UL));
    ASSERT_NO_THROW(cache->addChanges(TEST_UUID, pair(1, 0), TWO_BYTE_UTF8_CHARACTER, 2UL));

    // then
    ASSERT_THROW(cache->addChanges(TEST_UUID, pair(2, 1), FOUR_BYTE_UTF8_CHARACTER, 4UL), feather::utils::exception::FeatherCacheMissException);
    ASSERT_TRUE(cache->isCharacterInCache(TEST_UUID, 1));
    ASSERT_FALSE(cache->isCharacterInCache(TEST_UUID, 0));
  }

  TEST_F(CacheTest, getChanges)
  {
    // before
    configuration->forceTextMode(testFile);
    std::unique_ptr<Cache> cache = std::make_unique<Cache>();

    // when
    // then
    ASSERT_THROW(cache->getChanges(TEST_UUID, 1UL, 0UL, 4UL), feather::utils::exception::FeatherCacheMissException);

    // when
    cache->addChanges(TEST_UUID, pair(0, 0), std::vector<char>{'t'}, 1UL);
    cache->addChanges(TEST_UUID, pair(0, 1), std::vector<char>{'e'}, 1UL);
    cache->addChanges(TEST_UUID, pair(0, 2), FOUR_BYTE_UTF8_CHARACTER, 4UL);

    // then
    ASSERT_THROW(cache->getChanges(TEST_UUID, 1UL, 7UL, 1UL), feather::utils::exception::FeatherCacheMissException);
    ASSERT_THROW(cache->getChanges(TEST_UUID, 1UL, 0UL, 7UL), feather::utils::exception::FeatherCacheMissException);
    auto changes = cache->getChanges(TEST_UUID, 0, 0, 2UL);
    ASSERT_EQ(changes[0], 't');
    ASSERT_EQ(changes[1], 'e');
    changes = cache->getChanges(TEST_UUID, 0, 2, 4UL);
    ASSERT_EQ(changes, FOUR_BYTE_UTF8_CHARACTER);
    changes = cache->getChanges(TEST_UUID, 0, 0, 3UL);
    ASSERT_EQ(changes, (std::vector<char>{'t', 'e', (char)0xF0}));
  }

  TEST_F(CacheTest, removeChanges)
  {
    // before
    std::unique_ptr<Cache> cache = std::make_unique<Cache>();

    // when
    cache->addChanges(TEST_UUID, pair(0, 0), std::vector<char>{'t'}, 1);
    cache->removeChanges(TEST_UUID, 1UL);

    // then
    ASSERT_EQ(cache->currCacheSize, 1UL);

    // when
    cache->removeChanges(TEST_UUID, 0UL);
    ASSERT_EQ(cache->currCacheSize, 0UL);
  }
}