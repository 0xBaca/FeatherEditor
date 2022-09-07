#pragma once

#include <vector>

namespace feather::utils::algorithm
{
    class SearchBufferEntry
    {
    private:
        std::vector<char> buffer;
        size_t currEntryForwardIdx, currEntryBackwardIdx;

    public:
        SearchBufferEntry(SearchBufferEntry &&);
        SearchBufferEntry(std::vector<char> &&);
        size_t getSize() const;
        bool isWholeEntryScannedForward() const;
        bool isWholeEntryScannedBackward() const;
        char32_t getNextEntry();
        char32_t getPreviousEntry();
        char32_t operator[](size_t);
        ~SearchBufferEntry();
    };
} // namespace feather::utils::algorithm
