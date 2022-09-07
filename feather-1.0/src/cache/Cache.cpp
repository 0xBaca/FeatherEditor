#include "cache/Cache.hpp"
#include "utils/exception/FeatherMemoryException.hpp"
#include "utils/exception/FeatherCacheMissException.hpp"
#include "utils/Utf8Util.hpp"

namespace feather::cache
{
    Cache::Cache() : currCacheSize(0), cacheList(std::make_unique<List>())
    {
    }

    void Cache::addChanges(utils::datatypes::Uuid const &windowUUID, pair pos, std::vector<char> const &buffer, size_t bufferSize)
    {
        if (!isCharacterInCache(windowUUID, pos.first))
        {
            if (pos.second > 0)
            {
                throw utils::exception::FeatherCacheMissException("Already dumped");
            }
            changesCache[windowUUID][pos.first] = cacheList->addNewNode(windowUUID, pos.first);
        }
        auto node = cacheList->moveToBegining(changesCache[windowUUID][pos.first]);
        node->addChanges(buffer, bufferSize, pos.second);
        currCacheSize += bufferSize;
    }

    std::tuple<utils::datatypes::Uuid, size_t> Cache::getCacheLeastRecentlyUsedInfo()
    {
        return cacheList->getLeastRecentlyUsed()->getChangesInfo();
    }

    std::vector<char> Cache::getChanges(utils::datatypes::Uuid const &windowUUID, size_t pos, size_t offset, size_t size)
    {
        if (!isCharacterInCache(windowUUID, pos))
        {
            throw utils::exception::FeatherCacheMissException("Changes starting at", pos, "are not in cache");
        }
        auto const &rawValues = changesCache[windowUUID][pos]->getElements();
        if (rawValues.size() <= offset || rawValues.size() < offset + size)
        {
            throw utils::exception::FeatherCacheMissException("Changes", pos, " : ", offset, "not in cache");
        }
        std::vector<char> toReturn(std::next(rawValues.begin(), offset), std::next(rawValues.begin(), offset + size));
        //size_t lastValidUtf8CharacterEndPosition = utils::Utf8Util::getLastValidEndOfUtf8CharacterPosition(toReturn, size);
        //toReturn.resize(lastValidUtf8CharacterEndPosition);
        return toReturn;
    }

    bool Cache::isCharacterInCache(utils::datatypes::Uuid const &windowUUID, size_t pos)
    {
        return changesCache.count(windowUUID) && changesCache[windowUUID].count(pos);
    }

    void Cache::removeChanges(utils::datatypes::Uuid const &windowUUID, size_t pos)
    {
        if (!isCharacterInCache(windowUUID, pos))
        {
            return;
        }
        currCacheSize -= changesCache[windowUUID][pos]->getSize();
        cacheList->removeNode(changesCache[windowUUID][pos]);
        changesCache[windowUUID].erase(pos);
    }
} // namespace feather::cache
