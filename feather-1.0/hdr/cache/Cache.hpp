#pragma once

#ifdef _FEATHER_TEST_
#include "googletest/include/gtest/gtest_prod.h"
#endif

#include "cache/List.hpp"
#include "utils/datatypes/Global.hpp"
#include "utils/datatypes/UuidHash.hpp"

#include <unordered_map>

namespace feather::cache
{
  class Cache
  {
#ifdef _FEATHER_TEST_
    FRIEND_TEST(CacheTest, removeChanges);
#endif
  public:
    explicit Cache();
    void addChanges(utils::datatypes::Uuid const &, pair, std::vector<char> const &, size_t);
    std::tuple<utils::datatypes::Uuid, size_t> getCacheLeastRecentlyUsedInfo();
    std::vector<char> getChanges(utils::datatypes::Uuid const &, size_t, size_t, size_t);
    bool isCharacterInCache(utils::datatypes::Uuid const &, size_t);
    void removeChanges(utils::datatypes::Uuid const &, size_t);

  private:
    size_t currCacheSize;
    std::unique_ptr<List> cacheList;
    std::unordered_map<utils::datatypes::Uuid, std::unordered_map<size_t, std::shared_ptr<typename List::Node>>, utils::datatypes::UuidHasher> changesCache;
  };
} // namespace feather::cache
