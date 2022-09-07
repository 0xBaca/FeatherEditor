#include "printer/FilePrintingStorage.hpp"
#include "printer/MemoryPrintingStorage.hpp"
#include "printer/PrintingOrchestrator.hpp"
#include "utils/BufferFiller.hpp"
#include "utils/Filesystem.hpp"
#include "utils/ProgramOptionsParser.hpp"
#include "utils/Serializer.hpp"
#include "utils/datatypes/DeserializedChangesArchive.hpp"
#include "utils/datatypes/DeserializedDeletionsArchive.hpp"
#include "utils/datatypes/Strings.hpp"
#include "utils/exception/FeatherCacheMissException.hpp"
#include "utils/exception/FeatherFileConcurrentlyModifiedException.hpp"
#include "utils/exception/FeatherInvalidArgumnetException.hpp"
#include "utils/exception/FeatherMemoryException.hpp"
#include "utils/helpers/Conversion.hpp"
#include "utils/helpers/Lambda.hpp"
#include "utils/Utf8Util.hpp"

#include <cassert>

extern feather::utils::FEATHER_MODE currentFeatherMode;
extern std::unique_ptr<const feather::config::Configuration> configuration;

namespace feather::printer
{
    PrintingOrchestrator::PrintingOrchestrator(std::shared_ptr<utils::storage::AbstractStorageFactory> storageFactoryArg, std::shared_ptr<PrintingStorageInterface> primaryPrintingStorageArg, std::shared_ptr<PrintingStorageInterface> secondaryPrintingStorageArg, std::shared_ptr<printer::ChangesStack> changesStackArg, std::shared_ptr<utils::FilesystemInterface> filesystemArg, std::shared_ptr<utils::storage::AbstractStorage> const &storage)
        : storageFactory(storageFactoryArg), filesystem(filesystemArg), primaryPrintingStorage(primaryPrintingStorageArg), secondaryPrintingStorage(secondaryPrintingStorageArg), changesStack(changesStackArg)
    {
        loadExistingChanges(std::make_unique<utils::Filesystem>(), storage);
        lastStorageModificationTimestamp.clear();
        lastStoragePositionCheck.clear();
        lastValidPosition.clear();
    }

    void PrintingOrchestrator::loadExistingChanges(std::unique_ptr<utils::FilesystemInterface> filesystem, std::shared_ptr<utils::storage::AbstractStorage> const &storage)
    {
        if (nullptr == storage)
        {
            return;
        }

        try
        {
            if (utils::ProgramOptionsParser::getExistingWindowUUID().has_value() && filesystem->isFileDirectory(configuration->getStorageDirectory() + utils::ProgramOptionsParser::getExistingWindowUUID().value().getString()))
            {
                auto const existingWindowUUID = utils::ProgramOptionsParser::getExistingWindowUUID().value();
                auto const absoluteDirectoryPath = utils::helpers::Lambda::mapStringToWideString(configuration->getStorageDirectory() + existingWindowUUID.getString());
                auto const filesInDir = utils::helpers::Lambda::getAllFilesInDirectory(absoluteDirectoryPath);
                auto storageSize = storage->getSize();
                for (auto const &entry : filesInDir)
                {
                    if (utils::helpers::Lambda::mapStringToWideString(utils::datatypes::Strings::DELETIONS_FILE_NAME) == entry)
                    {
                        auto deletionsStorage = storageFactory->getStorage(utils::helpers::Conversion::squeezeu32String(absoluteDirectoryPath + U'/' + entry), existingWindowUUID);
                        if (deletionsStorage->getLastModificationTimestamp() < storage->getLastModificationTimestamp() && !configuration->isForceLoadChanges())
                        {
                            throw utils::exception::FeatherFileConcurrentlyModifiedException("Can't load changes for window :", utils::ProgramOptionsParser::getExistingWindowUUID().value().getString() + ".", "Looks like file was modified after. Use --force.");
                        }
                        try
                        {
                            auto deserializedDeletions = utils::Serializer::deserializeDeletion(deletionsStorage);
                            if (deserializedDeletions.watermark != FEATHER_WATERMARK)
                            {
                                continue;
                            }
                            for (auto const &e : deserializedDeletions.deletions)
                            {
                                if (e.first.first < storageSize && e.second.first <= storageSize)
                                {
                                    reverseDeletions[existingWindowUUID][e.second] = e.first;
                                    totalBytesOfDeletions[existingWindowUUID] += getDiffBytes(e.first, e.second, existingWindowUUID);
                                    deletions[existingWindowUUID].insert(e);
                                }
                            }
                        }
                        catch (const std::exception &e)
                        {
                            // continue
                        }
                    }
                    else if (utils::helpers::Lambda::mapStringToWideString(utils::datatypes::Strings::COLORS_FILE_NAME) == entry)
                    {
                        auto colorsStorage = storageFactory->getStorage(utils::helpers::Conversion::squeezeu32String(absoluteDirectoryPath + U'/' + entry), existingWindowUUID);
                        if (colorsStorage->getLastModificationTimestamp() < storage->getLastModificationTimestamp() && !configuration->isForceLoadChanges())
                        {
                            throw utils::exception::FeatherFileConcurrentlyModifiedException("Can't load changes for window :", utils::ProgramOptionsParser::getExistingWindowUUID().value().getString() + ".", "Looks like file was modified after. Use --force.");
                        }
                        try
                        {
                            auto deserializedColors = utils::Serializer::deserializeColors(colorsStorage);
                            if (deserializedColors.watermark != FEATHER_WATERMARK)
                            {
                                continue;
                            }
                            highlightedStrings[existingWindowUUID] = deserializedColors.textColors;
                            colorPriority[existingWindowUUID] = deserializedColors.colorPriority;
                            for (auto const &e : deserializedColors.textChunksColors)
                            {
                                if (e.first.first.first < storageSize && e.first.second.first <= storageSize)
                                {
                                    highlightedTextChunks[existingWindowUUID].insert(e);
                                }
                            }
                        }
                        catch (const std::exception &e)
                        {
                            // continue
                        }
                    }
                    else
                    {
                        auto changeStorage = storageFactory->getStorage(utils::helpers::Conversion::squeezeu32String(absoluteDirectoryPath + U'/' + entry), existingWindowUUID);
                        if (changeStorage->getLastModificationTimestamp() < storage->getLastModificationTimestamp() && !configuration->isForceLoadChanges())
                        {
                            throw utils::exception::FeatherFileConcurrentlyModifiedException("Can't load changes for window :", utils::ProgramOptionsParser::getExistingWindowUUID().value().getString() + ".", "Looks like file was modified after. Use --force.");
                        }
                        try
                        {
                            auto deserializedChanges = utils::Serializer::deserializeChange(changeStorage);
                            if (deserializedChanges.watermark != FEATHER_WATERMARK)
                            {
                                continue;
                            }

                            if (deserializedChanges.position <= storageSize)
                            {
                                size_t changesSize = deserializedChanges.changes.size();
                                changes[existingWindowUUID][deserializedChanges.position] = changesSize;
                                totalBytesOfNewChanges[existingWindowUUID] += changesSize;
                                primaryPrintingStorage->addToNewChanges(existingWindowUUID, pair(deserializedChanges.position, 0), deserializedChanges.changes, deserializedChanges.changes.size());
                            }
                        }
                        catch (std::exception const &e)
                        {
                            // continue
                        }
                    }
                }
            }
        }
        catch (utils::exception::FeatherFileConcurrentlyModifiedException const &e)
        {
            throw e;
        }
        catch (std::exception const &e)
        {
            throw utils::exception::FeatherInvalidArgumentException("Can't load changes for window :", utils::ProgramOptionsParser::getExistingWindowUUID().value().getString());
        }
    }

    std::vector<char> PrintingOrchestrator::getNewChanges(utils::datatypes::Uuid const &windowUUID, size_t pos, size_t offset, size_t size) const
    {
        try
        {
            return primaryPrintingStorage->getNewChanges(windowUUID, pos, offset, size);
        }
        catch (utils::exception::FeatherCacheMissException const &e)
        {
            return secondaryPrintingStorage->getNewChanges(windowUUID, pos, offset, size);
        }
    }

    std::unordered_multimap<std::u32string, std::pair<size_t, std::set<utils::datatypes::COLOR>>> const &PrintingOrchestrator::getHighlightedStrings(utils::datatypes::Uuid const &windowUUID) const
    {
        return highlightedStrings[windowUUID];
    }

    std::multimap<std::pair<pair, pair>, std::pair<size_t, std::set<utils::datatypes::COLOR>>> const &PrintingOrchestrator::getHighlitedTextChunks(utils::datatypes::Uuid const &windowUUID) const
    {
        return highlightedTextChunks[windowUUID];
    }

    std::optional<std::pair<pair, pair>> PrintingOrchestrator::getTextHighlight(utils::datatypes::Uuid const &windowUUID) const
    {
        return textHighlight.count(windowUUID) ? std::make_optional(textHighlight.at(windowUUID)) : std::nullopt;
    }

    std::set<utils::datatypes::COLOR> PrintingOrchestrator::getTextHighlightColor(utils::datatypes::Uuid const &windowUUID) const
    {
        return textHighlightColor[windowUUID];
    }

    bool PrintingOrchestrator::isCharAtFirstPosition(pair pos, utils::datatypes::Uuid const &windowUUID) const
    {
        return pos == getFirstValidPosition(windowUUID);
    }

    bool PrintingOrchestrator::isAllCharactersRemoved(utils::datatypes::Uuid const &windowUUID, size_t fileSize)
    {
        size_t totalNewChanges = totalBytesOfNewChanges.count(windowUUID) ? totalBytesOfNewChanges.find(windowUUID)->second : 0;
        size_t totalDeletions = totalBytesOfDeletions.count(windowUUID) ? totalBytesOfDeletions.find(windowUUID)->second : 0;
        return (fileSize + totalNewChanges) == totalDeletions;
    }

    bool PrintingOrchestrator::isAnyChangesMade(utils::datatypes::Uuid const &windowUUID)
    {
        size_t totalNewChanges = totalBytesOfNewChanges.count(windowUUID) ? totalBytesOfNewChanges.find(windowUUID)->second : 0;
        size_t totalDeletions = totalBytesOfDeletions.count(windowUUID) ? totalBytesOfDeletions.find(windowUUID)->second : 0;
        if (!totalNewChanges && !totalDeletions)
        {
            return false;
        }
        else if (totalNewChanges != totalDeletions)
        {
            return true;
        }
        std::map<pair, pair> changesLocal;
        std::transform(changes[windowUUID].cbegin(), changes[windowUUID].cend(), std::inserter(changesLocal, changesLocal.end()), [](auto p)
                       { return std::make_pair(pair(p.first, 0), pair(p.first, p.second)); });
        return !std::equal(deletions[windowUUID].begin(), deletions[windowUUID].end(), changesLocal.begin(), changesLocal.end());
    }

    bool PrintingOrchestrator::isCharAtPosDeleted(pair pos, utils::datatypes::Uuid const &windowUUID) const
    {
        if (!deletions.count(windowUUID) || deletions[windowUUID].empty())
        {
            return false;
        }
        else if (pos < deletions[windowUUID].begin()->first || pos >= deletions[windowUUID].rbegin()->second)
        {
            return false;
        }
        return std::find_if(deletions.find(windowUUID)->second.cbegin(), deletions.find(windowUUID)->second.cend(), [&](auto const &e)
                            { return (pos >= e.first && pos < e.second); }) != deletions.find(windowUUID)->second.cend();
    }

    bool PrintingOrchestrator::isCharAtPosWithChanges(size_t realPos, utils::datatypes::Uuid const &windowUUID) const
    {
        return changes.count(windowUUID) ? changes.find(windowUUID)->second.count(realPos) : false;
    }

    bool PrintingOrchestrator::isPreviousCharacterDeleted(pair pos, utils::datatypes::Uuid const &windowUUID) const
    {
        return reverseDeletions.count(windowUUID) && reverseDeletions[windowUUID].count(pos);
    }

    pair PrintingOrchestrator::getDeletionStart(utils::datatypes::Uuid const &windowUUID, pair pos)
    {
        auto middleOfDeletions = std::find_if(deletions.find(windowUUID)->second.cbegin(), deletions.find(windowUUID)->second.cend(), [&](auto const &e)
                                              { return (pos >= e.first && pos < e.second); });
        if (middleOfDeletions != deletions[windowUUID].cend())
        {
            return middleOfDeletions->first;
        }
        return reverseDeletions[windowUUID][pos];
    }

    pair PrintingOrchestrator::getDeletionEnd(utils::datatypes::Uuid const &windowUUID, pair pos)
    {
        auto middleOfDeletions = std::find_if(deletions.find(windowUUID)->second.cbegin(), deletions.find(windowUUID)->second.cend(), [&](auto const &e)
                                              { return (pos >= e.first && pos < e.second); });
        if (middleOfDeletions != deletions[windowUUID].cend())
        {
            return middleOfDeletions->second;
        }
        return deletions[windowUUID][pos];
    }

    pair PrintingOrchestrator::getLastValidPosition(utils::datatypes::Uuid const &windowUUID, std::shared_ptr<utils::storage::AbstractStorage> storage)
    {
        if (!lastStoragePositionCheck.count(windowUUID))
        {
            lastStoragePositionCheck[windowUUID] = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
        }
        if (!lastValidPosition.count(windowUUID))
        {
            lastValidPosition[windowUUID] = getPreviousVirtualPosition(windowUUID, storage, pair(storage->getSize(), getNumberOfChangesBytesAtPos(storage->getSize(), windowUUID)));
        }
        if (lastStoragePositionCheck[windowUUID] < lastStorageModificationTimestamp[windowUUID] || storage->getLastModificationTimestamp() > lastStoragePositionCheck[windowUUID])
        {
            lastValidPosition[windowUUID] = getPreviousVirtualPosition(windowUUID, storage, pair(storage->getSize(), getNumberOfChangesBytesAtPos(storage->getSize(), windowUUID)));
            lastStoragePositionCheck[windowUUID] = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
        }
        return lastValidPosition[windowUUID];
    }

    pair PrintingOrchestrator::getNextVirtualPosition(utils::datatypes::Uuid const &windowUUID, std::shared_ptr<utils::storage::AbstractStorage> storage, pair pos, bool ignoreDeletions)
    {
        size_t bytesToForward = utils::Utf8Util::getCharacterBytesTaken(getContinousCharacters(pos, MAX_UTF8_CHAR_BYTES, storage, windowUUID).first);
        return utils::helpers::Lambda::moveVirtualPositionForward(pos, bytesToForward, this, windowUUID, ignoreDeletions);
    }

    pair PrintingOrchestrator::getPreviousVirtualPosition(utils::datatypes::Uuid const &windowUUID, std::shared_ptr<utils::storage::AbstractStorage> storage, pair pos)
    {
        if (getFirstValidPosition(windowUUID) >= pos)
        {
            return getFirstValidPosition(windowUUID);
        }
        auto probe = getContinousCharactersBackward(pos, MAX_UTF8_CHAR_BYTES, storage, windowUUID);
        size_t bytesToRewind = (probe.first.size() - utils::Utf8Util::getPrevCharacterStartPos(probe.first, probe.first.size()));
        return utils::helpers::Lambda::moveVirtualPositionBackward(pos, bytesToRewind, this, windowUUID);
    }

    void PrintingOrchestrator::addChanges(utils::datatypes::Uuid const &windowUUID, std::shared_ptr<utils::storage::AbstractStorage> storage, pair pos, std::vector<char> const &buffer, size_t bufferSize, bool appendChunk, std::optional<pair> otherUndoPos, std::optional<pair> otherRedoPos)
    {
        lastStorageModificationTimestamp[windowUUID] = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
        size_t currNumberOfChangesBytes = getNumberOfChangesBytesAtPos(pos.first, windowUUID);
        try
        {
            primaryPrintingStorage->addToNewChanges(windowUUID, pos, buffer, bufferSize);
        }
        // Fallback to file storage
        catch (utils::exception::FeatherMemoryException const &e)
        {
            std::vector<char> existingChanges;
            try
            {
                existingChanges = primaryPrintingStorage->getNewChanges(windowUUID, pos.first, 0, currNumberOfChangesBytes);
            }
            catch (utils::exception::FeatherCacheMissException const &e)
            {
                // No changes were in cache, just the chunk we try to put is too large
            }
            secondaryPrintingStorage->addToNewChanges(windowUUID, pair(pos.first, 0), std::move(existingChanges), currNumberOfChangesBytes);
            secondaryPrintingStorage->addToNewChanges(windowUUID, pos, buffer, bufferSize);
            primaryPrintingStorage->removeChanges(windowUUID, pos.first);
        }
        catch (utils::exception::FeatherCacheMissException const &e)
        {
            secondaryPrintingStorage->addToNewChanges(windowUUID, pos, buffer, bufferSize);
        }
        totalBytesOfNewChanges[windowUUID] += bufferSize;
        changes[windowUUID][pos.first] += bufferSize;
        if (deletions.count(windowUUID))
        {
            deletions[windowUUID] = utils::helpers::Lambda::shiftIntervals(deletions[windowUUID], pos, bufferSize, std::nullopt);
            reverseDeletions[windowUUID].clear();
            totalBytesOfDeletions[windowUUID] = 0;
            for (auto &deletion : deletions[windowUUID])
            {
                reverseDeletions[windowUUID][deletion.second] = deletion.first;
                totalBytesOfDeletions[windowUUID] += getDiffBytes(deletion.first, deletion.second, windowUUID);
            }
        }
        updateHighlightedTextChunks(pos, bufferSize, windowUUID);
        updateTextCopyPositionsToSkip(pos, bufferSize, windowUUID);

        if (appendChunk)
        {
            changesStack->appendInsert(bufferSize, windowUUID);
        }
        else
        {
            changesStack->validateChanges(pos, bufferSize, windowUUID);
            if (changesStack->isUndoStackEmpty(windowUUID))
            {
                // We don't want to undo to position that don't exist
                auto undoPos = getPreviousVirtualPosition(windowUUID, storage, pos);
                changesStack->addChange(utils::datatypes::ChangeType::INSERT, windowUUID, otherUndoPos.has_value() ? otherUndoPos.value() : undoPos, otherRedoPos.has_value() ? otherRedoPos.value() : pair(pos.first, pos.second + bufferSize), std::map<pair, pair>{{pos, pair{pos.first, pos.second + bufferSize}}});
            }
            else
            {
                changesStack->addChange(utils::datatypes::ChangeType::INSERT, windowUUID, otherUndoPos.has_value() ? otherUndoPos.value() : pos, otherRedoPos.has_value() ? otherRedoPos.value() : pair(pos.first, pos.second + bufferSize), std::map<pair, pair>{{pos, pair{pos.first, pos.second + bufferSize}}});
            }
        }
        changesStack->clearRedoChanges(windowUUID);
    }

    void PrintingOrchestrator::applyDeletions(std::map<pair, pair> newDeletions, utils::datatypes::Uuid const &windowUUID)
    {
        lastStorageModificationTimestamp[windowUUID] = lastStoragePositionCheck[windowUUID] + std::chrono::nanoseconds(1);
        if (newDeletions.empty())
        {
            deletions.erase(windowUUID);
            reverseDeletions.erase(windowUUID);
            totalBytesOfDeletions.erase(windowUUID);
            return;
        }
        deletions[windowUUID] = newDeletions;
        reverseDeletions[windowUUID].clear();
        totalBytesOfDeletions[windowUUID] = 0;
        for (auto &deletion : deletions[windowUUID])
        {
            reverseDeletions[windowUUID][deletion.second] = deletion.first;
            totalBytesOfDeletions[windowUUID] += getDiffBytes(deletion.first, deletion.second, windowUUID);
        }
    }

    size_t PrintingOrchestrator::dumpCache(utils::datatypes::Uuid const &windowUUID)
    {
        std::vector<char> existingChanges;
        size_t dumpedChanges = 0, dumpedDeletions = totalBytesOfDeletions.count(windowUUID) ? totalBytesOfDeletions[windowUUID] : 0;
        if (changes.count(windowUUID))
        {
            for (auto &c : changes[windowUUID])
            {
                try
                {
                    existingChanges = primaryPrintingStorage->getNewChanges(windowUUID, c.first, 0, c.second);
                }
                catch (utils::exception::FeatherCacheMissException const &e)
                {
                    continue;
                }
                dumpedChanges += c.second;
                secondaryPrintingStorage->dumpChanges(windowUUID, pair(c.first, 0), std::move(existingChanges));
            }
        }

        if (deletions.count(windowUUID))
        {
            if (secondaryPrintingStorage)
            {
                secondaryPrintingStorage->dumpDeletions(windowUUID, deletions[windowUUID]);
            }
            else
            {
                primaryPrintingStorage->dumpDeletions(windowUUID, deletions[windowUUID]);
            }
        }

        if (!highlightedTextChunks[windowUUID].empty() || !highlightedStrings[windowUUID].empty())
        {
            dumpedChanges += 1;
            if (secondaryPrintingStorage)
            {
                secondaryPrintingStorage->dumpColors(windowUUID, highlightedTextChunks[windowUUID], highlightedStrings[windowUUID], colorPriority[windowUUID]);
            }
            else
            {
                primaryPrintingStorage->dumpColors(windowUUID, highlightedTextChunks[windowUUID], highlightedStrings[windowUUID], colorPriority[windowUUID]);
            }
        }
        return dumpedChanges + dumpedDeletions;
    }

    void PrintingOrchestrator::highlightSelection(std::pair<std::pair<pair, pair>, std::pair<size_t, std::set<utils::datatypes::COLOR>>> selectedText, utils::datatypes::Uuid const &windowUUID)
    {
        highlightedTextChunks[windowUUID].insert(selectedText);
    }

    std::set<utils::datatypes::COLOR> PrintingOrchestrator::highlightSelection(std::pair<pair, pair> const &highlightedPositions, utils::datatypes::Uuid const &windowUUID, std::set<utils::datatypes::COLOR> color)
    {
        if (!textHighlightColor.count(windowUUID))
        {
            if (utils::NCursesWrapper::hasTerminalLimitedColorsCapabilities())
            {
                int colorIdx = std::rand() % BACKGROUND_COLORS.size();
                textHighlightColor[windowUUID].insert(BACKGROUND_COLORS[colorIdx]);
            }
            else if (!color.empty())
            {
                textHighlightColor[windowUUID] = color;
            }
            else
            {
                for (int i = 0; i < 3; ++i)
                {
                    int colorIdx = std::rand() % BACKGROUND_COLORS.size();
                    auto shuffledColor = BACKGROUND_COLORS[colorIdx];
                    while (textHighlightColor[windowUUID].count(shuffledColor))
                    {
                        colorIdx = std::rand() % BACKGROUND_COLORS.size();
                        shuffledColor = BACKGROUND_COLORS[colorIdx];
                    }
                    textHighlightColor[windowUUID].insert(shuffledColor);
                }
            }
        }
        textHighlight[windowUUID] = highlightedPositions;
        textHighlightColor[windowUUID].insert(utils::datatypes::COLOR::FEATHER_COLOR_WHITE);
        return textHighlightColor[windowUUID];
    }

    void PrintingOrchestrator::highlightText(std::u32string text, std::set<utils::datatypes::COLOR> color, utils::datatypes::Uuid const &windowUUID)
    {
        if (this->highlightedStrings[windowUUID].count(text))
        {
            auto range = this->highlightedStrings[windowUUID].equal_range(text);
            for (auto it = range.first; it != range.second; ++it)
            {
                if (color == it->second.second)
                {
                    return;
                }
            }
        }
        this->highlightedStrings[windowUUID].insert({text, {colorPriority[windowUUID]++, color}});
    }

    std::pair<std::pair<pair, pair>, std::pair<size_t, std::set<utils::datatypes::COLOR>>> PrintingOrchestrator::highlightTextChunk(std::pair<pair, pair> const &highlightTextChunk, utils::datatypes::Uuid const &windowUUID)
    {
        auto toHighlight = std::make_pair(highlightTextChunk, std::make_pair(highlightedTextChunkPriority[windowUUID]++, textHighlightColor[windowUUID]));
        this->highlightedTextChunks[windowUUID].insert(toHighlight);
        return toHighlight;
    }

    void PrintingOrchestrator::unhighlightText(std::u32string text, utils::datatypes::Uuid const &windowUUID)
    {
        this->highlightedStrings[windowUUID].erase(text);
    }

    void PrintingOrchestrator::normalizeAfterSave(utils::datatypes::Uuid const &windowUUID)
    {
        auto updateCopiedPosToSkip = [&]() -> void
        {
			std::map<pair, pair> posToSkip;
			for (auto const &e : textToCopyPositionsToSkip->second)
			{
				posToSkip.insert(std::make_pair(pair(convertVirtualPositionToByteOffset(e.first, windowUUID), 0), pair(convertVirtualPositionToByteOffset(e.second, windowUUID), 0)));
			}
			textToCopyPositionsToSkip->second = posToSkip;	
        };

        if (highlightedTextChunks.count(windowUUID))
        {
            std::multimap<std::pair<pair, pair>, std::pair<size_t, std::set<utils::datatypes::COLOR>>> updatedHighlightedTextChunks;
            for (auto const &e : highlightedTextChunks[windowUUID])
            {
                auto rangeIncludingDeletions = getRangeIncludingDeletions({e.first.first, e.first.second}, windowUUID);
                if (rangeIncludingDeletions.first != rangeIncludingDeletions.second)
                {
                    updatedHighlightedTextChunks.insert({std::make_pair(pair(convertVirtualPositionToByteOffset(rangeIncludingDeletions.first, windowUUID), 0), pair(convertVirtualPositionToByteOffset(rangeIncludingDeletions.second, windowUUID), 0)), e.second});
                }
            }
            std::swap(highlightedTextChunks[windowUUID], updatedHighlightedTextChunks);
            if (textHighlight.count(windowUUID))
            {
                auto rangeIncludingDeletions = getRangeIncludingDeletions({textHighlight[windowUUID].first, textHighlight[windowUUID].second}, windowUUID);
                if (rangeIncludingDeletions.first != rangeIncludingDeletions.second)
                {
                    textHighlight[windowUUID] = std::make_pair(pair(convertVirtualPositionToByteOffset(rangeIncludingDeletions.first, windowUUID), 0), pair(convertVirtualPositionToByteOffset(rangeIncludingDeletions.second, windowUUID), 0));
                }
            }
        }
    
		if (isTextCopied(windowUUID))
		{
			if (!utils::helpers::Lambda::intervalsOverlap(textToCopy->second, deletions[windowUUID]))
			{
				textToCopy->second = std::make_pair(pair(convertVirtualPositionToByteOffset(textToCopy->second.first, windowUUID), 0), pair(convertVirtualPositionToByteOffset(textToCopy->second.second, windowUUID), 0));
				updateCopiedPosToSkip();
			}
			else
			{
				textToCopy = std::nullopt;
				textToCopyPositionsToSkip = std::nullopt;
			}
		}
    	else if (isLineCopied(windowUUID))
    	{
			if (!utils::helpers::Lambda::intervalsOverlap(copiedLine->second, deletions[windowUUID]))
			{
				copiedLine->second = std::make_pair(pair(convertVirtualPositionToByteOffset(copiedLine->second.first, windowUUID), 0), pair(convertVirtualPositionToByteOffset(copiedLine->second.second, windowUUID), 0));
				updateCopiedPosToSkip();
			}
			else
			{
				copiedLine = std::nullopt;
				textToCopyPositionsToSkip = std::nullopt;
			}			
		}
        else if(isTextCut(windowUUID))
        {
    	    textToCut = std::nullopt;
			textToCopyPositionsToSkip = std::nullopt;
    	    textToCutSnapshot = std::nullopt;
        }
    }

    void PrintingOrchestrator::undelete(pair start, pair end, utils::datatypes::Uuid const &windowUUID)
    {
        lastStorageModificationTimestamp[windowUUID] = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
        auto newDeletions = utils::helpers::Lambda::disjoin(deletions[windowUUID], start, end);
        applyDeletions(newDeletions, windowUUID);
    }

    void PrintingOrchestrator::unhighlightSelection(std::pair<std::pair<pair, pair>, std::pair<size_t, std::set<utils::datatypes::COLOR>>> selectedText, utils::datatypes::Uuid const &windowUUID)
    {
        if (!highlightedTextChunks.count(windowUUID))
        {
            return;
        }
        std::multimap<std::pair<pair, pair>, std::pair<size_t, std::set<utils::datatypes::COLOR>>> modifiedHighlightedTextChunks;
        for (auto &e : this->highlightedTextChunks[windowUUID])
        {
            if (e.first == selectedText.first && e.second.first == selectedText.second.first)
            {
                continue;
            }
            modifiedHighlightedTextChunks.insert(e);
        }
        this->highlightedTextChunks[windowUUID].swap(modifiedHighlightedTextChunks);
    }

    void PrintingOrchestrator::unhighlightSelection(utils::datatypes::Uuid const &windowUUID)
    {
        this->textHighlight.erase(windowUUID);
        this->textHighlightColor.erase(windowUUID);
    }

    void PrintingOrchestrator::unhighlightAllSelection(utils::datatypes::Uuid const &windowUUID)
    {
        this->textHighlight.erase(windowUUID);
        this->textHighlightColor.erase(windowUUID);
        this->highlightedTextChunks.erase(windowUUID);
    }

    void PrintingOrchestrator::updateHighlightedTextChunks(pair pos, size_t size, utils::datatypes::Uuid const &windowUUID)
    {
        if (!highlightedTextChunks.count(windowUUID))
        {
            return;
        }
        std::multimap<std::pair<pair, pair>, std::pair<size_t, std::set<utils::datatypes::COLOR>>> updatedHighlightedTextChunks;
        for (auto const &e : highlightedTextChunks[windowUUID])
        {
            updatedHighlightedTextChunks.insert({utils::helpers::Lambda::shiftInterval(e.first, pos, size), e.second});
        }
        highlightedTextChunks[windowUUID].swap(updatedHighlightedTextChunks);
    }

    pair PrintingOrchestrator::removeCharacters(utils::datatypes::Uuid const &windowUUID, std::shared_ptr<utils::storage::AbstractStorage> storage, pair startVirtualPos, pair endVirtualPos, bool putOnChangesStack, std::optional<pair> otherUndoPos, std::optional<pair> otherRedoPos)
    {
        lastStorageModificationTimestamp[windowUUID] = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
        if (startVirtualPos == endVirtualPos)
        {
            return endVirtualPos;
        }
        auto previousDeletions = deletions[windowUUID];
        if (putOnChangesStack)
        {
            changesStack->clearRedoChanges(windowUUID);
            std::map<pair, pair> deletionsInInterval;
            std::copy_if(
                begin(deletions[windowUUID]),
                end(deletions[windowUUID]),
                std::inserter(deletionsInInterval, begin(deletionsInInterval)),
                [&](std::pair<pair, pair> const &p)
                { return p.first >= startVirtualPos && p.second <= endVirtualPos; });
            auto deletionsToStash = utils::helpers::Lambda::disjoinDeletions(deletionsInInterval, startVirtualPos, endVirtualPos);
            changesStack->addChange(utils::datatypes::ChangeType::DELETE, windowUUID, otherUndoPos.has_value() ? otherUndoPos.value() : startVirtualPos, otherRedoPos.has_value() ? otherRedoPos.value() : endVirtualPos, deletionsToStash);
        }
        if (isCharAtPosDeleted(endVirtualPos, windowUUID))
        {
            endVirtualPos = getDeletionEnd(windowUUID, endVirtualPos);
        }
        deletions[windowUUID] = utils::helpers::Lambda::mergeIntervals(deletions[windowUUID], std::map<pair, pair>{{startVirtualPos, endVirtualPos}});
        reverseDeletions[windowUUID].clear();
        totalBytesOfDeletions[windowUUID] = 0;

        for (auto &deletion : deletions[windowUUID])
        {
            reverseDeletions[windowUUID][deletion.second] = deletion.first;
            totalBytesOfDeletions[windowUUID] += getDiffBytes(deletion.first, deletion.second, windowUUID);
        }
        pair lastValidPosition = getLastValidPosition(windowUUID, storage);
        pair newEnd = lastValidPosition;
        if (lastValidPosition < endVirtualPos && utils::FEATHER_MODE::WRITE_MODE != currentFeatherMode)
        {
            newEnd = lastValidPosition;
        }
        else
        {
            newEnd = endVirtualPos;
        }

        return newEnd;
    }

    void PrintingOrchestrator::setSearchedText(utils::datatypes::Uuid const &windowUUID, std::optional<std::pair<pair, size_t>> searchedText)
    {
        this->searchedText[windowUUID] = searchedText;
    }

    std::optional<std::pair<pair, size_t>> PrintingOrchestrator::getSearchedText(utils::datatypes::Uuid const &windowUUID) const
    {
        return this->searchedText.count(windowUUID) ? this->searchedText.at(windowUUID) : std::nullopt;
    }

    void PrintingOrchestrator::removeCharacters(utils::datatypes::Uuid const &windowUUID, std::shared_ptr<utils::storage::AbstractStorage> storage, std::map<pair, pair> toRemove, bool putOnChangesStack)
    {
        if (toRemove.empty())
        {
            return;
        }
        lastStorageModificationTimestamp[windowUUID] = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
        if (putOnChangesStack)
        {
            changesStack->clearRedoChanges(windowUUID);
            std::map<pair, pair> deletionsInInterval;
            std::copy_if(
                begin(deletions[windowUUID]),
                end(deletions[windowUUID]),
                std::inserter(deletionsInInterval, begin(deletionsInInterval)),
                [&](std::pair<pair, pair> const &p)
                {
                    for (auto const &d : toRemove)
                    {
                        if (p.first >= d.first && p.second <= d.second)
                            return true;
                    }
                    return false;
                });
            std::map<pair, pair> deletionsToStash;
            for (auto const &d : toRemove)
            {
                deletionsToStash.merge(utils::helpers::Lambda::disjoinDeletions(deletionsInInterval, d.first, d.second));
            }
            changesStack->addChange(utils::datatypes::ChangeType::DELETE, windowUUID, toRemove.cbegin()->first, std::prev(toRemove.cend())->second, deletionsToStash);
        }
        deletions[windowUUID] = utils::helpers::Lambda::mergeIntervals(deletions[windowUUID], toRemove);
        reverseDeletions[windowUUID].clear();
        totalBytesOfDeletions[windowUUID] = 0;

        for (auto &deletion : deletions[windowUUID])
        {
            reverseDeletions[windowUUID][deletion.second] = deletion.first;
            totalBytesOfDeletions[windowUUID] += getDiffBytes(deletion.first, deletion.second, windowUUID);
        }
    }

    void PrintingOrchestrator::removeChangesRelatedToWindow(utils::datatypes::Uuid const &windowUUID)
    {
        lastStorageModificationTimestamp[windowUUID] = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
        if (changes.count(windowUUID))
        {
            for (auto const &c : changes[windowUUID])
            {
                try
                {
                    primaryPrintingStorage->removeChanges(windowUUID, c.first);
                }
                catch (utils::exception::FeatherCacheMissException const &e)
                {
                    secondaryPrintingStorage->removeChanges(windowUUID, c.first);
                }
            }
        }
        changes.erase(windowUUID);
        deletions.erase(windowUUID);
        reverseDeletions.erase(windowUUID);
        totalBytesOfNewChanges[windowUUID] = 0;
        totalBytesOfDeletions[windowUUID] = 0;
        changesStack->clearRedoChanges(windowUUID);
        changesStack->clearUndoChanges(windowUUID);
    }

    std::pair<std::basic_string<char>, pair> PrintingOrchestrator::getContinousCharactersAsString(pair currPos, size_t size, std::shared_ptr<utils::storage::AbstractStorage> storage, utils::datatypes::Uuid const &windowUUID, bool alignChunk, std::map<pair, pair> const &toSkip)
    {
        std::basic_string<char> buffer;
        pair const firstInvalidPos = getFirstInvalidPosition(windowUUID, storage);
        if (currPos >= firstInvalidPos)
        {
            return std::make_pair(buffer, currPos);
        }
        size_t initialCursorPosition = storage->getCurrentPointerPosition();
        storage->setCursor(currPos.first);
        while (size)
        {
            if ((currPos >= firstInvalidPos) || ((storage->getSize() + getTotalBytesOfNewChanges(windowUUID) == getTotalBytesOfDeletions(windowUUID))))
            {
                storage->setCursor(initialCursorPosition);
                return std::make_pair(buffer, currPos);
            }
            else if (deletions.count(windowUUID) && deletions[windowUUID].count(currPos))
            {
                currPos = deletions[windowUUID][currPos];
                storage->setCursor(currPos.first);
                continue;
            }
            else if (toSkip.find(currPos) != std::end(toSkip))
            {
                currPos = toSkip.find(currPos)->second;
                storage->setCursor(currPos.first);
                continue;
            }
            auto nextSkip = toSkip.lower_bound(currPos);
            size_t diffToNextDeletionOrSkip = (nextSkip == std::end(toSkip)) ? getDiffToNextDeletion(currPos, storage, windowUUID) : std::min(getDiffToNextDeletion(currPos, storage, windowUUID), getDiffBytes(currPos, nextSkip->first, windowUUID));
            // Still in new changes
            if (currPos.second < getNumberOfChangesBytesAtPos(currPos.first, windowUUID))
            {
                size_t canCopy = std::min(size, !diffToNextDeletionOrSkip ? (getNumberOfChangesBytesAtPos(currPos.first, windowUUID) - currPos.second) : std::min(diffToNextDeletionOrSkip, getNumberOfChangesBytesAtPos(currPos.first, windowUUID) - currPos.second));
                auto localBuffer = getNewChanges(windowUUID, currPos.first, currPos.second, canCopy);
                std::copy(localBuffer.begin(), localBuffer.end(), std::back_inserter(buffer));
                currPos.second += canCopy;
                size -= canCopy;
                continue;
            }
            size_t diffToNextChange = getDiffToNextChange(currPos, storage, windowUUID);
            size_t diffToFileEnd = getDiffBytesWithoutDeletions(currPos, firstInvalidPos, windowUUID);
            size_t canCopy_1 = std::min(size, diffToFileEnd);
            size_t canCopy_2 = (!diffToNextChange && diffToNextDeletionOrSkip) ? diffToNextDeletionOrSkip : (diffToNextChange && !diffToNextDeletionOrSkip) ? diffToNextChange
                                                                                                                                                            : std::min(diffToNextChange, diffToNextDeletionOrSkip);
            size_t canCopy = !canCopy_2 ? canCopy_1 : std::min(canCopy_1, canCopy_2);
            std::vector<char> localBuffer(canCopy, 0);
            if (!canCopy)
            {
                storage->setCursor(initialCursorPosition);
                return std::make_pair(buffer, currPos);
            }
            storage->readChunk(localBuffer, canCopy, alignChunk);
            std::copy(localBuffer.begin(), localBuffer.end(), std::back_inserter(buffer));
            if (!localBuffer.empty())
            {
                currPos.first += localBuffer.size();
                currPos.second = 0;
            }
            // We wanted to read more but we hitted middle of character
            if (canCopy == size && localBuffer.size() > canCopy)
            {
                storage->setCursor(initialCursorPosition);
                return std::make_pair(buffer, currPos);
            }
            size -= localBuffer.size();
        }
        storage->setCursor(initialCursorPosition);
        return std::make_pair(buffer, currPos);
    }

    std::pair<std::vector<char>, pair> PrintingOrchestrator::getContinousCharacters(pair currPos, size_t size, std::shared_ptr<utils::storage::AbstractStorage> storage, utils::datatypes::Uuid const &windowUUID, bool alignChunk, std::map<pair, pair> const &toSkip)
    {
        std::vector<char> buffer;
        pair const firstInvalidPos = getFirstInvalidPosition(windowUUID, storage);
        if (currPos >= firstInvalidPos)
        {
            return std::make_pair(buffer, currPos);
        }
        size_t initialCursorPosition = storage->getCurrentPointerPosition();
        storage->setCursor(currPos.first);
        while (size)
        {
            if ((currPos >= firstInvalidPos) || ((storage->getSize() + getTotalBytesOfNewChanges(windowUUID) == getTotalBytesOfDeletions(windowUUID))))
            {
                storage->setCursor(initialCursorPosition);
                return std::make_pair(buffer, currPos);
            }
            else if (deletions.count(windowUUID) && deletions[windowUUID].count(currPos))
            {
                currPos = deletions[windowUUID][currPos];
                storage->setCursor(currPos.first);
                continue;
            }
            else if (toSkip.find(currPos) != std::end(toSkip))
            {
                currPos = toSkip.find(currPos)->second;
                storage->setCursor(currPos.first);
                continue;
            }
            auto nextSkip = toSkip.lower_bound(currPos);
            size_t diffToNextDeletionOrSkip = (nextSkip == std::end(toSkip)) ? getDiffToNextDeletion(currPos, storage, windowUUID) : std::min(getDiffToNextDeletion(currPos, storage, windowUUID), getDiffBytes(currPos, nextSkip->first, windowUUID));
            // Still in new changes
            if (currPos.second < getNumberOfChangesBytesAtPos(currPos.first, windowUUID))
            {
                size_t canCopy = std::min(size, !diffToNextDeletionOrSkip ? (getNumberOfChangesBytesAtPos(currPos.first, windowUUID) - currPos.second) : std::min(diffToNextDeletionOrSkip, getNumberOfChangesBytesAtPos(currPos.first, windowUUID) - currPos.second));
                auto localBuffer = getNewChanges(windowUUID, currPos.first, currPos.second, canCopy);
                std::copy(localBuffer.begin(), localBuffer.end(), std::back_inserter(buffer));
                currPos.second += canCopy;
                size -= canCopy;
                continue;
            }
            size_t diffToNextChange = getDiffToNextChange(currPos, storage, windowUUID);
            size_t diffToFileEnd = getDiffBytesWithoutDeletions(currPos, firstInvalidPos, windowUUID);
            size_t canCopy_1 = std::min(size, diffToFileEnd);
            size_t canCopy_2 = (!diffToNextChange && diffToNextDeletionOrSkip) ? diffToNextDeletionOrSkip : (diffToNextChange && !diffToNextDeletionOrSkip) ? diffToNextChange
                                                                                                                                                            : std::min(diffToNextChange, diffToNextDeletionOrSkip);
            size_t canCopy = !canCopy_2 ? canCopy_1 : std::min(canCopy_1, canCopy_2);
            std::vector<char> localBuffer(canCopy, 0);
            if (!canCopy)
            {
                storage->setCursor(initialCursorPosition);
                return std::make_pair(buffer, currPos);
            }
            storage->readChunk(localBuffer, canCopy, alignChunk);
            std::copy(localBuffer.begin(), localBuffer.end(), std::back_inserter(buffer));
            if (!localBuffer.empty())
            {
                currPos.first += localBuffer.size();
                currPos.second = 0;
            }
            // We wanted to read more but we hitted middle of character
            if (canCopy == size && localBuffer.size() > canCopy)
            {
                storage->setCursor(initialCursorPosition);
                return std::make_pair(buffer, currPos);
            }
            size -= localBuffer.size();
        }
        storage->setCursor(initialCursorPosition);
        return std::make_pair(buffer, currPos);
    }

    std::pair<std::vector<char>, pair> PrintingOrchestrator::getContinousCharactersBackward(pair currPos, size_t size, std::shared_ptr<utils::storage::AbstractStorage> storage, utils::datatypes::Uuid const &windowUUID)
    {
        std::vector<char> buffer(size, 0);
        size_t currBufferEnd = buffer.size();
        pair firstValidPosition = getFirstValidPosition(windowUUID);
        size_t initialCursorPosition = storage->getCurrentPointerPosition();
        storage->setCursor(currPos.first);
        while (size)
        {
            if (currPos == firstValidPosition)
            {
                buffer.erase(buffer.begin(), std::next(buffer.begin(), currBufferEnd));
                storage->setCursor(initialCursorPosition);
                return std::make_pair(buffer, firstValidPosition);
            }
            else if (reverseDeletions.count(windowUUID) && reverseDeletions[windowUUID].count(currPos))
            {
                currPos = reverseDeletions[windowUUID][currPos];
            }
            auto prevReverseDeletions = getDiffToPrevReverseDeletion(currPos, storage, windowUUID);
            // Still in new changes
            if (currPos.second <= getNumberOfChangesBytesAtPos(currPos.first, windowUUID) && currPos.second > 0)
            {
                size_t canCopy = std::min(size, std::min(prevReverseDeletions.first, currPos.second));
                auto localBuffer = getNewChanges(windowUUID, currPos.first, currPos.second - canCopy, canCopy);
                // Intentional cutoff
                if (localBuffer.size() > canCopy)
                {
                    localBuffer.erase(localBuffer.begin(), std::next(localBuffer.begin(), localBuffer.size() - canCopy));
                }
                // localBuffer = utils::helpers::Lambda::alignBufferBegining(std::move(localBuffer));
                currBufferEnd -= localBuffer.size();
                std::copy(localBuffer.begin(), localBuffer.end(), std::next(buffer.begin(), currBufferEnd));
                currPos.second -= localBuffer.size();
                if (canCopy == size && localBuffer.size() < canCopy)
                {
                    buffer.erase(buffer.begin(), std::next(buffer.begin(), currBufferEnd));
                    storage->setCursor(initialCursorPosition);
                    return std::make_pair(buffer, isCharAtPosDeleted(currPos, windowUUID) ? deletions[windowUUID][currPos] : currPos);
                }
                size -= localBuffer.size();
                continue;
            }
            auto diffToPrevChange = getDiffToPrevChanges(currPos, storage, windowUUID);
            size_t canCopy = std::min(size, std::min(diffToPrevChange.first, prevReverseDeletions.first));
            std::vector<char> localBuffer(canCopy, 0);
            storage->setCursor(currPos.first - canCopy);
            storage->readChunk(localBuffer, canCopy, false);
            currBufferEnd -= localBuffer.size();
            std::copy(localBuffer.begin(), localBuffer.end(), std::next(buffer.begin(), currBufferEnd));
            currPos.first -= localBuffer.size();
            currPos.second = localBuffer.empty() ? currPos.second : getNumberOfChangesBytesAtPos(currPos.first, windowUUID);
            // We wanted to read more but we hitted middle of character
            if (canCopy == size && localBuffer.size() < canCopy)
            {
                buffer.erase(buffer.begin(), std::next(buffer.begin(), currBufferEnd));
                storage->setCursor(initialCursorPosition);
                return std::make_pair(buffer, isCharAtPosDeleted(currPos, windowUUID) ? deletions[windowUUID][currPos] : currPos);
            }
            size -= std::min(size, localBuffer.size());
        }
        storage->setCursor(initialCursorPosition);
        return std::make_pair(buffer, currPos);
    }

    size_t PrintingOrchestrator::convertVirtualPositionToByteOffset(pair pos, utils::datatypes::Uuid const &windowUUID) const
    {
        size_t bytesOfChanges = 0;
        if (changes.count(windowUUID))
        {
            bytesOfChanges = std::accumulate(changes.find(windowUUID)->second.cbegin(), changes.find(windowUUID)->second.cend(), 0, [&](size_t sum, auto &el)
                                             {
                                                 if (el.first < pos.first)
                                                 {
                                                     sum += el.second;
                                                 }
                                                 return sum; });
        }

        size_t bytesOfDeletions = 0;
        if (deletions.count(windowUUID))
        {
            bytesOfDeletions = std::accumulate(deletions.find(windowUUID)->second.cbegin(), deletions.find(windowUUID)->second.cend(), 0, [&](size_t sum, auto &el)
                                               {
                                                   if (el.second <= pos)
                                                   {
                                                       sum += getDiffBytes(el.first, el.second, windowUUID);
                                                   }
                                                   return sum; });
        }
        return pos.first + pos.second + bytesOfChanges - bytesOfDeletions;
    }

    unsigned short PrintingOrchestrator::getCharacterBytesTaken(pair position, std::shared_ptr<utils::storage::AbstractStorage> storage, utils::datatypes::Uuid const &windowUUID) const
    {
        size_t numOfChangesBytes = getNumberOfChangesBytesAtPos(position.first, windowUUID);
        if (position.second == numOfChangesBytes)
        {
            return utils::Utf8Util::getCharacterBytesTakenLight(storage->getCharacterAtPos(position.first));
        }
        return utils::Utf8Util::getCharacterBytesTaken(getNewChanges(windowUUID, position.first, position.second, std::min(numOfChangesBytes - position.second, MAX_UTF8_CHAR_BYTES)));
    }

    std::optional<std::chrono::nanoseconds> PrintingOrchestrator::getLastStorageModificationTimestamp(utils::datatypes::Uuid const &windowUUID) const
    {
        return !lastStorageModificationTimestamp.count(windowUUID) ? std::nullopt : std::optional<std::chrono::nanoseconds>(lastStorageModificationTimestamp[windowUUID]);
    }

    size_t PrintingOrchestrator::getDiffBytes(pair left, pair right, utils::datatypes::Uuid const &windowUUID) const
    {
        if (left >= right)
        {
            return 0;
        }
        else if (left.first == right.first)
        {
            return right.second - left.second;
        }
        return getNumberOfChangesBytesAtPos(left.first, windowUUID) - left.second + right.second + (right.first - left.first) + getNumberOfChangesBytesBetweenPos(left.first, right.first, windowUUID); // - getNumberOfDeletedBytesBetweenPos(left.first, right.first, windowUUID);
    }

    size_t PrintingOrchestrator::getDiffBytesWithoutDeletions(pair left, pair right, utils::datatypes::Uuid const &windowUUID, std::map<pair, pair> const &toSkip) const
    {
        size_t diffBytes = getDiffBytes(left, right, windowUUID);
        if (deletions.count(windowUUID))
        {
            for (auto const &d : deletions[windowUUID])
            {
                if (d.first >= left && d.second <= right)
                {
                    diffBytes -= getDiffBytes(d.first, d.second, windowUUID);
                }
            }
        }
        for (auto const &s : toSkip)
        {
            if (s.first >= left && s.second <= right)
            {
                diffBytes -= getDiffBytes(s.first, s.second, windowUUID);
            }
        }
        return diffBytes;
    }

    size_t PrintingOrchestrator::getDiffToNextChange(pair pos, std::shared_ptr<utils::storage::AbstractStorage> storage, utils::datatypes::Uuid const &windowUUID)
    {
        // No more change and deletions
        pair lastValidPosition = getLastValidPosition(windowUUID, storage);
        if (pos >= lastValidPosition)
        {
            return 0;
        }
        if (!changes.count(windowUUID) || changes[windowUUID].empty())
        {
            return getDiffBytes(pos, lastValidPosition, windowUUID) + getCharacterBytesTaken(lastValidPosition, storage, windowUUID);
        }
        auto const greaterChangeIt = std::find_if(changes[windowUUID].begin(), changes[windowUUID].end(), [&](auto &el)
                                                  { return el.first > pos.first; });
        return greaterChangeIt != std::cend(changes[windowUUID]) ? getDiffBytes(pos, pair(greaterChangeIt->first, 0), windowUUID) : getDiffBytes(pos, lastValidPosition, windowUUID) + getCharacterBytesTaken(lastValidPosition, storage, windowUUID);
    }

    size_t PrintingOrchestrator::getDiffToNextDeletion(pair pos, std::shared_ptr<utils::storage::AbstractStorage> storage, utils::datatypes::Uuid const &windowUUID)
    {
        // No more change and deletions
        pair lastValidPosition = getLastValidPosition(windowUUID, storage);
        if (pos >= getFirstInvalidPosition(windowUUID, storage))
        {
            return 0;
        }
        if (!deletions.count(windowUUID) || deletions[windowUUID].empty())
        {
            return getDiffBytes(pos, lastValidPosition, windowUUID) + getCharacterBytesTaken(lastValidPosition, storage, windowUUID);
        }
        auto const greaterChangeIt = std::find_if(deletions[windowUUID].begin(), deletions[windowUUID].end(), [&](auto &el)
                                                  { return el.first > pos; });
        return greaterChangeIt != std::cend(deletions[windowUUID]) ? getDiffBytes(pos, greaterChangeIt->first, windowUUID) : getDiffBytes(pos, lastValidPosition, windowUUID) + getCharacterBytesTaken(lastValidPosition, storage, windowUUID);
    }

    std::pair<size_t, pair> PrintingOrchestrator::getDiffToPrevChanges(pair pos, std::shared_ptr<utils::storage::AbstractStorage> storage, utils::datatypes::Uuid const &windowUUID)
    {
        if (!changes.count(windowUUID) || changes[windowUUID].empty())
        {
            return std::make_pair(getDiffBytes(getFirstValidPosition(windowUUID), pos, windowUUID), getFirstValidPosition(windowUUID));
        }
        auto lowerBound = changes[windowUUID].lower_bound(pos.first);
        if (lowerBound == changes[windowUUID].cbegin())
        {
            return std::make_pair(getDiffBytes(getFirstValidPosition(windowUUID), pos, windowUUID), getFirstValidPosition(windowUUID));
        }
        pair endOfChangesPos = std::make_pair(std::prev(lowerBound)->first, getNumberOfChangesBytesAtPos(std::prev(lowerBound)->first, windowUUID));
        return std::make_pair(getDiffBytes(endOfChangesPos, pos, windowUUID), endOfChangesPos);
    }

    std::pair<size_t, pair> PrintingOrchestrator::getDiffToPrevReverseDeletion(pair pos, std::shared_ptr<utils::storage::AbstractStorage> storage, utils::datatypes::Uuid const &windowUUID)
    {
        if (!deletions.count(windowUUID) || deletions[windowUUID].empty())
        {
            return std::make_pair(getDiffBytes(getFirstValidPosition(windowUUID), pos, windowUUID), getFirstValidPosition(windowUUID));
        }
        auto lowerBound = reverseDeletions[windowUUID].lower_bound(pos);
        if (lowerBound == reverseDeletions[windowUUID].cbegin())
        {
            return std::make_pair(getDiffBytes(getFirstValidPosition(windowUUID), pos, windowUUID), getFirstValidPosition(windowUUID));
        }
        else if (lowerBound == reverseDeletions[windowUUID].cend())
        {
            return std::make_pair(getDiffBytes(reverseDeletions[windowUUID].crbegin()->first, pos, windowUUID), reverseDeletions[windowUUID].crbegin()->first);
        }
        return std::make_pair(getDiffBytes(std::prev(lowerBound)->first, pos, windowUUID), std::prev(lowerBound)->first);
    }

    size_t PrintingOrchestrator::getNumberOfChangesBytesAtPos(size_t pos, utils::datatypes::Uuid const &windowUUID) const
    {
        return changes.count(windowUUID) ? (changes.find(windowUUID)->second.count(pos) ? changes.find(windowUUID)->second.find(pos)->second : 0) : 0;
    }

    size_t PrintingOrchestrator::getNumberOfChangesBytesBetweenPos(size_t left, size_t right, utils::datatypes::Uuid const &windowUUID) const
    {
        return changes.count(windowUUID) ? std::accumulate(changes.find(windowUUID)->second.begin(), changes.find(windowUUID)->second.end(), 0, [&](size_t sum, auto e)
                                                           { return sum + (e.first > left && e.first < right ? e.second : 0); })
                                         : 0;
    }

    std::pair<pair, pair> PrintingOrchestrator::getRangeIncludingDeletions(std::pair<pair, pair> range, utils::datatypes::Uuid const &windowUUID)
    {
        if (isCharAtPosDeleted(range.first, windowUUID))
        {
            range.first = getDeletionEnd(windowUUID, range.first);
        }

        if (isCharAtPosDeleted(range.second, windowUUID))
        {
            range.second = getDeletionEnd(windowUUID, range.second);
        }
        return range;
    }

    pair PrintingOrchestrator::getFirstInvalidPosition(utils::datatypes::Uuid const &windowUUID, std::shared_ptr<utils::storage::AbstractStorage> storage)
    {
        auto lastValidPos = getLastValidPosition(windowUUID, storage);
        size_t diffToFirstInvalid = getDiffBytesWithoutDeletions(lastValidPos, pair(storage->getSize(), getNumberOfChangesBytesAtPos(storage->getSize(), windowUUID)), windowUUID);
        if (isCharAtPosWithChanges(lastValidPos.first, windowUUID))
        {
            return pair(lastValidPos.first, lastValidPos.second + diffToFirstInvalid);
        }
        return pair(lastValidPos.first + diffToFirstInvalid, 0);
    }

    pair PrintingOrchestrator::getFirstValidPosition(utils::datatypes::Uuid const &windowUUID) const
    {
        return deletions.count(windowUUID) ? (deletions[windowUUID].count(pair(0, 0)) ? deletions[windowUUID][pair(0, 0)] : pair(0, 0)) : pair(0, 0);
    }

    size_t PrintingOrchestrator::getTotalBytesOfDeletions(utils::datatypes::Uuid const &windowUUID) const
    {
        return (totalBytesOfDeletions.count(windowUUID) ? totalBytesOfDeletions.find(windowUUID)->second : 0);
    }

    size_t PrintingOrchestrator::getTotalBytesOfNewChanges(utils::datatypes::Uuid const &windowUUID) const
    {
        return totalBytesOfNewChanges.count(windowUUID) ? totalBytesOfNewChanges.find(windowUUID)->second : 0;
    }

    std::map<pair, pair> PrintingOrchestrator::getChangesPositions(utils::datatypes::Uuid const &windowUUID)
    {
        std::map<pair, pair> changesStartPositions;
        if (changes.count(windowUUID))
        {
            for (auto const &e : changes[windowUUID])
            {
                changesStartPositions.insert(std::make_pair(pair(e.first, 0), pair(e.first, e.second)));
            }
        }
        return changesStartPositions;
    }

    std::map<pair, pair> PrintingOrchestrator::getReverseDeletionsPositions(utils::datatypes::Uuid const &windowUUID)
    {
        std::map<pair, pair> deletionsPositions;
        if (reverseDeletions.count(windowUUID))
        {
            for (auto const &e : reverseDeletions[windowUUID])
            {
                deletionsPositions.insert(e);
            }
        }
        return deletionsPositions;
    }

    std::map<pair, pair> PrintingOrchestrator::getDeletionsPositions(utils::datatypes::Uuid const &windowUUID)
    {
        std::map<pair, pair> deletionsPositions;
        if (deletions.count(windowUUID))
        {
            for (auto const &e : deletions[windowUUID])
            {
                deletionsPositions.insert(e);
            }
        }
        return deletionsPositions;
    }

    void PrintingOrchestrator::swapDeletions(utils::datatypes::DeletionsSnapshot const &deletionsSnapshot, utils::datatypes::Uuid const &windowUUID)
    {
        deletions[windowUUID] = deletionsSnapshot.deletions;
        reverseDeletions[windowUUID] = deletionsSnapshot.reverseDeletions;
        totalBytesOfDeletions[windowUUID] = deletionsSnapshot.totalBytesOfDeletions;
    }

    std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, std::pair<pair, pair>>> PrintingOrchestrator::getLineToCopy() const
    {
        return copiedLine;
    }

    std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, std::pair<pair, pair>>> &PrintingOrchestrator::getTextToCopy() const
    {
        return textToCopy;
    }

    std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, std::map<pair, pair>>> PrintingOrchestrator::getTextToCopyPositionsToSkip() const
    {
        return textToCopyPositionsToSkip;
    }

    std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, std::pair<pair, pair>>> PrintingOrchestrator::getTextToCut() const
    {
        return textToCut;
    }

    bool PrintingOrchestrator::isLineCopied() const
    {
        return copiedLine.has_value() && filesystem->isFileExist(copiedLine->first.second);
    }

    bool PrintingOrchestrator::isTextCopied() const
    {
        return textToCopy.has_value() && filesystem->isFileExist(textToCopy->first.second);
    }

    bool PrintingOrchestrator::isTextCut() const
    {
        return textToCut.has_value() && filesystem->isFileExist(textToCut->first.second);
    }

    bool PrintingOrchestrator::isLineCopied(utils::datatypes::Uuid const &windowUUID) const
    {
        return copiedLine.has_value() && copiedLine->first.first == windowUUID && filesystem->isFileExist(copiedLine->first.second);
    }

    bool PrintingOrchestrator::isTextCopied(utils::datatypes::Uuid const &windowUUID) const
    {
        return textToCopy.has_value() && textToCopy->first.first == windowUUID && filesystem->isFileExist(textToCopy->first.second);
    }

    bool PrintingOrchestrator::isTextCut(utils::datatypes::Uuid const &windowUUID) const
    {
        return textToCut.has_value() && textToCut->first.first == windowUUID && filesystem->isFileExist(textToCut->first.second);
    }

    void PrintingOrchestrator::setLineToCopy(std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, std::pair<pair, pair>>> &&lineToCopyArg, std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, utils::datatypes::DeletionsSnapshot>> &&deletionsSnapshot)
    {
        copiedLine = lineToCopyArg;
        textToCutSnapshot = deletionsSnapshot;
    }

    void PrintingOrchestrator::setTextToCopy(std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, std::pair<pair, pair>>> const &textToCopyArg, std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, utils::datatypes::DeletionsSnapshot>> &&deletionsSnapshot)
    {
        textToCopy = textToCopyArg;
        textToCutSnapshot = deletionsSnapshot;
    }

    void PrintingOrchestrator::setTextToCopyPositionsToSkip(std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, std::map<pair, pair>>> const &textToCopyArg)
    {
        textToCopyPositionsToSkip = textToCopyArg;
    }

    void PrintingOrchestrator::setTextToCut(std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, std::pair<pair, pair>>> &&textToCutArg, std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, utils::datatypes::DeletionsSnapshot>> &&deletionsSnapshot)
    {
        textToCut = textToCutArg;
        textToCutSnapshot = deletionsSnapshot;
    }

    std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, utils::datatypes::DeletionsSnapshot>> PrintingOrchestrator::getTextToCutSnapshot() const
    {
        return textToCutSnapshot;
    }

    void PrintingOrchestrator::updateTextCopyPositionsToSkip(pair const &pos, size_t size, utils::datatypes::Uuid const &windowUUID)
    {
        bool shouldUpdateCopyPositionsToSkip = false;
        if (isLineCopied(windowUUID))
        {
            copiedLine->second = utils::helpers::Lambda::shiftInterval(copiedLine->second, pos, size);
        	shouldUpdateCopyPositionsToSkip = (pos >= copiedLine->second.first && pos < copiedLine->second.second);
        }
        else if (isTextCopied(windowUUID))
        {
            textToCopy->second = utils::helpers::Lambda::shiftInterval(textToCopy->second, pos, size);
        	shouldUpdateCopyPositionsToSkip = (pos >= textToCopy->second.first && pos < textToCopy->second.second);
        }
        else if (isTextCut(windowUUID))
        {
            textToCut->second = utils::helpers::Lambda::shiftInterval(textToCut->second, pos, size);
        }

        if (isLineCopied(windowUUID) || isTextCopied(windowUUID) || isTextCut(windowUUID))
        {
            //Update positions to skip
            std::map<pair, pair> textToCopyPositionsToSkipUpdated;
            for (auto const &e : textToCopyPositionsToSkip->second)
            {
                textToCopyPositionsToSkipUpdated.insert(utils::helpers::Lambda::shiftInterval(e, pos, size));
            }
            if (shouldUpdateCopyPositionsToSkip)
            {
            	textToCopyPositionsToSkipUpdated = utils::helpers::Lambda::mergeIntervals(textToCopyPositionsToSkipUpdated, std::map<pair, pair>{{pos, pair{pos.first, pos.second + size}}});
            }
            textToCopyPositionsToSkip->second.swap(textToCopyPositionsToSkipUpdated);

			//Update deletions snapshot
            textToCutSnapshot->second.shift(pos, size);
        }
    }

    PrintingOrchestrator::~PrintingOrchestrator()
    {
    }

    std::unordered_map<utils::datatypes::Uuid, std::chrono::nanoseconds, utils::datatypes::UuidHasher> PrintingOrchestrator::lastStorageModificationTimestamp;
    std::unordered_map<utils::datatypes::Uuid, std::chrono::nanoseconds, utils::datatypes::UuidHasher> PrintingOrchestrator::lastStoragePositionCheck;
    std::unordered_map<utils::datatypes::Uuid, pair, utils::datatypes::UuidHasher> PrintingOrchestrator::lastValidPosition;
    std::vector<utils::datatypes::COLOR> PrintingOrchestrator::BACKGROUND_COLORS = {{utils::datatypes::COLOR::FEATHER_COLOR_BLACK_BCKG, utils::datatypes::COLOR::FEATHER_COLOR_RED_BCKG, utils::datatypes::COLOR::FEATHER_COLOR_GREEN_BCKG, utils::datatypes::COLOR::FEATHER_COLOR_YELLOW_BCKG, utils::datatypes::COLOR::FEATHER_COLOR_BLUE_BCKG, utils::datatypes::COLOR::FEATHER_COLOR_MAGENTA_BCKG, utils::datatypes::COLOR::FEATHER_COLOR_CYAN_BCKG, utils::datatypes::COLOR::FEATHER_COLOR_WHITE_BCKG}};
    std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, std::pair<pair, pair>>> PrintingOrchestrator::copiedLine;
    std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, std::pair<pair, pair>>> PrintingOrchestrator::textToCopy;
    std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, std::map<pair, pair>>> PrintingOrchestrator::textToCopyPositionsToSkip;
    std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, std::pair<pair, pair>>> PrintingOrchestrator::textToCut;
    std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, utils::datatypes::DeletionsSnapshot>> PrintingOrchestrator::textToCutSnapshot;
} // namespace feather::printer
