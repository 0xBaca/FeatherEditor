#pragma once

#ifdef _FEATHER_TEST_
#include "googletest/include/gtest/gtest_prod.h"
#endif

#include "utils/algorithm/SearchResultsController.hpp"
#include "utils/algorithm/SearchEngineBase.hpp"
#include "utils/algorithm/SearchZooKeeper.hpp"

namespace feather::utils::algorithm
{
    class FastSearch : public SearchEngineBase
    {
#ifdef _FEATHER_TEST_
        FRIEND_TEST(FastSearchTest, getNextSearchIntervalNoChanges);
        FRIEND_TEST(FastSearchIntegTest, getFirstSearchIntervalEmptyStorage);
        FRIEND_TEST(FastSearchIntegTest, getFirstSearchInterval_2);
        FRIEND_TEST(FastSearchIntegTest, noMoreNewChangesIntervals);
        FRIEND_TEST(FastSearchIntegTest, nextChangeAtPosGreaterThanSearchedWordSize);
        FRIEND_TEST(FastSearchIntegTest, nextChangeWhenInTheMiddleOfChanges);
        FRIEND_TEST(FastSearchIntegTest, searchBetweenNewChanges);
        FRIEND_TEST(FastSearchIntegTest, searchBetweenNewChanges_2);
        FRIEND_TEST(FastSearchIntegTest, searchBetweenNewChanges_3);
        FRIEND_TEST(FastSearchIntegTest, searchBetweenNewChanges_4);
        FRIEND_TEST(FastSearchIntegTest, getNextSearchResult);
        FRIEND_TEST(FastSearchIntegTest, getNextAndPreviousSearchResult);
        FRIEND_TEST(FastSearchIntegTest, getNextAndPreviousSearchResultWithOneResult);
        FRIEND_TEST(FastSearchIntegTest, getNextAndThenPreviousFromSearchOnly);
        FRIEND_TEST(FastSearchIntegTest, getPreviousSecondarySearchResult);
        FRIEND_TEST(FastSearchIntegTest, getNextMatch);
        FRIEND_TEST(FastSearchIntegTest, resetResultsIfFileWasModified);
#endif
    private:
        std::string flatteredSearchedString;
        bool escReceived, signalReceived;
        bool firstSearch;
        bool secondarySearchFinished, initialSearchToFinished, initialSearchFromFinished;
        bool isHexSearch;
        std::optional<std::pair<pair, size_t>> lastInitialToResultPositionVisited, lastInitialFromResultPositionVisited, lastSecondaryResultPositionVisited;
        std::pair<pair, size_t> lastCursorPos;
        pair cursorPos;
        std::basic_string<char> toCursorSearchBuffer, fromCursorSearchBuffer;
        std::unique_ptr<SearchResultsController> resultsController;
        SearchZooKeeper zooKeeper;
        void initialFromCursorSearchAsync(size_t, size_t &, bool &);
        void initialToCursorSearchAsync(size_t, size_t &, bool &);
        void checkIfEarlyResultAvailable(std::shared_ptr<feather::windows::WindowsManager> &, std::unique_ptr<printer::PrintingVisitorInterface> &, std::unique_ptr<printer::PrintingVisitorInterface> &, std::unique_ptr<feather::windows::SubWindowInterface> &, utils::FEATHER_MODE &, size_t, utils::Direction &, bool &, bool);
        void removePreviousSecondaryChangesResults();
        void searchBetweenNewChanges();

    public:
        FastSearch(utils::datatypes::Uuid const &, std::u32string &&, std::shared_ptr<utils::storage::AbstractStorageFactory>, std::shared_ptr<utils::storage::AbstractStorageFactory>, std::shared_ptr<printer::PrintingOrchestratorInterface>, std::unique_ptr<utils::BufferFillerInterface> const &, pair, std::list<pair> &, std::list<pair>::iterator &, std::set<pair> &, bool = false);
        std::pair<std::optional<std::pair<pair, size_t>>, utils::datatypes::ERROR> getNextMatch(std::shared_ptr<feather::windows::WindowsManager>, std::unique_ptr<printer::PrintingVisitorInterface> &, std::unique_ptr<printer::PrintingVisitorInterface> &, std::unique_ptr<feather::windows::SubWindowInterface> &, utils::FEATHER_MODE &currentFeatherMode, utils::Direction) override;
        ~FastSearch();
    };
} // namespace feather::utils::algorithm