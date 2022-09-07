#include "reader/KeyStroke.hpp"
#include "utils/BufferFiller.hpp"
#include "utils/Utf8Util.hpp"
#include "utils/algorithm/SearchBufferEntry.hpp"
#include "utils/algorithm/SearchResultsController.hpp"

#include <filesystem>

extern std::unique_ptr<const feather::config::Configuration> configuration;

namespace feather::utils
{
    BufferFiller::BufferFiller(std::shared_ptr<utils::storage::AbstractStorage> storageArg, std::shared_ptr<utils::storage::AbstractStorageFactory> storageFactoryArg)
        : storage(storageArg), storageFactory(storageFactoryArg)
    {
    }

    utils::windows::FramePositions const &BufferFiller::getFramePositions() const
    {
        return framePosition;
    }

    ScreenBuffer const &BufferFiller::getNewWindowBuffer(feather::windows::MainWindowInterface *window, FilledChunk chunk, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator)
    {
#ifndef _FEATHER_TEST_
        if (!std::filesystem::exists(storage->getName()))
        {
            throw std::filesystem::filesystem_error("File " + storage->getName() + " is not available anymore...", std::make_error_code(std::io_errc::stream));
        }
#endif
        //Automatically reload file content
        bool cursorShouldBeSetAtFirstPos = false;
        auto realoadResult = window->getStorage()->reload();
        if (std::nullopt != realoadResult && realoadResult->first > realoadResult->second)
        {
            pair const lastValidPos = printingOrchestrator->getLastValidPosition(window->getUUID(), window->getStorage());
            utils::algorithm::SearchResultsController::clearResults(window->getUUID());
            reader::KeyStroke::cleanBreakPoints(lastValidPos, window->getUUID());
            if (lastValidPos < framePosition.startFramePosition)
            {
                setStartFramePosition(printingOrchestrator->getLastValidPosition(window->getUUID(), window->getStorage()));
                setEndFramePosition(printingOrchestrator->getLastValidPosition(window->getUUID(), window->getStorage()));
                rewindLines(window->getWindowDimensions().first, window->getWindowDimensions().second, window, printingOrchestrator);
                cursorShouldBeSetAtFirstPos = true;
            }
        }
        std::unique_ptr<ScreenBuffer> newScreenBuffer = std::make_unique<ScreenBuffer>(0UL, window->getWindowDimensions().first, window->getWindowDimensions().second);
        //It can turn out our start frame is deleted
        if (printingOrchestrator->isCharAtPosDeleted(framePosition.startFramePosition, window->getUUID()))
        {
            setStartFramePosition(printingOrchestrator->getPreviousVirtualPosition(window->getUUID(), window->getStorage(), printingOrchestrator->getDeletionStart(window->getUUID(), framePosition.startFramePosition)));
            //Now it can turn out deletions starts at pos (0, 0)
            if (printingOrchestrator->isCharAtPosDeleted(framePosition.startFramePosition, window->getUUID()))
            {
                setStartFramePosition(printingOrchestrator->getFirstValidPosition(window->getUUID()));
            }
        }

        switch (chunk)
        {
        case FilledChunk::CURR_FRAME:
            break;
        case FilledChunk::LINE_UP:
            rewindLines(1UL, window->getWindowDimensions().second, window, printingOrchestrator);
            break;
        case FilledChunk::LINE_DOWN:
            forwardLines(1UL, window, printingOrchestrator);
            break;
        case FilledChunk::HALF_FRAME_DOWN:
            forwardLines(window->getWindowDimensions().first / 2, window, printingOrchestrator);
            break;
        case FilledChunk::HALF_FRAME_UP:
            rewindLines(window->getWindowDimensions().first / 2, window->getWindowDimensions().second, window, printingOrchestrator);
            break;
        case FilledChunk::FRAME_UP:
            rewindLines(window->getWindowDimensions().first, window->getWindowDimensions().second, window, printingOrchestrator);
            break;
        case FilledChunk::FRAME_DOWN:
            forwardLines(window->getWindowDimensions().first, window, printingOrchestrator);
            break;
        }
        fillFrameBuffer(*newScreenBuffer, window, printingOrchestrator);
        screenBuffer.swap(newScreenBuffer);
        if (cursorShouldBeSetAtFirstPos)
        {
            window->setCursorPosition(pair(0, 0));
        }
        return *screenBuffer;
    }

    ScreenBuffer const &BufferFiller::getScreenBuffer() const
    {
        return *screenBuffer;
    }

    void BufferFiller::fillFrameBuffer(ScreenBuffer &rawBuffer, feather::windows::MainWindowInterface *window, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator)
    {
        size_t linesExtracted = 0;
        size_t linesToExtract = window->getWindowDimensions().first;
        size_t columnsRead = 0;
        std::u32string lineWithChanges;
        lineWithChanges.reserve(window->getWindowDimensions().second);
        if (framePosition.startFramePosition < printingOrchestrator->getFirstValidPosition(window->getUUID()))
        {
            auto firstValidPosition = printingOrchestrator->getFirstValidPosition(window->getUUID());
            setStartFramePosition(printingOrchestrator->getDeletionStart(window->getUUID(), framePosition.startFramePosition));
            framePosition.endFramePosition = firstValidPosition;
        }
        utils::windows::FramePositions oldFramePos(framePosition);
        auto localBuffer = fillCacheBuffer(window->getUUID(), printingOrchestrator, MAX_UTF8_CHAR_BYTES * window->getWindowDimensions().first * window->getWindowDimensions().second, framePosition.startFramePosition);
        std::optional<pair> frameLastPosition = std::nullopt;
        pair frameLastValidPosition = framePosition.endFramePosition;
        while (linesExtracted < linesToExtract && localBuffer->getNoUnreadBytes())
        {
            pair posBeforeCharacter = frameLastValidPosition;
            frameLastValidPosition = localBuffer->getCurrentPosition();
            char32_t currCharacter = localBuffer->getNextCharacter();
            unsigned long characterWidth = utils::helpers::Lambda::getCharacterWidth(currCharacter, true, window->isSecondaryHexModeMainWindow());
            if (utils::helpers::Lambda::isNewLineChar(currCharacter) && !configuration->isHexMode())
            {
                if (columnsRead < window->getWindowDimensions().second)
                {
                    lineWithChanges.push_back(currCharacter);
                    rawBuffer.fillBufferLine(lineWithChanges);
                    lineWithChanges.clear();
                    ++linesExtracted;
                    columnsRead = 0UL;
                    continue;
                }
                rawBuffer.fillBufferLine(lineWithChanges);
                lineWithChanges.clear();
                ++linesExtracted;
                if (linesExtracted == linesToExtract)
                {
                    continue;
                }
                lineWithChanges.push_back(currCharacter);
                rawBuffer.fillBufferLine(lineWithChanges);
                lineWithChanges.clear();
                columnsRead = 0UL;
                ++linesExtracted;
                continue;
            }
            else if (columnsRead + characterWidth > window->getWindowDimensions().second)
            {
                frameLastPosition = posBeforeCharacter;
                rawBuffer.fillBufferLine(lineWithChanges);
                lineWithChanges.clear();
                lineWithChanges.push_back(currCharacter);
                columnsRead = utils::helpers::Lambda::getCharacterWidth(currCharacter, true, window->isSecondaryHexModeMainWindow());
                ++linesExtracted;
                if (linesExtracted == linesToExtract)
                {
                    break;
                }
                frameLastPosition = std::nullopt;
                continue;
            }
            columnsRead += characterWidth;
            lineWithChanges.push_back(currCharacter);
        }
        //When reaching EOF, push last line on screen
        if (linesExtracted < linesToExtract && !lineWithChanges.empty())
        {
            rawBuffer.fillBufferLine(lineWithChanges);
        }
        framePosition.endFramePosition = frameLastPosition.has_value() ? frameLastPosition.value() : frameLastValidPosition;
        //It can turn out our last pos has changed and it is deleted now, we need to align position accordingly
        if (printingOrchestrator->isCharAtPosDeleted(framePosition.endFramePosition, window->getUUID()))
        {
            pair deletionStart = printingOrchestrator->getDeletionStart(window->getUUID(), framePosition.endFramePosition);
            pair deletionEnd = printingOrchestrator->getDeletionEnd(window->getUUID(), deletionStart);
            framePosition.endFramePosition = printingOrchestrator->getLastValidPosition(window->getUUID(), window->getStorage()) < deletionEnd ? printingOrchestrator->getPreviousVirtualPosition(window->getUUID(), window->getStorage(), deletionStart) : deletionEnd;
        }
        //File content was modified
        if (framePosition.endFramePosition > printingOrchestrator->getLastValidPosition(window->getUUID(), window->getStorage()))
        {
            framePosition.endFramePosition = printingOrchestrator->getLastValidPosition(window->getUUID(), window->getStorage());
        }
        framePosition.startFramePosition = oldFramePos.startFramePosition;
        storage->setCursor(framePosition.startFramePosition.first);
    }

    std::unique_ptr<utils::algorithm::SearchBuffer> BufferFiller::fillCacheBuffer(utils::datatypes::Uuid const &windowUUID, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator, size_t size, pair currPos)
    {
        std::unique_ptr<utils::algorithm::SearchBuffer> localBuffer = std::make_unique<utils::algorithm::SearchBuffer>(windowUUID, printingOrchestrator, currPos);
        localBuffer->addEntry(printingOrchestrator->getContinousCharacters(currPos, size, storage, windowUUID).first);
        return localBuffer;
    }

    std::unique_ptr<utils::algorithm::SearchBuffer> BufferFiller::fillCacheBufferBackward(utils::datatypes::Uuid const &windowUUID, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator, size_t size, pair currPos)
    {
        pair const firstValidPosition = printingOrchestrator->getFirstValidPosition(windowUUID);
        std::unique_ptr<utils::algorithm::SearchBuffer> localBuffer = std::make_unique<utils::algorithm::SearchBuffer>(windowUUID, printingOrchestrator, currPos);
        while (size)
        {
            auto data = printingOrchestrator->getContinousCharactersBackward(currPos, std::min(configuration->getMemoryBytesRelaxed(), size), storage, windowUUID);
            if (data.first.empty())
            {
                return localBuffer;
            }
            else if (data.second == firstValidPosition)
            {
                localBuffer->addEntryToFront(std::move(data.first));
                return localBuffer;
            }
            size -= data.first.size();
            localBuffer->addEntryToFront(std::move(data.first));
            currPos = data.second;
        }
        return localBuffer;
    }

    std::shared_ptr<utils::storage::AbstractStorage> const &BufferFiller::getStorage() const
    {
        return storage;
    }

    bool BufferFiller::isFrameAtLastPosition(std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestartor, utils::datatypes::Uuid const &windowUUID)
    {
        return (framePosition.endFramePosition == printingOrchestartor->getLastValidPosition(windowUUID, storage));
    }

    void BufferFiller::forwardLines(size_t linesToForward, feather::windows::MainWindowInterface *window, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator)
    {
        size_t noColumnsInLine = window->getWindowDimensions().second;
        size_t columnsRead = 0;
        size_t noNewLinesRead = 0;
        pair const lastValidPosition = printingOrchestrator->getLastValidPosition(window->getUUID(), storage);

        //Can't go any further down
        if (isFrameAtLastPosition(printingOrchestrator, window->getUUID()))
        {
            return;
        }
        else if (framePosition.startFramePosition < printingOrchestrator->getFirstValidPosition(window->getUUID()))
        {
            auto firstValidPosition = printingOrchestrator->getFirstValidPosition(window->getUUID());
            storage->setCursor(firstValidPosition.first);
            framePosition.startFramePosition = firstValidPosition;
        }
        pair currCharacterPos = framePosition.startFramePosition;
        auto localBuffer = fillCacheBuffer(window->getUUID(), printingOrchestrator, (noColumnsInLine + 4UL) * (linesToForward + 1), currCharacterPos);
        while (noNewLinesRead != linesToForward)
        {
            if (!localBuffer->getNoUnreadBytes())
            {
                if (lastValidPosition == currCharacterPos)
                {
                    framePosition.startFramePosition = currCharacterPos;
                    return;
                }
                localBuffer = fillCacheBuffer(window->getUUID(), printingOrchestrator, (noColumnsInLine + 4UL) * linesToForward, printingOrchestrator->getNextVirtualPosition(window->getUUID(), storage, currCharacterPos));
            }
            char32_t currCharacter = localBuffer->getNextCharacter();
            if ((!configuration->isHexMode() && utils::helpers::Lambda::isNewLineChar(currCharacter)) || (columnsRead + utils::helpers::Lambda::getCharacterWidth(currCharacter, true, window->isSecondaryHexModeMainWindow()) == noColumnsInLine))
            {
                ++noNewLinesRead;
                columnsRead = 0;
                currCharacterPos = localBuffer->getCurrentPosition();
                continue;
            }
            else if (columnsRead + utils::helpers::Lambda::getCharacterWidth(currCharacter, true, window->isSecondaryHexModeMainWindow()) > noColumnsInLine)
            {
                ++noNewLinesRead;
                columnsRead = utils::helpers::Lambda::getCharacterWidth(currCharacter, true, window->isSecondaryHexModeMainWindow());
                currCharacterPos = localBuffer->getCurrentPosition();
                continue;
            }
            columnsRead += utils::helpers::Lambda::getCharacterWidth(currCharacter, true, window->isSecondaryHexModeMainWindow());
            currCharacterPos = localBuffer->getCurrentPosition();
        }
        framePosition.startFramePosition = currCharacterPos;
    }

    void BufferFiller::replaceMappedFile(std::shared_ptr<utils::storage::AbstractStorage> &newStorage, size_t newFramePos)
    {
        newStorage->setPermissions(storage->getPermissions());
        auto name = storage->getName();
        storageFactory->removeStorage(storage->getName());
        newStorage->changeName(storage->getName());
        storage.swap(newStorage);
        storage->setCursor(newFramePos);
        framePosition = utils::windows::FramePositions{pair{newFramePos, 0}, pair{0, 0}};
    }

    void BufferFiller::rewindLines(size_t linesToRewind, size_t maxNoColumns, feather::windows::MainWindowInterface *window, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator)
    {
        size_t columnsLeft = maxNoColumns;
        pair currCursorPosition = framePosition.startFramePosition;
        bool firstNewLineMeet = false;
        auto localBuffer = fillCacheBufferBackward(window->getUUID(), printingOrchestrator, ((maxNoColumns + 4UL) * (linesToRewind + 2UL)), currCursorPosition);
        while (linesToRewind)
        {
            //We can't go any further
            if (printingOrchestrator->isCharAtFirstPosition(currCursorPosition, window->getUUID()) || printingOrchestrator->isCharAtFirstPosition(localBuffer->getCurrentPosition(), window->getUUID()))
            {
                framePosition.startFramePosition = currCursorPosition;
                return;
            }
            char32_t previousCharacter = localBuffer->getPreviousCharacter();
            if (!configuration->isHexMode() && utils::helpers::Lambda::isNewLineChar(previousCharacter))
            {
                if (firstNewLineMeet)
                {
                    --linesToRewind;
                }
                else
                {
                    firstNewLineMeet = true;
                }
                columnsLeft = maxNoColumns;
                if (0 == linesToRewind)
                {
                    framePosition.startFramePosition = currCursorPosition;
                    return;
                }
            }
            if (columnsLeft == utils::helpers::Lambda::getCharacterWidth(previousCharacter, true, window->isSecondaryHexModeMainWindow()))
            {
                --linesToRewind;
                columnsLeft = maxNoColumns;
                if (0 == linesToRewind)
                {
                    framePosition.startFramePosition = localBuffer->getCurrentPosition();
                    return;
                }
                continue;
            }
            else if (columnsLeft < utils::helpers::Lambda::getCharacterWidth(previousCharacter, true, window->isSecondaryHexModeMainWindow()))
            {
                --linesToRewind;
                columnsLeft = maxNoColumns - utils::helpers::Lambda::getCharacterWidth(previousCharacter, true, window->isSecondaryHexModeMainWindow());
                if (0 == linesToRewind)
                {
                    framePosition.startFramePosition = currCursorPosition;
                    return;
                }
                currCursorPosition = localBuffer->getCurrentPosition();
                continue;
            }
            currCursorPosition = localBuffer->getCurrentPosition();
            columnsLeft -= utils::helpers::Lambda::getCharacterWidth(previousCharacter, true, window->isSecondaryHexModeMainWindow());
        }
    }

    void BufferFiller::setStartFramePosition(pair newPositions)
    {
        storage->setCursor(newPositions.first);
        framePosition.startFramePosition = newPositions;
    }

    void BufferFiller::setEndFramePosition(pair newPositions)
    {
        framePosition.endFramePosition = newPositions;
    }

    BufferFiller::~BufferFiller()
    {
    }
} // namespace feather::utils
