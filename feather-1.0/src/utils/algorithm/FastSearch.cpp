#include "utils/Filesystem.hpp"
#include "utils/Utf8Util.hpp"
#include "utils/algorithm/FastSearch.hpp"
#include "utils/algorithm/RegexSearch.hpp"
#include "utils/datatypes/Strings.hpp"
#include "utils/exception/FeatherInterruptedException.hpp"
#include "utils/helpers/Signal.hpp"

#include <experimental/functional>

extern bool exitFeather;

extern sig_atomic_t sigIntReceived;
extern sig_atomic_t sigBusReceived;

namespace feather::utils::algorithm
{
    FastSearch::FastSearch(utils::datatypes::Uuid const &windowUUIDArg, std::u32string &&searchedStringArg, std::shared_ptr<utils::storage::AbstractStorageFactory> fileStorageFactoryArg, std::shared_ptr<utils::storage::AbstractStorageFactory> storageFactoryArg, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestratorArg, std::unique_ptr<utils::BufferFillerInterface> const &bufferFillerArg, pair cursorPosArg, std::list<pair> &breakPointsArg, std::list<pair>::iterator &currentBreakPointArg, std::set<pair> &breakPointsAddedArg, bool isHexSearchArg)
        : SearchEngineBase(windowUUIDArg, std::move(searchedStringArg), fileStorageFactoryArg, storageFactoryArg, printingOrchestratorArg, bufferFillerArg, breakPointsArg, currentBreakPointArg, breakPointsAddedArg), firstSearch(true), isHexSearch(isHexSearchArg), cursorPos(cursorPosArg), zooKeeper(cursorPosArg.first, lastPos)
    {
        escReceived = signalReceived = false;
        flatteredSearchedString = utils::helpers::Conversion::squeezeu32String(searchedString);
        lastInitialToResultPositionVisited = lastInitialFromResultPositionVisited = lastSecondaryResultPositionVisited = std::nullopt;
        lastCursorPos = std::make_pair(cursorPos, 0);
        secondarySearchFinished = initialSearchToFinished = initialSearchFromFinished = true;
        resultsController = std::make_unique<SearchResultsController>(flatteredSearchedString, bufferFiller->getStorage(), printingOrchestrator, windowUUID, cursorPosArg);
    }

    std::pair<std::optional<std::pair<pair, size_t>>, utils::datatypes::ERROR> FastSearch::getNextMatch(std::shared_ptr<feather::windows::WindowsManager> windowsManager, std::unique_ptr<printer::PrintingVisitorInterface> &refreshVisitor, std::unique_ptr<printer::PrintingVisitorInterface> &halfFrameUpVisitor, std::unique_ptr<feather::windows::SubWindowInterface> &currBottomBarWindow, utils::FEATHER_MODE &currentFeatherMode, utils::Direction searchDirection)
    {
        bool frameSetEarly = false, fromThreadCapped = false, toThreadCapped = false;
        size_t currFromResults = 0, currToResults = 0;
        if (std::nullopt != currBottomBarWindow->getParentWindowHandler()->getStorage()->reload())
        {
            fromCursorStorage->reload();
            toCursorStorage->reload();
        }
        if (currBottomBarWindow->getParentWindowHandler()->getStorage()->getLastModificationTimestamp() > resultsController->getLastSearchTimestamp())
        {
            SearchResultsController::clearResults(currBottomBarWindow->getParentWindowHandler()->getUUID());
            lastPos = currBottomBarWindow->getParentWindowHandler()->getStorage()->getSize();
            cursorPos = currBottomBarWindow->getParentWindowHandler()->getCursorRealPosition(printingOrchestrator.get());
            lastCursorPos = std::make_pair(cursorPos, 0);
            zooKeeper = SearchZooKeeper(cursorPos.first, lastPos);
        }
        std::future<void> initialToCursorSearchFuture, initialFromCursorSearchFuture, secondarySearchResultFuture;
        try
        {
            utils::helpers::unblockSignals();
            // If any changes has been made, reinitialize secondary search from scratch
            auto lastModTimestamp = printingOrchestrator->getLastStorageModificationTimestamp(windowUUID);
            if (lastModTimestamp.has_value() && lastModTimestamp.value() > resultsController->getLastSearchTimestamp())
            {
                removePreviousSecondaryChangesResults();
            }

            if (!resultsController->isSecondarySearchAlreadyPerformed(flatteredSearchedString, windowUUID))
            {
                resultsController->markSecondarySearchResult(flatteredSearchedString, windowUUID);
                secondarySearchFinished = false;
                secondarySearchResultFuture = std::async(std::launch::async, &FastSearch::searchBetweenNewChanges, this);
            }

            if (!resultsController->isSearchAlreadyPerformed(flatteredSearchedString, windowUUID))
            {
                initialSearchToFinished = initialSearchFromFinished = false;
                resultsController->markInitialSearchResult(flatteredSearchedString, windowUUID);
#ifndef _FEATHER_TEST_
                utils::helpers::Lambda::subWindowPrint(utils::Direction::DOWN == searchDirection ? utils::datatypes::Strings::SEARCHING_DOWN : utils::datatypes::Strings::SEARCHING_UP, std::nullopt, windowsManager->getSubWindow(windowsManager->getBottomBarWindow(windowUUID).value()), windowsManager->getSubWindow(windowsManager->getProgressBarWindow(windowUUID).value()), refreshVisitor);
#endif
                auto nextInterval = zooKeeper.getNextInterval();
                if (nextInterval.has_value())
                {
                    fromCursorSearchBuffer.resize(nextInterval->first);
                    fromCursorStorage->setCursor(nextInterval->second);
                    fromCursorStorage->readChunkAsString(fromCursorSearchBuffer, nextInterval->first, false);
                    initialFromCursorSearchFuture = std::async(std::launch::async, &FastSearch::initialFromCursorSearchAsync, this, nextInterval->second, std::ref(currFromResults), std::ref(fromThreadCapped));
                }
                auto prevInterval = zooKeeper.getPrevInterval();
                if (prevInterval.has_value())
                {
                    toCursorSearchBuffer.resize(prevInterval->first);
                    toCursorStorage->setCursor(prevInterval->second);
                    toCursorStorage->readChunkAsString(toCursorSearchBuffer, prevInterval->first, false);
                    initialToCursorSearchFuture = std::async(std::launch::async, &FastSearch::initialToCursorSearchAsync, this, prevInterval->second, std::ref(currToResults), std::ref(toThreadCapped));
                }

                do
                {
                    if (sigIntReceived)
                    {
                        currBottomBarWindow->getParentWindowHandler()->disableCursor();
                        while (!isFutureReady(initialFromCursorSearchFuture) || !isFutureReady(initialToCursorSearchFuture) || !isFutureReady(secondarySearchResultFuture))
                            ;
                        utils::helpers::blockSignals();
                        resultsController->clearAllSearches(flatteredSearchedString, windowUUID);
                        return std::make_pair(lastCursorPos, utils::datatypes::ERROR::INTERRUPTED);
                    }
                    else if (sigBusReceived)
                    {
                        currBottomBarWindow->getParentWindowHandler()->disableCursor();
                        while (!isFutureReady(initialFromCursorSearchFuture) || !isFutureReady(initialToCursorSearchFuture) || !isFutureReady(secondarySearchResultFuture))
                            ;
                        utils::helpers::blockSignals();
                        resultsController->clearAllSearches(flatteredSearchedString, windowUUID);
                        return std::make_pair(std::nullopt, utils::datatypes::ERROR::FILE_NO_LONGER_AVAILABLE);
                    }
                    else if (escReceived)
                    {
                        currBottomBarWindow->getParentWindowHandler()->disableCursor();
                        while (!isFutureReady(initialFromCursorSearchFuture) || !isFutureReady(initialToCursorSearchFuture) || !isFutureReady(secondarySearchResultFuture))
                            ;
                        resultsController->clearAllSearches(flatteredSearchedString, windowUUID);
                        return std::make_pair(lastCursorPos, utils::datatypes::ERROR::ESC_INTERRUPTED);
                    }
                    checkIfEarlyResultAvailable(windowsManager, refreshVisitor, halfFrameUpVisitor, currBottomBarWindow, currentFeatherMode, zooKeeper.bytesTaken(), searchDirection, frameSetEarly, fromThreadCapped && toThreadCapped);
                    if (prevInterval.has_value() && isFutureReady(initialToCursorSearchFuture) && !toThreadCapped)
                    {
                        initialToCursorSearchFuture.get();
#ifndef _FEATHER_TEST_
                        if (currentFeatherMode == utils::FEATHER_MODE::SEARCHING_IN_PROGRESS_DOWN || currentFeatherMode == utils::FEATHER_MODE::SEARCHING_IN_PROGRESS_UP)
                        {
                            windowsManager->refreshProgresWindow(refreshVisitor, windowUUID, std::make_pair(utils::windows::FramePositions(pair(zooKeeper.bytesTaken(), 0), pair{zooKeeper.bytesTaken(), 0}), pair(zooKeeper.bytesTaken(), 0)));
                            utils::NCursesWrapper::applyRefresh();
                        }
#endif
                        auto savedPrevInterval = prevInterval;
                        prevInterval = zooKeeper.getPrevInterval();
                        if (prevInterval.has_value())
                        {
                            if (savedPrevInterval->first != prevInterval->first)
                            {
                                toCursorSearchBuffer.resize(prevInterval->first);
                            }
                            toCursorStorage->setCursor(prevInterval->second);
                            auto chunkReadAsync = std::async(std::launch::async, &utils::storage::AbstractStorage::readChunkAsString, toCursorStorage.get(), std::ref(toCursorSearchBuffer), prevInterval->first, false);
                            while (!isFutureReady(chunkReadAsync))
                            {
                                checkIfEarlyResultAvailable(windowsManager, refreshVisitor, halfFrameUpVisitor, currBottomBarWindow, currentFeatherMode, zooKeeper.bytesTaken(), searchDirection, frameSetEarly, fromThreadCapped && toThreadCapped);
                            }
                            initialToCursorSearchFuture = std::async(std::launch::async, &FastSearch::initialToCursorSearchAsync, this, prevInterval->second, std::ref(currToResults), std::ref(toThreadCapped));
                        }
                    }

                    if (nextInterval.has_value() && isFutureReady(initialFromCursorSearchFuture) && !fromThreadCapped)
                    {
                        initialFromCursorSearchFuture.get();
#ifndef _FEATHER_TEST_
                        if (currentFeatherMode == utils::FEATHER_MODE::SEARCHING_IN_PROGRESS_DOWN || currentFeatherMode == utils::FEATHER_MODE::SEARCHING_IN_PROGRESS_DOWN)
                        {
                            windowsManager->refreshProgresWindow(refreshVisitor, windowUUID, std::make_pair(utils::windows::FramePositions(pair(zooKeeper.bytesTaken(), 0), pair{zooKeeper.bytesTaken(), 0}), pair(zooKeeper.bytesTaken(), 0)));
                            utils::NCursesWrapper::applyRefresh();
                        }
#endif
                        auto savedNextInterval = nextInterval;
                        nextInterval = zooKeeper.getNextInterval();
                        if (nextInterval.has_value())
                        {
                            if (savedNextInterval->first != nextInterval->first)
                            {
                                fromCursorSearchBuffer.resize(nextInterval->first);
                            }
                            fromCursorStorage->setCursor(nextInterval->second);
                            auto chunkReadAsync = std::async(std::launch::async, &utils::storage::AbstractStorage::readChunkAsString, fromCursorStorage.get(), std::ref(fromCursorSearchBuffer), nextInterval->first, false);
                            while (!isFutureReady(chunkReadAsync))
                            {
                                checkIfEarlyResultAvailable(windowsManager, refreshVisitor, halfFrameUpVisitor, currBottomBarWindow, currentFeatherMode, zooKeeper.bytesTaken(), searchDirection, frameSetEarly, fromThreadCapped && toThreadCapped);
                            }
                            initialFromCursorSearchFuture = std::async(std::launch::async, &FastSearch::initialFromCursorSearchAsync, this, nextInterval->second, std::ref(currFromResults), std::ref(fromThreadCapped));
                        }
                    }
                    if (!prevInterval.has_value() || toThreadCapped)
                    {
                        initialSearchToFinished = true;
                    }
                    if (!nextInterval.has_value() || fromThreadCapped)
                    {
                        initialSearchFromFinished = true;
                    }
                } while (!initialSearchToFinished || !initialSearchFromFinished);
            }
            auto fileContentChanged = currBottomBarWindow->getParentWindowHandler()->getStorage()->reload();
            if (fileContentChanged.has_value() && fileContentChanged.value().first < fileContentChanged.value().second)
            {
                fromCursorStorage->reload();
                // Start catching up results
                size_t catchUpCursorPos = ((fileContentChanged.value().second - fileContentChanged.value().first) < (flatteredSearchedString.length() - 1UL)) || fileContentChanged.value().first < flatteredSearchedString.length() ? 0 : fileContentChanged.value().first - (flatteredSearchedString.length() - 1UL);
                fromCursorStorage->setCursor(catchUpCursorPos);
                size_t interval = std::min(configuration->getMemoryBytesRelaxed(), fileContentChanged.value().first < flatteredSearchedString.length() ? 0 : fileContentChanged.value().second - catchUpCursorPos);
                while (interval >= flatteredSearchedString.length())
                {
                    if (interval != fromCursorSearchBuffer.size())
                    {
                        fromCursorSearchBuffer.resize(interval);
                    }
                    fromCursorStorage->readChunkAsString(fromCursorSearchBuffer, interval, false);
                    initialFromCursorSearchAsync(catchUpCursorPos, std::ref(currFromResults), std::ref(fromThreadCapped));
                    interval -= (flatteredSearchedString.length() - 1UL);
                    catchUpCursorPos += interval;
                    interval = std::min(configuration->getMemoryBytesRelaxed(), fileContentChanged.value().first < (flatteredSearchedString.length() - 1UL) ? 0 : fileContentChanged.value().second - catchUpCursorPos);
                }
            }

            if (!secondarySearchFinished)
            {
                while (!isFutureReady(secondarySearchResultFuture))
                    ;
            }
            // Case where search has already been performed
            secondarySearchFinished = initialSearchToFinished = initialSearchFromFinished = true;

            if (utils::Direction::DOWN == searchDirection)
            {
                // Check if we have stopped in partial search when searching for the first time
                if (lastCursorPos.first != cursorPos)
                {
                    cursorPos = lastCursorPos.first;
                    return std::make_pair(lastCursorPos, utils::datatypes::ERROR::NONE);
                }
                else
                {
                    auto nextResult = frameSetEarly ? lastCursorPos : resultsController->getNextSearchResult(flatteredSearchedString, windowUUID, false);
                    if (nextResult.has_value())
                    {
                        lastCursorPos = nextResult.value();
                        cursorPos = lastCursorPos.first;
                        return std::make_pair(nextResult, utils::datatypes::ERROR::NONE);
                    }
                    return std::make_pair(std::nullopt, utils::datatypes::ERROR::NONE);
                }
            }
            else if (utils::Direction::UP == searchDirection)
            {
                auto prevResult = frameSetEarly ? lastCursorPos : resultsController->getPrevSearchResult(flatteredSearchedString, windowUUID, false);
                if (prevResult.has_value())
                {
                    lastCursorPos = prevResult.value();
                    cursorPos = lastCursorPos.first;
                    return std::make_pair(prevResult, utils::datatypes::ERROR::NONE);
                }
                return std::make_pair(std::nullopt, utils::datatypes::ERROR::NONE);
            }
            return std::make_pair(std::nullopt, utils::datatypes::ERROR::NONE);
        }
        catch (std::exception const &e)
        {
            utils::helpers::blockSignals();
            exitFeather = true;
            while (!isFutureReady(initialToCursorSearchFuture) || !isFutureReady(initialFromCursorSearchFuture) || !isFutureReady(secondarySearchResultFuture))
                ;
            throw e;
        }
    }

    void FastSearch::checkIfEarlyResultAvailable(std::shared_ptr<feather::windows::WindowsManager> &windowsManager, std::unique_ptr<printer::PrintingVisitorInterface> &refreshVisitor, std::unique_ptr<printer::PrintingVisitorInterface> &halfFrameUpVisitor, std::unique_ptr<feather::windows::SubWindowInterface> &currBottomBarWindow, utils::FEATHER_MODE &currentFeatherMode, size_t offset, utils::Direction &searchDirection, bool &frameSetEarly, bool searchIsCapped)
    {
#ifndef _FEATHER_TEST_
        if (firstSearch && secondarySearchFinished)
        {
            auto earlyResult = (utils::Direction::DOWN == searchDirection) ? resultsController->getNextSearchResult(flatteredSearchedString, windowUUID, true, lastInitialToResultPositionVisited, lastInitialFromResultPositionVisited) : resultsController->getPrevSearchResult(flatteredSearchedString, windowUUID, true, lastInitialToResultPositionVisited, lastInitialFromResultPositionVisited);
            if (earlyResult.has_value())
            {
#ifndef _FEATHER_TEST_
                currentFeatherMode = (utils::Direction::DOWN == searchDirection) ? utils::FEATHER_MODE::BROWSE_SEARCH_RESULT_DOWN_MODE : utils::FEATHER_MODE::BROWSE_SEARCH_RESULT_UP_MODE;
                utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, refreshVisitor, halfFrameUpVisitor, earlyResult.value().first);
                printingOrchestrator->setSearchedText(currBottomBarWindow->getParentWindowHandler()->getUUID(), earlyResult);
                windowsManager->refreshAllWindows(refreshVisitor, {windowsManager->getProgressBarWindow(windowUUID).value()});
                utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, refreshVisitor, halfFrameUpVisitor, earlyResult.value().first);
                utils::helpers::Lambda::subWindowPrint((searchDirection == utils::Direction::DOWN) ? utils::datatypes::Strings::BROWSE_SEARCH_DOWN_MODE : utils::datatypes::Strings::BROWSE_SEARCH_UP_MODE, std::nullopt, currBottomBarWindow, windowsManager->getSubWindow(windowsManager->getProgressBarWindow(currBottomBarWindow->getParentWindowHandler()->getUUID()).value()), refreshVisitor);
                utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, refreshVisitor, halfFrameUpVisitor, earlyResult.value().first);
                windowsManager->refreshProgresWindow(refreshVisitor, currBottomBarWindow->getParentWindowHandler()->getUUID(), std::make_pair(currBottomBarWindow->getParentWindowHandler()->getFramePosition(), earlyResult.value().first));
                currBottomBarWindow->getParentWindowHandler()->enableCursor();
#endif
                lastCursorPos = earlyResult.value();
                firstSearch = false;
                frameSetEarly = true;
            }
            else
            {
                firstSearch = true;
            }
        }

        if (!searchIsCapped || !frameSetEarly)
        {
            timeout(0);
        }
        // Check if window dimensions has not changed
        switch (windowsManager->getMainWindow(windowUUID)->getLibraryWindowHandler()->getCharacter())
        {
        case KEY_RESIZE:
        {
            windowsManager->recreateWindows();
            windowsManager->refreshAllWindows(refreshVisitor, {windowsManager->getProgressBarWindow(windowUUID).value()});
            if (currentFeatherMode == utils::FEATHER_MODE::BROWSE_SEARCH_RESULT_DOWN_MODE || currentFeatherMode == utils::FEATHER_MODE::BROWSE_SEARCH_RESULT_UP_MODE)
            {
                utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, refreshVisitor, halfFrameUpVisitor, lastCursorPos.first);
                windowsManager->refreshProgresWindow(refreshVisitor, currBottomBarWindow->getParentWindowHandler()->getUUID(), std::make_pair(currBottomBarWindow->getParentWindowHandler()->getFramePosition(), lastCursorPos.first));
                currBottomBarWindow->getParentWindowHandler()->enableCursor();
            }
            utils::NCursesWrapper::applyRefresh();
            break;
        }
        case U'n':
        {
            currBottomBarWindow->getParentWindowHandler()->getStorage()->reload();
            auto nextResult = resultsController->getNextSearchResult(flatteredSearchedString, windowUUID, true, lastInitialToResultPositionVisited, lastInitialFromResultPositionVisited);
            if (nextResult.has_value())
            {
                currentFeatherMode = utils::FEATHER_MODE::BROWSE_SEARCH_RESULT_DOWN_MODE;
                utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, refreshVisitor, halfFrameUpVisitor, nextResult.value().first);
                printingOrchestrator->setSearchedText(currBottomBarWindow->getParentWindowHandler()->getUUID(), nextResult);
                windowsManager->refreshAllWindows(refreshVisitor, {windowsManager->getProgressBarWindow(windowUUID).value()});
                utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, refreshVisitor, halfFrameUpVisitor, nextResult.value().first);
                windowsManager->refreshProgresWindow(refreshVisitor, currBottomBarWindow->getParentWindowHandler()->getUUID(), std::make_pair(currBottomBarWindow->getParentWindowHandler()->getFramePosition(), nextResult.value().first));
                currBottomBarWindow->getParentWindowHandler()->enableCursor();
                lastCursorPos = nextResult.value();
                firstSearch = false;
            }
            else
            {
                currentFeatherMode = utils::FEATHER_MODE::SEARCHING_IN_PROGRESS_DOWN;
                printingOrchestrator->setSearchedText(currBottomBarWindow->getParentWindowHandler()->getUUID(), std::nullopt);
                windowsManager->refreshAllWindows(refreshVisitor, {windowsManager->getProgressBarWindow(windowUUID).value()});
                currBottomBarWindow->getParentWindowHandler()->disableCursor();
                firstSearch = true;
                searchDirection = utils::Direction::DOWN;
            }
            utils::NCursesWrapper::applyRefresh();
            break;
        }
        case U'N':
        {
            currBottomBarWindow->getParentWindowHandler()->getStorage()->reload();
            auto prevResult = resultsController->getPrevSearchResult(flatteredSearchedString, windowUUID, true, lastInitialToResultPositionVisited, lastInitialFromResultPositionVisited);
            if (prevResult.has_value())
            {
                currentFeatherMode = utils::FEATHER_MODE::BROWSE_SEARCH_RESULT_UP_MODE;
                utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, refreshVisitor, halfFrameUpVisitor, prevResult.value().first);
                printingOrchestrator->setSearchedText(currBottomBarWindow->getParentWindowHandler()->getUUID(), prevResult);
                windowsManager->refreshAllWindows(refreshVisitor, {windowsManager->getProgressBarWindow(windowUUID).value()});
                utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, refreshVisitor, halfFrameUpVisitor, prevResult.value().first);
                windowsManager->refreshProgresWindow(refreshVisitor, currBottomBarWindow->getParentWindowHandler()->getUUID(), std::make_pair(currBottomBarWindow->getParentWindowHandler()->getFramePosition(), prevResult.value().first));
                currBottomBarWindow->getParentWindowHandler()->enableCursor();
                lastCursorPos = prevResult.value();
                firstSearch = false;
            }
            else
            {
                currentFeatherMode = utils::FEATHER_MODE::SEARCHING_IN_PROGRESS_UP;
                printingOrchestrator->setSearchedText(currBottomBarWindow->getParentWindowHandler()->getUUID(), std::nullopt);
                windowsManager->refreshAllWindows(refreshVisitor, {windowsManager->getProgressBarWindow(windowUUID).value()});
                currBottomBarWindow->getParentWindowHandler()->disableCursor();
                firstSearch = true;
                searchDirection = utils::Direction::UP;
            }
            utils::NCursesWrapper::applyRefresh();
            break;
        }
        case ctrl(U'b'): // Add break point
        {
            addBreakpoint(currBottomBarWindow, halfFrameUpVisitor, windowsManager);
            break;
        }
        case 27:
        {
            if (currentFeatherMode != utils::FEATHER_MODE::SEARCHING_IN_PROGRESS_DOWN && currentFeatherMode != utils::FEATHER_MODE::SEARCHING_IN_PROGRESS_UP)
            {
                currBottomBarWindow->disableCursor();
                utils::helpers::Lambda::subWindowPrint(utils::datatypes::Strings::EMPTY_STRING, std::nullopt, currBottomBarWindow, windowsManager->getSubWindow(windowsManager->getProgressBarWindow(currBottomBarWindow->getParentWindowHandler()->getUUID()).value()), refreshVisitor);
                printingOrchestrator->setSearchedText(currBottomBarWindow->getParentWindowHandler()->getUUID(), std::nullopt);
                escReceived = true;
            }
        }
        }
        timeout(-1);
#endif
    }

    void FastSearch::initialToCursorSearchAsync(size_t offset, size_t &currToResults, bool &toThreadCapped)
    {
        utils::helpers::unblockSignals();
        try
        {
            auto result = isHexSearch ? RegexSearch::searchHex(toCursorSearchBuffer, flatteredSearchedString, offset, printingOrchestrator, windowUUID) : RegexSearch::search(toCursorSearchBuffer, searchedString, offset, printingOrchestrator, windowUUID);
            bool shouldCapResults = false;
            if (!result.empty())
            {
                if (result.size() >= resultsController->getCapSize() - currToResults)
                {
                    result.erase(result.begin(), std::next(result.begin(), result.size() - (resultsController->getCapSize() - currToResults)));
                    currToResults = resultsController->getCapSize();
                    shouldCapResults = true;
                }
                else
                {
                    currToResults += result.size();
                }
                resultsController->addResult(flatteredSearchedString, std::move(result), windowUUID);
            }
            lastInitialToResultPositionVisited = {{offset, 0}, 0};
            signalReceived = false;
            if (shouldCapResults)
            {
                toThreadCapped = true;
            }
        }
        catch (utils::exception::FeatherInterruptedException const &e)
        {
            return;
        }
    }

    void FastSearch::initialFromCursorSearchAsync(size_t offset, size_t &currFromResults, bool &fromThreadCapped)
    {
        utils::helpers::unblockSignals();
        try
        {
            auto result = isHexSearch ? RegexSearch::searchHex(fromCursorSearchBuffer, flatteredSearchedString, offset, printingOrchestrator, windowUUID) : RegexSearch::search(fromCursorSearchBuffer, searchedString, offset, printingOrchestrator, windowUUID);
            bool shouldCapResults = false;
            if (!result.empty())
            {
                if (result.size() >= resultsController->getCapSize() - currFromResults)
                {
                    result.erase(std::next(result.begin(), resultsController->getCapSize() - currFromResults), result.end());
                    currFromResults = resultsController->getCapSize();
                    shouldCapResults = true;
                }
                else
                {
                    currFromResults += result.size();
                }
                resultsController->addResult(flatteredSearchedString, std::move(result), windowUUID);
            }
            lastInitialFromResultPositionVisited = {{offset + fromCursorSearchBuffer.size(), 0}, 0};
            signalReceived = false;
            if (shouldCapResults)
            {
                fromThreadCapped = true;
            }
        }
        catch (utils::exception::FeatherInterruptedException const &e)
        {
            return;
        }
    }

    void FastSearch::removePreviousSecondaryChangesResults()
    {
        resultsController->clearSecondaryResults(flatteredSearchedString, windowUUID, printingOrchestrator);
        SearchResultsController::resetLastSearchTimestamp();
    }

    void FastSearch::searchBetweenNewChanges()
    {
        SearchResultsController::resetLastSearchTimestamp();
        auto allChangesPositions = printingOrchestrator->getChangesPositions(windowUUID);
        auto allDeletionsPositions = printingOrchestrator->getDeletionsPositions(windowUUID);
        auto mergedChanges = helpers::Lambda::mergeIntervals(allChangesPositions, allDeletionsPositions);
        std::stringstream localBatchResults;
        std::map<pair, pair> intervals;
        for (auto const &p : mergedChanges)
        {
            if (printingOrchestrator->isCharAtPosDeleted(p.first, windowUUID))
            {
                intervals.insert(std::make_pair(printingOrchestrator->getDeletionEnd(windowUUID, p.first), p.second));
                continue;
            }
            intervals.insert(p);
        }
        std::map<pair, pair> intervalsMerged = utils::helpers::Lambda::mergeInterval(intervals, windowUUID, printingOrchestrator, searchedStringWithoutLastChar[flatteredSearchedString].length());

        for (auto const &change : intervalsMerged)
        {
            auto searchInterval = getNextSearchInterval(change, flatteredSearchedString, printingOrchestrator, secondaryCursorStorage, windowUUID);
            if (!searchInterval.second)
            {
                continue;
            }
            pair startPos = searchInterval.first;
            size_t bytesToCheck = searchInterval.second;
            while (bytesToCheck && !signalReceived && !escReceived)
            {
                if (exitFeather)
                {
                    return;
                }
                auto chunk = printingOrchestrator->getContinousCharactersAsString(startPos, std::min(bytesToCheck, configuration->getMemoryBytesRelaxed()), secondaryCursorStorage, windowUUID);
                if (chunk.first.empty())
                {
                    break;
                }
                auto result = isHexSearch ? RegexSearch::searchHex(chunk.first, flatteredSearchedString, startPos, printingOrchestrator, secondaryCursorStorage, chunk.first, windowUUID) : RegexSearch::search(chunk.first, searchedString, startPos, printingOrchestrator, secondaryCursorStorage, chunk.first, windowUUID);
                for (auto &r : result)
                {
                    resultsController->addToSecondaryResults(flatteredSearchedString, r, windowUUID);
                    lastSecondaryResultPositionVisited = r;
                }
                if (bytesToCheck <= chunk.first.size())
                {
                    break;
                }
                bytesToCheck -= chunk.first.size();
                pair oldStartPos = startPos;
                startPos = chunk.second;
                for (size_t idx = 0; bytesToCheck && idx < MAX_SEARCH_RESULT_DELTA && startPos > oldStartPos; ++idx)
                {
                    startPos = printingOrchestrator->getPreviousVirtualPosition(windowUUID, secondaryCursorStorage, startPos);
                }

                if (startPos == oldStartPos)
                {
                    startPos = printingOrchestrator->getNextVirtualPosition(windowUUID, secondaryCursorStorage, startPos);
                }
                bytesToCheck += printingOrchestrator->getDiffBytesWithoutDeletions(startPos, chunk.second, windowUUID);
            }
        }
        secondarySearchFinished = true;
    }

    FastSearch::~FastSearch()
    {
    }
} // namespace feather::utils::algorithm