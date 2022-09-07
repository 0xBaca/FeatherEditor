#include "utils/Utf8Util.hpp"
#include "utils/algorithm/SearchBuffer.hpp"

namespace feather::utils::algorithm
{
    SearchBuffer::SearchBuffer(utils::datatypes::Uuid const &windowUUIDArg, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestratorArg, pair const &startPosition)
        : windowUUID(windowUUIDArg), printingOrchestrator(printingOrchestratorArg), currPosition(startPosition), currIdx(0ULL), size(0ULL)
    {
    }

    void SearchBuffer::addEntry(SearchBufferEntry &&entryArg)
    {
        size += entryArg.getSize();
        buffer.emplace_back(std::move(entryArg));
    }

    void SearchBuffer::addEntryToFront(SearchBufferEntry &&entryArg)
    {
        size += entryArg.getSize();
        buffer.emplace_front(std::move(entryArg));
    }

    pair const &SearchBuffer::getCurrentPosition() const
    {
        return currPosition;
    }

    char32_t SearchBuffer::getNextCharacter()
    {
        char32_t nextCharacter = buffer.front().getNextEntry();
        size_t bytesTakenByCharacter = Utf8Util::getCharacterBytesTakenLight(nextCharacter);
        currIdx += bytesTakenByCharacter;
        currPosition = utils::helpers::Lambda::moveVirtualPositionForward(currPosition, bytesTakenByCharacter, printingOrchestrator.get(), windowUUID, false);

        if (buffer.front().isWholeEntryScannedForward())
        {
            buffer.pop_front();
        }
        return nextCharacter;
    }

    char32_t SearchBuffer::getPreviousCharacter()
    {
        char32_t previousCharacter = buffer.back().getPreviousEntry();
        size_t bytesTakenByCharacter = Utf8Util::getCharacterBytesTakenLight(previousCharacter);
        currIdx += bytesTakenByCharacter;
        currPosition = utils::helpers::Lambda::moveVirtualPositionBackward(currPosition, bytesTakenByCharacter, printingOrchestrator.get(), windowUUID);
        
        if (buffer.back().isWholeEntryScannedBackward())
        {
            buffer.pop_back();
        }
        return previousCharacter;
    }

    size_t SearchBuffer::getNoUnreadBytes() const
    {
        return getSize() - currIdx;
    }

    size_t SearchBuffer::getNoUnreadBytes(size_t tolerance) const
    {
        if (getSize() - currIdx < tolerance)
        {
            return 0;
        }
        return getSize() - currIdx;
    }

    size_t SearchBuffer::getSize() const
    {
        return size;
    }

    void SearchBuffer::updateCurrPos(pair pos)
    {
        currPosition = pos;
    }

    bool SearchBuffer::isBufferEmpty() const
    {
        return buffer.empty();
    }
} // namespace feather::utils::algorithm
