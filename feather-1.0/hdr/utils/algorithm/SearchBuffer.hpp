#pragma once

#include "printer/PrintingOrchestratorInterface.hpp"
#include "utils/algorithm/SearchBufferEntry.hpp"

#include <list>

namespace feather::utils::algorithm
{
    class SearchBuffer
    {
    private:
        utils::datatypes::Uuid const &windowUUID;
        std::list<SearchBufferEntry> buffer;
        std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator;
        pair currPosition;
        size_t currIdx;
        size_t size;

    public:
        SearchBuffer(utils::datatypes::Uuid const &, std::shared_ptr<printer::PrintingOrchestratorInterface>, pair const &);
        void addEntry(SearchBufferEntry &&);
        void addEntryToFront(SearchBufferEntry &&);
        pair const &getCurrentPosition() const;
        char32_t getNextCharacter();
        char32_t getPreviousCharacter();
        size_t getNoUnreadBytes() const;
        size_t getNoUnreadBytes(size_t) const;
        size_t getSize() const;
        void updateCurrPos(pair);
        bool isBufferEmpty() const;
    };
} // namespace feather::utils::algorithm
