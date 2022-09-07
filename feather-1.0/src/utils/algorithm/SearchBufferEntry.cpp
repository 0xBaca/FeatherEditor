#include "utils/Utf8Util.hpp"
#include "utils/algorithm/SearchBufferEntry.hpp"

namespace feather::utils::algorithm
{
    SearchBufferEntry::SearchBufferEntry(SearchBufferEntry &&other)
    {
        currEntryForwardIdx = other.currEntryForwardIdx;
        currEntryBackwardIdx = other.currEntryBackwardIdx;
        buffer = std::move(other.buffer);
    }

    SearchBufferEntry::SearchBufferEntry(std::vector<char> &&bufferArg)
        : buffer(std::move(bufferArg)), currEntryForwardIdx(0), currEntryBackwardIdx(buffer.size())
    {
    }

    size_t SearchBufferEntry::getSize() const
    {
        return buffer.size();
    }

    bool SearchBufferEntry::isWholeEntryScannedForward() const
    {
        return (currEntryForwardIdx == buffer.size());
    }

    bool SearchBufferEntry::isWholeEntryScannedBackward() const
    {
        return (currEntryBackwardIdx == 0);
    }

    char32_t SearchBufferEntry::getNextEntry()
    {
        unsigned short entrySize = utils::Utf8Util::getCharacterBytesTaken(utils::helpers::Lambda::getSubVector(buffer, currEntryForwardIdx, MAX_UTF8_CHAR_BYTES));
        char32_t toReturn = 0ULL;
        memcpy((void *)&toReturn, (void *)&buffer[currEntryForwardIdx], entrySize);
        currEntryForwardIdx += entrySize;
        return toReturn;
    }

    char32_t SearchBufferEntry::getPreviousEntry()
    {
        char32_t toReturn = 0;
        size_t entrySize = utils::Utf8Util::getCharacterBytesTakenBackward((currEntryBackwardIdx < MAX_UTF8_CHAR_BYTES) ? std::vector<char>{buffer.begin(), std::next(buffer.begin(), currEntryBackwardIdx)} : utils::helpers::Lambda::getSubVector(buffer, currEntryBackwardIdx - MAX_UTF8_CHAR_BYTES, MAX_UTF8_CHAR_BYTES));
        currEntryBackwardIdx -= entrySize;
        memcpy((void *)&toReturn, (void *)&buffer[currEntryBackwardIdx], entrySize);
        return toReturn;
    }

    char32_t SearchBufferEntry::operator[](size_t idx)
    {
        return buffer[idx];
    }

    SearchBufferEntry::~SearchBufferEntry()
    {
    }
} // namespace feather::utils::algorithm
