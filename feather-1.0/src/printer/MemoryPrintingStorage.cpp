#include "printer/MemoryPrintingStorage.hpp"

namespace feather::printer
{
    MemoryPrintingStorage::MemoryPrintingStorage()
    {
        cache = std::make_unique<cache::Cache>();
    }

    void MemoryPrintingStorage::addToNewChanges(utils::datatypes::Uuid const &windowUUID, pair pos, std::vector<char> const &buffer, size_t bufferSize)
    {
        cache->addChanges(windowUUID, pos, buffer, bufferSize);
    }

    std::tuple<utils::datatypes::Uuid, size_t> MemoryPrintingStorage::getCacheLeastRecentlyUsedInfo()
    {
        return cache->getCacheLeastRecentlyUsedInfo();
    }

    std::vector<char> MemoryPrintingStorage::getNewChanges(utils::datatypes::Uuid const &windowUUID, size_t pos, size_t offset, size_t bytesToRead)
    {
        return cache->getChanges(windowUUID, pos, offset, bytesToRead);
    }

    bool MemoryPrintingStorage::isChangesInMemory(utils::datatypes::Uuid const &windowUUID, size_t pos)
    {
        return cache->isCharacterInCache(windowUUID, pos);
    }

    void MemoryPrintingStorage::removeChanges(utils::datatypes::Uuid const &windowUUID, size_t pos)
    {
        cache->removeChanges(windowUUID, pos);
    }

    void MemoryPrintingStorage::dumpChanges(utils::datatypes::Uuid const &, pair, std::vector<char> const &)
    {
        //EMPTY
    }

    void MemoryPrintingStorage::dumpDeletions(utils::datatypes::Uuid const &windowUUID, std::map<pair, pair> const &deletions)
    {
        //EMPTY
    }

    void MemoryPrintingStorage::dumpColors(utils::datatypes::Uuid const &windowUUID, std::multimap<std::pair<pair, pair>, std::pair<size_t, std::set<utils::datatypes::COLOR>>> const &, std::unordered_multimap<std::u32string, std::pair<size_t, std::set<utils::datatypes::COLOR>>> const &, size_t)
    {
        //EMPTY
    }
} // namespace feather::printer
