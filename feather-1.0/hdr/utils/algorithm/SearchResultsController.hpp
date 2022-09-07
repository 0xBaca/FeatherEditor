#pragma once

#ifdef _FEATHER_TEST_
#include "googletest/include/gtest/gtest_prod.h"
#endif

#include "printer/PrintingOrchestratorInterface.hpp"
#include "utils/datatypes/Global.hpp"
#include "utils/datatypes/UuidHash.hpp"

#include <set>
#include <unordered_set>
#include <unordered_map>

namespace feather::utils::algorithm
{
    enum RESULT_SCOPE
    {
        INITIAL = 0,
        SECONDARY = 1
    };

    class SearchResultsController
    {
#ifdef _FEATHER_TEST_
        FRIEND_TEST(SearchResultsControllerTest, addSearchResult);
        FRIEND_TEST(SearchResultsControllerTest, getFirstPosAfter);
        FRIEND_TEST(SearchResultsControllerTest, getFirstPosAfter_CursorBeforeAllResults);
        FRIEND_TEST(SearchResultsControllerTest, getFirstPosAfter_CursorAfterAllResultsButSearchAlreadyWrapped);
        FRIEND_TEST(SearchResultsControllerTest, getFirstPosAfter_CursorAfterAllResultsSearchDidntWrapped);
        FRIEND_TEST(SearchResultsControllerTest, getFirstPosBefore_CursorAfterAllResults);
        FRIEND_TEST(SearchResultsControllerTest, getFirstPosBefore_CursorAfterAllResultsSearchDidntWrapped);
        FRIEND_TEST(SearchResultsControllerTest, getFirstPosBefore_CursorAfterAllResultsSearchWrapped);
        FRIEND_TEST(SearchResultsControllerTest, getFirstPosBefore);
        FRIEND_TEST(SearchResultsControllerTest, moveCurrResultToPrevPos);
        FRIEND_TEST(SearchResultsControllerTest, moveCurrResultToNextPos);
        FRIEND_TEST(SearchResultsControllerTest, getPrevSearchResult);
        FRIEND_TEST(SearchResultsControllerTest, getPrevSearchResult_2);
        FRIEND_TEST(SearchResultsControllerTest, getNextSearchResult);
        FRIEND_TEST(SearchResultsControllerTest, getPrevSearchResultOneResultNoSecondarySearchResults);
        FRIEND_TEST(SearchResultsControllerTest, getPrevSearchResultOneToResultBeforeOneSecondarySearchResult);
        FRIEND_TEST(SearchResultsControllerTest, getNextSearchResultOneToResultBeforeOneSecondarySearchResult);
        FRIEND_TEST(SearchResultsControllerTest, getNextSearchResultOneToResultBeforeOneSecondarySearchResult_2);
        FRIEND_TEST(SearchResultsControllerTest, getNextSearchResultOneToResultBeforeOneSecondarySearchResult_3);
        FRIEND_TEST(SearchResultsControllerTest, getPrevSearchResultOneToResultAfterOneSecondarySearchResult);
        FRIEND_TEST(SearchResultsControllerTest, getNextSearchResultOneToResultAfterOneSecondarySearchResult);
        FRIEND_TEST(SearchResultsControllerTest, getNextSearchResultOneResultNoSecondarySearchResults);
        FRIEND_TEST(SearchResultsControllerTest, getPrevSearchResultOneFromResultBeforeOneSecondarySearchResult);
        FRIEND_TEST(SearchResultsControllerTest, getPrevSearchResultMultipleResultsNoSecondarySearchResult);
        FRIEND_TEST(SearchResultsControllerTest, getNextSearchResultMultipleResultsNoSecondarySearchResult);
        FRIEND_TEST(SearchResultsControllerTest, getPrevNextSearchResult);
#endif
    private:
        /*  1) Search esults
            2.a curr element
            2.b first element up search
            2.c last element down seearch
        3) Secondary search results
            1) Element not read yet or end() if nothing
        */
        static constexpr size_t RESULT_CAP_THRESHOLD = 5000UL;
        static std::unordered_map<utils::datatypes::Uuid, std::unordered_map<std::string, std::pair<std::pair<std::map<pair, size_t>, std::map<pair, size_t>::iterator>, std::map<pair, size_t>>>, utils::datatypes::UuidHasher> results;
        static std::chrono::nanoseconds lastSearchTimestamp;
        static std::unordered_map<utils::datatypes::Uuid, std::unordered_map<std::string, bool>, utils::datatypes::UuidHasher> initialSearchExecuted;
        static std::unordered_map<utils::datatypes::Uuid, std::unordered_map<std::string, bool>, utils::datatypes::UuidHasher> secondarySearchExecuted;
        static std::mutex lock;
        pair cursorPos;
        std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator;
        std::shared_ptr<utils::storage::AbstractStorage> storage;

        static std::map<pair, size_t> &getInitialResults(std::string const &, utils::datatypes::Uuid const &);
        static std::map<pair, size_t> &getSecondaryResults(std::string const &, utils::datatypes::Uuid const &);
        static bool anyStringCharacterModified(utils::datatypes::Uuid const &, std::string const &, size_t, std::shared_ptr<printer::PrintingOrchestratorInterface>, std::shared_ptr<utils::storage::AbstractStorage>);
        std::map<pair, size_t>::iterator getFirstPosBefore(std::string const &, pair, bool, std::optional<std::pair<pair, size_t>>, utils::datatypes::Uuid const &);
        std::map<pair, size_t>::iterator getFirstPosAfter(std::string const &, pair, bool, std::optional<std::pair<pair, size_t>>, utils::datatypes::Uuid const &);
        std::map<pair, size_t>::iterator getCurrPosition(std::string const &, utils::datatypes::Uuid const &);
        std::map<pair, size_t>::iterator getBeginPosition(std::string const &, utils::datatypes::Uuid const &);
        std::map<pair, size_t>::iterator getEndPosition(std::string const &, utils::datatypes::Uuid const &);
        std::map<pair, size_t>::iterator getPrevPos(std::string const &, bool, std::optional<std::pair<pair, size_t>>, std::optional<std::pair<pair, size_t>>, utils::datatypes::Uuid const &);
        std::map<pair, size_t>::iterator getNextPos(std::string const &, bool, std::optional<std::pair<pair, size_t>>, std::optional<std::pair<pair, size_t>>, utils::datatypes::Uuid const &);
        std::map<pair, size_t>::iterator updateCurrPosition(std::string const &, std::map<pair, size_t>::iterator const &, utils::datatypes::Uuid const &);
        bool isCurrPositionNotSet(std::string const &, utils::datatypes::Uuid const &);
        bool isBeginIterator(std::string const &, std::map<pair, size_t>::iterator const &, utils::datatypes::Uuid const &);
        bool isEndIterator(std::string const &, std::map<pair, size_t>::iterator const &, utils::datatypes::Uuid const &);

    public:
        static void addToSecondaryResults(std::string const &, std::pair<const pair, size_t> const &, utils::datatypes::Uuid const &);
        static void normalizeResultsAfterSave(utils::datatypes::Uuid const &, std::shared_ptr<printer::PrintingOrchestratorInterface>, std::shared_ptr<utils::storage::AbstractStorage>);
        static void clearSecondaryResults(std::string const &, utils::datatypes::Uuid const &, std::shared_ptr<printer::PrintingOrchestratorInterface>);
        static void clearResults(utils::datatypes::Uuid const &);
        static std::vector<std::string> getAllSearchedStrings(utils::datatypes::Uuid const &);
        static bool isSearchCapped(std::string const &, utils::datatypes::Uuid const &);
        static void resetLastSearchTimestamp();
        static size_t getCapSize();
        SearchResultsController(std::string const &, std::shared_ptr<utils::storage::AbstractStorage>, std::shared_ptr<printer::PrintingOrchestratorInterface>, utils::datatypes::Uuid const &, pair);
        void addResult(std::string const &, std::map<pair, size_t> &&, utils::datatypes::Uuid const &);
        void clearAllSearches(std::string const &, utils::datatypes::Uuid const &);
        void markInitialSearchResult(std::string const &, utils::datatypes::Uuid const &);
        void markSecondarySearchResult(std::string const &, utils::datatypes::Uuid const &);
        bool isSearchAlreadyPerformed(std::string const &, utils::datatypes::Uuid const &) const;
        bool isSecondarySearchAlreadyPerformed(std::string const &, utils::datatypes::Uuid const &) const;
        std::optional<std::pair<pair, size_t>> getPrevSearchResult(std::string const &, utils::datatypes::Uuid const &, bool = false, std::optional<std::pair<pair, size_t>> = std::nullopt, std::optional<std::pair<pair, size_t>> = std::nullopt);
        std::optional<std::pair<pair, size_t>> getNextSearchResult(std::string const &, utils::datatypes::Uuid const &, bool = false, std::optional<std::pair<pair, size_t>> = std::nullopt, std::optional<std::pair<pair, size_t>> = std::nullopt);
        std::chrono::nanoseconds getLastSearchTimestamp() const;
        ~SearchResultsController();
    };
}