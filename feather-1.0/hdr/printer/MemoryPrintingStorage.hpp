#pragma once

#include "cache/Cache.hpp"
#include "printer/PrintingStorageInterface.hpp"

namespace feather::printer
{
  class MemoryPrintingStorage : public PrintingStorageInterface
  {
  private:
    std::unique_ptr<cache::Cache> cache;

  public:
    MemoryPrintingStorage();
    void addToNewChanges(utils::datatypes::Uuid const &, pair, std::vector<char> const &, size_t) override;
    std::tuple<utils::datatypes::Uuid, size_t> getCacheLeastRecentlyUsedInfo();
    std::vector<char> getNewChanges(utils::datatypes::Uuid const &, size_t, size_t, size_t) override;
    bool isChangesInMemory(utils::datatypes::Uuid const &, size_t);
    void removeChanges(utils::datatypes::Uuid const &, size_t) override;
    void dumpChanges(utils::datatypes::Uuid const &, pair, std::vector<char> const &) override;
    void dumpColors(utils::datatypes::Uuid const &, std::multimap<std::pair<pair, pair>, std::pair<size_t, std::set<utils::datatypes::COLOR>>> const &, std::unordered_multimap<std::u32string, std::pair<size_t, std::set<utils::datatypes::COLOR>>> const &, size_t) override;
    void dumpDeletions(utils::datatypes::Uuid const &, std::map<pair, pair> const &) override;
  };
} // namespace feather::printer