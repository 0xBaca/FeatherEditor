#include "utils/Utf8Util.hpp"
#include "utils/algorithm/SearchResultsController.hpp"

extern feather::utils::FEATHER_MODE currentFeatherMode;

namespace feather::utils::algorithm
{
    SearchResultsController::SearchResultsController(std::string const &searchedString, std::shared_ptr<utils::storage::AbstractStorage> storageArg, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestratorArg, utils::datatypes::Uuid const &windowUUID, pair cursorPosArg) : cursorPos(cursorPosArg), printingOrchestrator(printingOrchestratorArg), storage(storageArg)
    {
        if (!results.count(windowUUID) || !results[windowUUID].count(searchedString))
        {
            results[windowUUID][searchedString] = {std::make_pair(std::map<pair, size_t>{}, std::map<pair, size_t>::iterator()), std::map<pair, size_t>()};
        }
        results[windowUUID][searchedString].first.second = std::end(results[windowUUID][searchedString].first.first);
    }

    std::map<pair, size_t>::iterator SearchResultsController::getFirstPosBefore(std::string const &searchedString, pair pos, bool early, std::optional<std::pair<pair, size_t>> prevLastSeenResult, utils::datatypes::Uuid const &windowUUID)
    {
        if (getInitialResults(searchedString, windowUUID).empty() || (early && !prevLastSeenResult.has_value()))
        {
            return getEndPosition(searchedString, windowUUID);
        }
        auto greaterOrEqual = results[windowUUID][searchedString].first.first.lower_bound(pos);
        if (early)
        {
            if (isEndIterator(searchedString, greaterOrEqual, windowUUID))
            {
                if (prevLastSeenResult.value().first <= cursorPos)
                {
                    return std::prev(getEndPosition(searchedString, windowUUID))->first >= prevLastSeenResult.value().first ? std::prev(getEndPosition(searchedString, windowUUID)) : getEndPosition(searchedString, windowUUID);
                }
                else
                {
                    return std::prev(getEndPosition(searchedString, windowUUID));
                }
            }
            else if (isBeginIterator(searchedString, greaterOrEqual, windowUUID))
            {

                if (prevLastSeenResult.value().first < cursorPos)
                {
                    return getEndPosition(searchedString, windowUUID);
                }
                else
                {
                    return std::prev(getEndPosition(searchedString, windowUUID))->first >= prevLastSeenResult.value().first ? std::prev(getEndPosition(searchedString, windowUUID)) : getEndPosition(searchedString, windowUUID);
                }
            }
            else
            {
                if (prevLastSeenResult.value().first < cursorPos)
                {
                    return std::prev(greaterOrEqual)->first > prevLastSeenResult.value().first ? std::prev(greaterOrEqual) : getEndPosition(searchedString, windowUUID);
                }
                else
                {
                    return std::prev(greaterOrEqual);
                }
            }
        }
        return isEndIterator(searchedString, greaterOrEqual, windowUUID) ? std::prev(getEndPosition(searchedString, windowUUID)) : isBeginIterator(searchedString, greaterOrEqual, windowUUID) ? std::prev(getEndPosition(searchedString, windowUUID))
                                                                                                                                                                                               : std::prev(greaterOrEqual);
    }

    std::map<pair, size_t>::iterator SearchResultsController::getFirstPosAfter(std::string const &searchedString, pair pos, bool early, std::optional<std::pair<pair, size_t>> nextLastSeenResult, utils::datatypes::Uuid const &windowUUID)
    {
        if (getInitialResults(searchedString, windowUUID).empty() || (early && !nextLastSeenResult.has_value()))
        {
            return getEndPosition(searchedString, windowUUID);
        }
        auto greaterOrEqual = getInitialResults(searchedString, windowUUID).lower_bound(pos);
        if (early)
        {
            if (isEndIterator(searchedString, greaterOrEqual, windowUUID))
            {
                if (nextLastSeenResult.value().first >= cursorPos)
                {
                    return getEndPosition(searchedString, windowUUID);
                }
                else
                {
                    return getBeginPosition(searchedString, windowUUID)->first <= nextLastSeenResult.value().first ? getBeginPosition(searchedString, windowUUID) : getEndPosition(searchedString, windowUUID);
                }
            }
            else if (isBeginIterator(searchedString, greaterOrEqual, windowUUID))
            {
                if (nextLastSeenResult.value().first >= cursorPos)
                {
                    return getBeginPosition(searchedString, windowUUID)->first <= nextLastSeenResult.value().first ? getBeginPosition(searchedString, windowUUID) : getEndPosition(searchedString, windowUUID);
                }
                else
                {
                    return getBeginPosition(searchedString, windowUUID);
                }
            }
            else
            {
                if (nextLastSeenResult.value().first >= cursorPos)
                {
                    return greaterOrEqual->first <= nextLastSeenResult.value().first ? greaterOrEqual : getEndPosition(searchedString, windowUUID);
                }
                else
                {
                    return greaterOrEqual;
                }
            }
        }
        return isEndIterator(searchedString, greaterOrEqual, windowUUID) ? getBeginPosition(searchedString, windowUUID) : greaterOrEqual;
    }

    std::map<pair, size_t> &SearchResultsController::getInitialResults(std::string const &searchedString, utils::datatypes::Uuid const &windowUUID)
    {
        return std::get<INITIAL>(results[windowUUID][searchedString]).first;
    }

    std::map<pair, size_t> &SearchResultsController::getSecondaryResults(std::string const &searchedString, utils::datatypes::Uuid const &windowUUID)
    {
        return std::get<SECONDARY>(results[windowUUID][searchedString]);
    }

    bool SearchResultsController::isBeginIterator(std::string const &searchedString, std::map<pair, size_t>::iterator const &it, utils::datatypes::Uuid const &windowUUID)
    {
        return it == results[windowUUID][searchedString].first.first.begin();
    }

    bool SearchResultsController::isEndIterator(std::string const &searchedString, std::map<pair, size_t>::iterator const &it, utils::datatypes::Uuid const &windowUUID)
    {
        return it == results[windowUUID][searchedString].first.first.end();
    }

    void SearchResultsController::markInitialSearchResult(std::string const &searchedString, utils::datatypes::Uuid const &windowUUID)
    {
        initialSearchExecuted[windowUUID][searchedString] = true;
    }

    void SearchResultsController::markSecondarySearchResult(std::string const &searchedString, utils::datatypes::Uuid const &windowUUID)
    {
        secondarySearchExecuted[windowUUID][searchedString] = true;
    }

    void SearchResultsController::addResult(std::string const &searchedString, std::map<pair, size_t> &&toAdd, utils::datatypes::Uuid const &windowUUID)
    {
        const std::lock_guard<std::mutex> guard(lock);
        getInitialResults(searchedString, windowUUID).merge(toAdd);
    }

    void SearchResultsController::addToSecondaryResults(std::string const &searchedString, std::pair<const pair, size_t> const &toAdd, utils::datatypes::Uuid const &windowUUID)
    {
        const std::lock_guard<std::mutex> guard(lock);
        getInitialResults(searchedString, windowUUID).insert(toAdd);
        getSecondaryResults(searchedString, windowUUID).insert(toAdd);
    }

    void SearchResultsController::clearSecondaryResults(std::string const &searchedString, utils::datatypes::Uuid const &windowUUID, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator)
    {
        for (auto &e : getSecondaryResults(searchedString, windowUUID))
        {
            getInitialResults(searchedString, windowUUID).erase(e.first);
        }
        getSecondaryResults(searchedString, windowUUID).clear();
        secondarySearchExecuted[windowUUID][searchedString] = false;

        std::map<pair, size_t> updatedInitialResults;
        auto currEl = getInitialResults(searchedString, windowUUID).begin();
        while (currEl != getInitialResults(searchedString, windowUUID).end())
        {
            auto el = pair(currEl->first.first, printingOrchestrator->getNumberOfChangesBytesAtPos(currEl->first.first, windowUUID));
            updatedInitialResults.insert({el, currEl->second});
            ++currEl;
        }
        std::swap(results[windowUUID][searchedString].first.first, updatedInitialResults);
        results[windowUUID][searchedString].first.second = std::end(results[windowUUID][searchedString].first.first);
    }

    void SearchResultsController::clearResults(utils::datatypes::Uuid const &windowUUID)
    {
        if (results.count(windowUUID))
        {
            for (auto &e : results[windowUUID])
            {
                results[windowUUID][e.first] = {std::make_pair(std::map<pair, size_t>{}, std::map<pair, size_t>::iterator()), std::map<pair, size_t>()};
                results[windowUUID][e.first].first.second = std::end(results[windowUUID][e.first].first.first);
            }
            initialSearchExecuted.erase(windowUUID);
            secondarySearchExecuted.erase(windowUUID);
        }
    }

    void SearchResultsController::clearAllSearches(std::string const &searchedString, utils::datatypes::Uuid const &windowUUID)
    {
        results[windowUUID].erase(searchedString);
        initialSearchExecuted[windowUUID][searchedString] = secondarySearchExecuted[windowUUID][searchedString] = false;
    }

    std::map<pair, size_t>::iterator SearchResultsController::getCurrPosition(std::string const &searchedString, utils::datatypes::Uuid const &windowUUID)
    {
        return results[windowUUID][searchedString].first.second;
    }

    std::map<pair, size_t>::iterator SearchResultsController::getBeginPosition(std::string const &searchedString, utils::datatypes::Uuid const &windowUUID)
    {
        return std::begin(std::get<INITIAL>(results[windowUUID][searchedString]).first);
    }

    std::map<pair, size_t>::iterator SearchResultsController::getEndPosition(std::string const &searchedString, utils::datatypes::Uuid const &windowUUID)
    {
        return std::end(results[windowUUID][searchedString].first.first);
    }

    std::map<pair, size_t>::iterator SearchResultsController::getPrevPos(std::string const &searchedString, bool early, std::optional<std::pair<pair, size_t>> prevLastSeenResult, std::optional<std::pair<pair, size_t>> nextLastSeenResult, utils::datatypes::Uuid const &windowUUID)
    {
        if (early && utils::FEATHER_MODE::SEARCHING_IN_PROGRESS_DOWN == currentFeatherMode && !getInitialResults(searchedString, windowUUID).empty())
        {
            return std::prev(getEndPosition(searchedString, windowUUID));
        }

        if (early && (getInitialResults(searchedString, windowUUID).size() <= 1 || !prevLastSeenResult.has_value() || !nextLastSeenResult.has_value()))
        {
            return getEndPosition(searchedString, windowUUID);
        }
        if (getInitialResults(searchedString, windowUUID).empty())
        {
            return getEndPosition(searchedString, windowUUID);
        }
        auto prevPos = isBeginIterator(searchedString, results[windowUUID][searchedString].first.second, windowUUID) ? std::prev(std::end(std::get<INITIAL>(results[windowUUID][searchedString]).first)) : std::prev(results[windowUUID][searchedString].first.second);
        if (early)
        {
            // Both last seen results are not wrapped
            if (prevLastSeenResult.value().first <= cursorPos && nextLastSeenResult.value().first >= cursorPos)
            {
                if (isBeginIterator(searchedString, getCurrPosition(searchedString, windowUUID), windowUUID))
                {
                    return getEndPosition(searchedString, windowUUID);
                }
                return prevLastSeenResult.value().first <= prevPos->first ? prevPos : getEndPosition(searchedString, windowUUID);
            }
            // Previous last seen has wrapped
            else if (prevLastSeenResult.value().first > cursorPos && nextLastSeenResult.value().first >= cursorPos)
            {
                // Breaking condition, when jump without wrap
                if (getCurrPosition(searchedString, windowUUID)->first >= prevLastSeenResult.value().first && prevPos->first <= prevLastSeenResult.value().first)
                {
                    return getEndPosition(searchedString, windowUUID);
                }
                // Breaking condition, when jump and wrap
                else if (isBeginIterator(searchedString, getCurrPosition(searchedString, windowUUID), windowUUID) && prevPos->first <= prevLastSeenResult.value().first)
                {
                    return getEndPosition(searchedString, windowUUID);
                }
                else if (isBeginIterator(searchedString, getCurrPosition(searchedString, windowUUID), windowUUID) && getCurrPosition(searchedString, windowUUID)->first <= nextLastSeenResult.value().first && prevPos->first <= nextLastSeenResult.value().first)
                {
                    return getEndPosition(searchedString, windowUUID);
                }
                else if (isBeginIterator(searchedString, getCurrPosition(searchedString, windowUUID), windowUUID) && getCurrPosition(searchedString, windowUUID)->first >= prevLastSeenResult.value().first && prevPos->first >= prevLastSeenResult.value().first)
                {
                    return getEndPosition(searchedString, windowUUID);
                }
                return nextLastSeenResult.value().first < prevPos->first && prevLastSeenResult.value().first > prevPos->first ? getEndPosition(searchedString, windowUUID) : prevPos;
            }
            // Next last seen has wrapped
            else if (prevLastSeenResult.value().first < cursorPos && nextLastSeenResult.value().first <= cursorPos)
            {
                // Breaking condition
                if (getCurrPosition(searchedString, windowUUID)->first >= prevLastSeenResult.value().first && prevPos->first <= prevLastSeenResult.value().first)
                {
                    return getEndPosition(searchedString, windowUUID);
                }
                // Breaking condition, when jump and wrap
                else if (isBeginIterator(searchedString, getCurrPosition(searchedString, windowUUID), windowUUID) && prevPos->first <= prevLastSeenResult.value().first)
                {
                    return getEndPosition(searchedString, windowUUID);
                }
                else if (isBeginIterator(searchedString, getCurrPosition(searchedString, windowUUID), windowUUID) && getCurrPosition(searchedString, windowUUID)->first <= nextLastSeenResult.value().first && prevPos->first <= nextLastSeenResult.value().first)
                {
                    return getEndPosition(searchedString, windowUUID);
                }
                else if (isBeginIterator(searchedString, getCurrPosition(searchedString, windowUUID), windowUUID) && getCurrPosition(searchedString, windowUUID)->first >= prevLastSeenResult.value().first && prevPos->first >= prevLastSeenResult.value().first)
                {
                    return getEndPosition(searchedString, windowUUID);
                }
                return nextLastSeenResult.value().first < prevPos->first && prevLastSeenResult.value().first > prevPos->first ? getEndPosition(searchedString, windowUUID) : prevPos;
            }
        }
        return prevPos;
    }

    std::map<pair, size_t>::iterator SearchResultsController::getNextPos(std::string const &searchedString, bool early, std::optional<std::pair<pair, size_t>> prevLastSeenResult, std::optional<std::pair<pair, size_t>> nextLastSeenResult, utils::datatypes::Uuid const &windowUUID)
    {
        if (early && utils::FEATHER_MODE::SEARCHING_IN_PROGRESS_UP == currentFeatherMode && !getInitialResults(searchedString, windowUUID).empty())
        {
            return getBeginPosition(searchedString, windowUUID);
        }

        if (early && (getInitialResults(searchedString, windowUUID).size() <= 1 || !prevLastSeenResult.has_value() || !nextLastSeenResult.has_value()))
        {
            return getEndPosition(searchedString, windowUUID);
        }

        if (getInitialResults(searchedString, windowUUID).empty())
        {
            return getEndPosition(searchedString, windowUUID);
        }

        auto nextPos = std::next(results[windowUUID][searchedString].first.second);
        auto nextNextPos = isEndIterator(searchedString, nextPos, windowUUID) ? getBeginPosition(searchedString, windowUUID) : nextPos;
        if (early)
        {
            // Both last seen results are not wrapped
            if (prevLastSeenResult.value().first <= cursorPos && nextLastSeenResult.value().first >= cursorPos)
            {
                if (isEndIterator(searchedString, nextPos, windowUUID))
                {
                    return getEndPosition(searchedString, windowUUID);
                }
                return nextLastSeenResult.value().first >= nextPos->first ? nextPos : getEndPosition(searchedString, windowUUID);
            }
            // Previous last seen has wrapped
            else if (prevLastSeenResult.value().first > cursorPos && nextLastSeenResult.value().first >= cursorPos)
            {
                // Breaking condition, when jump without wrap
                if (getCurrPosition(searchedString, windowUUID)->first <= nextLastSeenResult.value().first && nextPos->first >= nextLastSeenResult.value().first)
                {
                    return getEndPosition(searchedString, windowUUID);
                }
                else if (isEndIterator(searchedString, nextPos, windowUUID) && nextNextPos->first >= nextLastSeenResult.value().first)
                {
                    return getEndPosition(searchedString, windowUUID);
                }
                else if (isEndIterator(searchedString, nextPos, windowUUID) && getCurrPosition(searchedString, windowUUID)->first <= nextLastSeenResult.value().first && nextNextPos->first <= nextLastSeenResult.value().first)
                {
                    return getEndPosition(searchedString, windowUUID);
                }
                else if (isEndIterator(searchedString, nextPos, windowUUID) && getCurrPosition(searchedString, windowUUID)->first >= prevLastSeenResult.value().first && nextNextPos->first >= prevLastSeenResult.value().first)
                {
                    return getEndPosition(searchedString, windowUUID);
                }
                return nextLastSeenResult.value().first < nextNextPos->first && prevLastSeenResult.value().first > nextNextPos->first ? getEndPosition(searchedString, windowUUID) : nextNextPos;
            }
            // Next last seen has wrapped
            else if (prevLastSeenResult.value().first < cursorPos && nextLastSeenResult.value().first <= cursorPos)
            {
                // Breaking condition
                if (getCurrPosition(searchedString, windowUUID)->first <= nextLastSeenResult.value().first && nextNextPos->first >= prevLastSeenResult.value().first)
                {
                    return getEndPosition(searchedString, windowUUID);
                }
                // Breaking condition, when jump and wrap
                else if (isEndIterator(searchedString, nextPos, windowUUID) && nextNextPos->first >= nextLastSeenResult.value().first)
                {
                    return getEndPosition(searchedString, windowUUID);
                }
                else if (isEndIterator(searchedString, nextPos, windowUUID) && getCurrPosition(searchedString, windowUUID)->first <= nextLastSeenResult.value().first && nextNextPos->first <= nextLastSeenResult.value().first)
                {
                    return getEndPosition(searchedString, windowUUID);
                }
                else if (isEndIterator(searchedString, nextPos, windowUUID) && getCurrPosition(searchedString, windowUUID)->first >= prevLastSeenResult.value().first && nextNextPos->first >= prevLastSeenResult.value().first)
                {
                    return getEndPosition(searchedString, windowUUID);
                }
                return nextLastSeenResult.value().first < nextNextPos->first && prevLastSeenResult.value().first > nextNextPos->first ? getEndPosition(searchedString, windowUUID) : nextNextPos;
            }
        }
        return nextNextPos;
    }

    bool SearchResultsController::anyStringCharacterModified(utils::datatypes::Uuid const &windowUUID, std::string const &searchedString, size_t realPos, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator, std::shared_ptr<utils::storage::AbstractStorage> storage)
    {
        for (size_t idx = 0; idx < searchedString.size(); ++idx)
        {
            if (printingOrchestrator->isCharAtPosDeleted(pair(realPos, 0), windowUUID))
            {
                return true;
            }
            else if (printingOrchestrator->isCharAtPosWithChanges(realPos, windowUUID))
            {
                return true;
            }
            realPos += utils::Utf8Util::getCharacterBytesTakenLight(storage->getCharacterAtPos(realPos));
        }
        return false;
    }

    std::vector<std::string> SearchResultsController::getAllSearchedStrings(utils::datatypes::Uuid const &windowUUID)
    {
        if (!results.count(windowUUID))
        {
            return {};
        }

        std::vector<std::string> searchedStrings;
        std::for_each(results[windowUUID].begin(), results[windowUUID].end(), [&](auto &e)
                      { searchedStrings.push_back(e.first); });
        return searchedStrings;
    }

    bool SearchResultsController::isSearchCapped(std::string const &searchedString, utils::datatypes::Uuid const &windowUUID)
    {
        if (!results.count(windowUUID))
        {
            return false;
        }
        return getInitialResults(searchedString, windowUUID).size() > RESULT_CAP_THRESHOLD;
    }

    void SearchResultsController::resetLastSearchTimestamp()
    {
        lastSearchTimestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
    }

    size_t SearchResultsController::getCapSize()
    {
        return RESULT_CAP_THRESHOLD;
    }

    bool SearchResultsController::isCurrPositionNotSet(std::string const &searchedString, utils::datatypes::Uuid const &windowUUID)
    {
        return results[windowUUID][searchedString].first.second == std::end(results[windowUUID][searchedString].first.first);
    }

    bool SearchResultsController::isSearchAlreadyPerformed(std::string const &searchedString, utils::datatypes::Uuid const &windowUUID) const
    {
        return initialSearchExecuted.count(windowUUID) && initialSearchExecuted[windowUUID].count(searchedString) && initialSearchExecuted[windowUUID][searchedString];
    }

    bool SearchResultsController::isSecondarySearchAlreadyPerformed(std::string const &searchedString, utils::datatypes::Uuid const &windowUUID) const
    {
        return secondarySearchExecuted.count(windowUUID) && secondarySearchExecuted[windowUUID].count(searchedString) && secondarySearchExecuted[windowUUID][searchedString];
    }

    std::map<pair, size_t>::iterator SearchResultsController::updateCurrPosition(std::string const &searchedString, std::map<pair, size_t>::iterator const &it, utils::datatypes::Uuid const &windowUUID)
    {
        return results[windowUUID][searchedString].first.second = it;
    }

    std::optional<std::pair<pair, size_t>> SearchResultsController::getPrevSearchResult(std::string const &searchedString, utils::datatypes::Uuid const &windowUUID, bool early, std::optional<std::pair<pair, size_t>> lastPrevSeenResult, std::optional<std::pair<pair, size_t>> lastNextSeenResult)
    {
        const std::lock_guard<std::mutex> guard(lock);
        if (getInitialResults(searchedString, windowUUID).empty())
        {
            return std::nullopt;
        }

        std::map<pair, size_t>::iterator lastSeenModified = std::end(getInitialResults(searchedString, windowUUID));
        while (true)
        {
            auto posBefore = isCurrPositionNotSet(searchedString, windowUUID) ? getFirstPosBefore(searchedString, cursorPos, early, lastPrevSeenResult, windowUUID) : getPrevPos(searchedString, early, lastPrevSeenResult, lastNextSeenResult, windowUUID);
            if (isEndIterator(searchedString, posBefore, windowUUID))
            {
                return std::nullopt;
            }

            if (printingOrchestrator->getLastValidPosition(windowUUID, storage) < posBefore->first)
            {
                getInitialResults(searchedString, windowUUID).erase(posBefore);
                if (getInitialResults(searchedString, windowUUID).empty())
                {
                    return std::nullopt;
                }
                continue;
            }
            updateCurrPosition(searchedString, posBefore, windowUUID);
            if (!getSecondaryResults(searchedString, windowUUID).count(posBefore->first) && anyStringCharacterModified(windowUUID, searchedString, posBefore->first.first, printingOrchestrator, storage))
            {
                if (isEndIterator(searchedString, lastSeenModified, windowUUID))
                {
                    lastSeenModified = posBefore;
                }
                else if (posBefore == lastSeenModified)
                {
                    return std::nullopt;
                }
                continue;
            }
            return *posBefore;
        }
    }

    std::optional<std::pair<pair, size_t>> SearchResultsController::getNextSearchResult(std::string const &searchedString, utils::datatypes::Uuid const &windowUUID, bool early, std::optional<std::pair<pair, size_t>> lastPrevSeenResult, std::optional<std::pair<pair, size_t>> lastNextSeenResult)
    {
        const std::lock_guard<std::mutex> guard(lock);
        if (getInitialResults(searchedString, windowUUID).empty())
        {
            return std::nullopt;
        }

        std::map<pair, size_t>::iterator lastSeenModified = std::end(getInitialResults(searchedString, windowUUID));
        while (true)
        {
            auto posAfter = isCurrPositionNotSet(searchedString, windowUUID) ? getFirstPosAfter(searchedString, cursorPos, early, lastNextSeenResult, windowUUID) : getNextPos(searchedString, early, lastPrevSeenResult, lastNextSeenResult, windowUUID);
            if (isEndIterator(searchedString, posAfter, windowUUID))
            {
                return std::nullopt;
            }

            if (printingOrchestrator->getLastValidPosition(windowUUID, storage) < posAfter->first)
            {
                getInitialResults(searchedString, windowUUID).erase(posAfter);
                if (getInitialResults(searchedString, windowUUID).empty())
                {
                    return std::nullopt;
                }
                continue;
            }
            updateCurrPosition(searchedString, posAfter, windowUUID);
            if (!getSecondaryResults(searchedString, windowUUID).count(posAfter->first) && anyStringCharacterModified(windowUUID, searchedString, posAfter->first.first, printingOrchestrator, storage))
            {
                if (isEndIterator(searchedString, lastSeenModified, windowUUID))
                {
                    lastSeenModified = posAfter;
                }
                else if (posAfter == lastSeenModified)
                {
                    return std::nullopt;
                }
                continue;
            }
            return *posAfter;
        }
    }

    std::chrono::nanoseconds SearchResultsController::getLastSearchTimestamp() const
    {
        return lastSearchTimestamp;
    }

    void SearchResultsController::normalizeResultsAfterSave(utils::datatypes::Uuid const &windowUUID, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator, std::shared_ptr<utils::storage::AbstractStorage> storage)
    {
        if (!results.count(windowUUID))
        {
            return;
        }

        for (auto &r : results[windowUUID])
        {
            std::map<pair, size_t> normalizedResults;
            auto &localResults = std::get<INITIAL>(r.second).first;
            auto currLocalResult = localResults.begin();
            while (currLocalResult != localResults.end())
            {
                auto curr = currLocalResult++;
                if ((curr->first > printingOrchestrator->getLastValidPosition(windowUUID, storage)) || (!std::get<SECONDARY>(r.second).count(curr->first) && anyStringCharacterModified(windowUUID, r.first, curr->first.first, printingOrchestrator, storage)))
                {
                    continue;
                }
                normalizedResults.insert({pair(printingOrchestrator->convertVirtualPositionToByteOffset(curr->first, windowUUID), 0), curr->second});
                localResults.erase(curr);
            }
            clearSecondaryResults(r.first, windowUUID, printingOrchestrator);
            std::swap(std::get<INITIAL>(r.second).first, normalizedResults);
            r.second.first.second = std::end(r.second.first.first);
            resetLastSearchTimestamp();
        }
    }

    SearchResultsController::~SearchResultsController()
    {
    }

    std::unordered_map<utils::datatypes::Uuid, std::unordered_map<std::string, std::pair<std::pair<std::map<pair, size_t>, std::map<pair, size_t>::iterator>, std::map<pair, size_t>>>, utils::datatypes::UuidHasher> SearchResultsController::results;
    std::unordered_map<utils::datatypes::Uuid, std::unordered_map<std::string, bool>, utils::datatypes::UuidHasher> SearchResultsController::initialSearchExecuted;
    std::unordered_map<utils::datatypes::Uuid, std::unordered_map<std::string, bool>, utils::datatypes::UuidHasher> SearchResultsController::secondarySearchExecuted;
    std::chrono::nanoseconds SearchResultsController::lastSearchTimestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
    std::mutex SearchResultsController::lock;
}