#include "helpers/TestBase.hpp"
#include "mocks/PrintingOrchestratorMock.hpp"
#include "mocks/StorageMock.hpp"
#include "utils/ProgramOptionsParser.hpp"
#include "utils/algorithm/SearchResultsController.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace feather::utils::algorithm
{
    class SearchResultsControllerTest : public ::testing::Test, public ::feather::test::TestBase
    {
    protected:
        std::string searchedString;
        std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestratorMock;
        std::shared_ptr<feather::utils::storage::AbstractStorage> storageMock;

    public:
        void SetUp() override
        {
            const char *commandLineArguments[] = {"./feather", "file", "--relax=5", "--mode=text"};
            utils::ProgramOptionsParser::parseArguments(4UL, commandLineArguments);
            configuration->overrideConfiguration();
            searchedString = "test";
            printingOrchestratorMock = std::make_shared<testing::mocks::PrintingOrchestratorMock>();
            storageMock = std::make_shared<testing::mocks::StorageMock>();
        }

        void TearDown() override
        {
        }
    };

    /* NO SECONDARY RESULTS */
    /*
    TEST_F(SearchResultsControllerTest, toThreadDidntStarted_early)
    {
        //before
        auto searchResultController = std::make_unique<SearchResultsController>(searchedString, storageMock, printingOrchestratorMock, TEST_UUID, pair(0, 0));

        //when
        auto prevResult = searchResultController->getPrevSearchResult(searchedString, TEST_UUID, true, std::nullopt);

        //then
        ASSERT_EQ(prevResult, std::nullopt);
    }

    TEST_F(SearchResultsControllerTest, fromThreadDidntStarted_early)
    {
        //before
        auto searchResultController = std::make_unique<SearchResultsController>(searchedString, storageMock, printingOrchestratorMock, TEST_UUID, pair(0, 0));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosDeleted(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosWithChanges(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::StorageMock *>(storageMock.get()), getCharacterAtPos(::testing::_)).WillRepeatedly(::testing::Return('a'));

        //when
        auto nextResult = searchResultController->getNextSearchResult(searchedString, TEST_UUID, true, std::nullopt);

        //then
        ASSERT_EQ(nextResult, std::nullopt);
    }

    TEST_F(SearchResultsControllerTest, toThreadStartedNoResults_early)
    {
        //before
        auto searchResultController = std::make_unique<SearchResultsController>(searchedString, storageMock, printingOrchestratorMock, TEST_UUID, pair(0, 0));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosDeleted(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosWithChanges(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::StorageMock *>(storageMock.get()), getCharacterAtPos(::testing::_)).WillRepeatedly(::testing::Return('a'));

        //when
        auto prevResult = searchResultController->getPrevSearchResult(searchedString, TEST_UUID, true, std::make_pair(pair(0, 0), 0));

        //then
        ASSERT_EQ(prevResult, std::nullopt);
    }

    TEST_F(SearchResultsControllerTest, fromThreadStartedNoResults_early)
    {
        //before
        auto searchResultController = std::make_unique<SearchResultsController>(searchedString, storageMock, printingOrchestratorMock, TEST_UUID, pair(0, 0));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosDeleted(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosWithChanges(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::StorageMock *>(storageMock.get()), getCharacterAtPos(::testing::_)).WillRepeatedly(::testing::Return('a'));

        //when
        auto nextResult = searchResultController->getNextSearchResult(searchedString, TEST_UUID, true, std::make_pair(pair(0, 0), 0);

        //then
        ASSERT_EQ(nextResult, std::nullopt);
    }

    TEST_F(SearchResultsControllerTest, toThreadStartedOneResultBeforeWrap_early)
    {
        //before
        auto searchResultController = std::make_unique<SearchResultsController>(searchedString, storageMock, printingOrchestratorMock, TEST_UUID, pair(2, 0));
        searchResultController->addResult(searchedString, pair(1, 0), TEST_UUID);
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosDeleted(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosWithChanges(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::StorageMock *>(storageMock.get()), getCharacterAtPos(::testing::_)).WillRepeatedly(::testing::Return('a'));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getLastValidPosition(TEST_UUID, storageMock))
            .WillOnce(::testing::Return(pair(16, 0)));

        //when
        auto prevResult = searchResultController->getPrevSearchResult(searchedString, TEST_UUID, true, std::make_pair(pair(0, 0), 0);

        //then
        ASSERT_EQ(prevResult, pair(1, 0));
    }

    TEST_F(SearchResultsControllerTest, fromThreadStartedOneResultBeforeWrap_early)
    {
        //before
        auto searchResultController = std::make_unique<SearchResultsController>(searchedString, storageMock, printingOrchestratorMock, TEST_UUID, pair(1, 0));
        searchResultController->addResult(searchedString, pair(1, 0), TEST_UUID);
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosDeleted(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosWithChanges(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::StorageMock *>(storageMock.get()), getCharacterAtPos(::testing::_)).WillRepeatedly(::testing::Return('a'));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getLastValidPosition(TEST_UUID, storageMock))
            .WillOnce(::testing::Return(pair(16, 0)));

        //when
        auto nextResult = searchResultController->getNextSearchResult(searchedString, TEST_UUID, true, std::make_pair(pair(1, 0), 0), std::make_pair(pair(1, 0), 0);

        //then
        ASSERT_EQ(nextResult, pair(1, 0));
    }

    TEST_F(SearchResultsControllerTest, toThreadStartedOneResultAfterWrap_early)
    {
        //before
        auto searchResultController = std::make_unique<SearchResultsController>(searchedString, storageMock, printingOrchestratorMock, TEST_UUID, pair(1, 0));
        searchResultController->addResult(searchedString, pair(99, 0), TEST_UUID);
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosDeleted(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosWithChanges(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::StorageMock *>(storageMock.get()), getCharacterAtPos(::testing::_)).WillRepeatedly(::testing::Return('a'));

        //when
        auto prevResult = searchResultController->getPrevSearchResult(searchedString, TEST_UUID, true, std::make_pair(pair(0, 0), 0));

        //then
        ASSERT_EQ(prevResult, std::nullopt);
    }

    TEST_F(SearchResultsControllerTest, toThreadStartedOneResultAfterWrap_early_2)
    {
        //before
        auto searchResultController = std::make_unique<SearchResultsController>(searchedString, storageMock, printingOrchestratorMock, TEST_UUID, pair(1, 0));
        searchResultController->addResult(searchedString, pair(99, 0), TEST_UUID);
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosDeleted(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosWithChanges(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::StorageMock *>(storageMock.get()), getCharacterAtPos(::testing::_)).WillRepeatedly(::testing::Return('a'));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getLastValidPosition(TEST_UUID, storageMock))
            .WillOnce(::testing::Return(pair(100, 0)));

        //when
        auto prevResult = searchResultController->getPrevSearchResult(searchedString, TEST_UUID, true, std::make_pair(pair(98, 0), 0), std::make_pair(pair(50, 0), 0));

        //then
        ASSERT_EQ(prevResult, pair(99, 0));
    }

    TEST_F(SearchResultsControllerTest, fromThreadStartedOneResultAfterWrap_early)
    {
        //before
        std::string searchedString("fromThreadStartedOneResultAfterWrap_early");
        auto searchResultController = std::make_unique<SearchResultsController>(searchedString, storageMock, printingOrchestratorMock, TEST_UUID, pair(2, 0));
        searchResultController->addResult(searchedString, pair(0, 0), TEST_UUID);
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosDeleted(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosWithChanges(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::StorageMock *>(storageMock.get()), getCharacterAtPos(::testing::_)).WillRepeatedly(::testing::Return('a'));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getLastValidPosition(TEST_UUID, storageMock))
            .WillOnce(::testing::Return(pair(100, 0)));

        //when
        auto nextResult = searchResultController->getNextSearchResult(searchedString, TEST_UUID, true, std::make_pair(pair(1, 0), std::make_pair(pair(1, 0), 0));

        //then
        ASSERT_EQ(nextResult, pair(0, 0));
    }

    TEST_F(SearchResultsControllerTest, toThreadStartedOneResultBeforeWrapButLesserThanCurrResult_early)
    {
        //before
        std::string searchedString("toThreadStartedOneResultBeforeWrapButLesserThanCurrResult_early");
        auto searchResultController = std::make_unique<SearchResultsController>(searchedString, storageMock, printingOrchestratorMock, TEST_UUID, pair(3, 0));
        searchResultController->addResult(searchedString, pair(0, 0), TEST_UUID);
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosDeleted(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosWithChanges(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::StorageMock *>(storageMock.get()), getCharacterAtPos(::testing::_)).WillRepeatedly(::testing::Return('a'));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getLastValidPosition(TEST_UUID, storageMock))
            .WillOnce(::testing::Return(pair(100, 0)));

        //when
        auto prevResult = searchResultController->getPrevSearchResult(searchedString, TEST_UUID, true, std::make_pair(pair(0, 0), 0), std::make_pair(pair(3, 0), 0));

        //then
        ASSERT_EQ(prevResult, pair(0, 0));
    }

    TEST_F(SearchResultsControllerTest, fromThreadStartedOneResultBeforeWrapButGreaterThanCurrResult_early)
    {
        //before
        auto searchResultController = std::make_unique<SearchResultsController>(searchedString, storageMock, printingOrchestratorMock, TEST_UUID, pair(3, 0));
        searchResultController->addResult(searchedString, pair(10, 0), TEST_UUID);
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosDeleted(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosWithChanges(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::StorageMock *>(storageMock.get()), getCharacterAtPos(::testing::_)).WillRepeatedly(::testing::Return('a'));

        //when
        auto prevResult = searchResultController->getNextSearchResult(searchedString, TEST_UUID, true, std::make_pair(pair(5, 0), 0));

        //then
        ASSERT_EQ(prevResult, std::nullopt);
    }

    TEST_F(SearchResultsControllerTest, toThreadStartedTwoResultsBeforeAfterWrapButAfterFromFromThread_early)
    {
        //before
        std::string searchedString("toThreadStartedTwoResultsBeforeAfterWrapButAfterFromFromThread_early");
        auto searchResultController = std::make_unique<SearchResultsController>(searchedString, storageMock, printingOrchestratorMock, TEST_UUID, pair(3, 0));
        searchResultController->addResult(searchedString, pair(0, 0), TEST_UUID);
        searchResultController->addResult(searchedString, pair(20, 0), TEST_UUID);
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosDeleted(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosWithChanges(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::StorageMock *>(storageMock.get()), getCharacterAtPos(::testing::_)).WillRepeatedly(::testing::Return('a'));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getLastValidPosition(TEST_UUID, storageMock))
            .WillOnce(::testing::Return(pair(100, 0)));

        //when
        auto prevResult = searchResultController->getPrevSearchResult(searchedString, TEST_UUID, true, std::make_pair(pair(19, 0), 0));

        //then
        ASSERT_EQ(prevResult, pair(0, 0));

        //when
        prevResult = searchResultController->getPrevSearchResult(searchedString, TEST_UUID, true, std::make_pair(pair(19, 0), 0));

        //then
        ASSERT_EQ(prevResult, std::nullopt);
    }

    TEST_F(SearchResultsControllerTest, toThreadStartedNoResults_withSecondaryResultBeforeWrap_early)
    {
        //before
        std::string searchedString("toThreadStartedNoResults_withSecondaryResultBeforeWrap_early");
        auto searchResultController = std::make_unique<SearchResultsController>(searchedString, storageMock, printingOrchestratorMock, TEST_UUID, pair(1, 1));
        searchResultController->addToSecondaryResults(searchedString, pair(0, 0), TEST_UUID);
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosDeleted(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosWithChanges(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::StorageMock *>(storageMock.get()), getCharacterAtPos(::testing::_)).WillRepeatedly(::testing::Return('a'));

        //when
        auto prevResult = searchResultController->getPrevSearchResult(searchedString, TEST_UUID, true, pair(1, 0), 0));

        //then
        ASSERT_EQ(prevResult, std::nullopt);
    }

    TEST_F(SearchResultsControllerTest, fromThreadStartedOneResultBeforeWrap_withSecondaryResult_early)
    {
        //before
        std::string searchedString("fromThreadStartedOneResultBeforeWrap_withSecondaryResult_early");
        auto searchResultController = std::make_unique<SearchResultsController>(searchedString, storageMock, printingOrchestratorMock, TEST_UUID, pair(1, 0));
        searchResultController->addToSecondaryResults(searchedString, pair(1, 3), TEST_UUID);
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosDeleted(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosWithChanges(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::StorageMock *>(storageMock.get()), getCharacterAtPos(::testing::_)).WillRepeatedly(::testing::Return('a'));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getLastValidPosition(TEST_UUID, storageMock))
            .WillOnce(::testing::Return(pair(100, 0)));

        //when
        auto nextResult = searchResultController->getNextSearchResult(searchedString, TEST_UUID, true, std::make_pair(pair(0, 0), 0), std::make_pair(pair(2, 0), 0));

        //then
        ASSERT_EQ(nextResult, pair(1, 3));
    }

    TEST_F(SearchResultsControllerTest, toThreadStartedNoResults_withSecondaryResultBeforeWrapButAlreadyScannedSecondary_early)
    {
        //before
        std::string searchedString("toThreadStartedNoResults_withSecondaryResultBeforeWrapButAlreadyScannedSecondary_early");
        auto searchResultController = std::make_unique<SearchResultsController>(searchedString, storageMock, printingOrchestratorMock, TEST_UUID, pair(1, 1));
        searchResultController->addToSecondaryResults(searchedString, pair(0, 0), TEST_UUID);
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosDeleted(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosWithChanges(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::StorageMock *>(storageMock.get()), getCharacterAtPos(::testing::_)).WillRepeatedly(::testing::Return('a'));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getLastValidPosition(TEST_UUID, storageMock))
            .WillOnce(::testing::Return(pair(100, 0)));

        //when
        auto prevResult = searchResultController->getPrevSearchResult(searchedString, TEST_UUID, true, std::make_pair(pair(99, 0), 0));

        //then
        ASSERT_EQ(prevResult, pair(0, 0));

        //when
        prevResult = searchResultController->getPrevSearchResult(searchedString, TEST_UUID, true, std::make_pair(pair(99, 0), 0));

        //then
        ASSERT_EQ(prevResult, std::nullopt);
    }

    TEST_F(SearchResultsControllerTest, fromThreadStartedOneResultBeforeWrap_withSecondaryResultButAlreadyScannedSecondary_early)
    {
        //before
        std::string searchedString("fromThreadStartedOneResultBeforeWrap_withSecondaryResultButAlreadyScannedSecondary_early");
        auto searchResultController = std::make_unique<SearchResultsController>(searchedString, storageMock, printingOrchestratorMock, TEST_UUID, pair(1, 0));
        searchResultController->addToSecondaryResults(searchedString, pair(1, 0), TEST_UUID);
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosDeleted(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosWithChanges(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::StorageMock *>(storageMock.get()), getCharacterAtPos(::testing::_)).WillRepeatedly(::testing::Return('a'));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getLastValidPosition(TEST_UUID, storageMock))
            .WillOnce(::testing::Return(pair(100, 0)));

        //when
        auto nextResult = searchResultController->getNextSearchResult(searchedString, TEST_UUID, true, std::make_pair(pair(0, 0), 0), std::make_pair(pair(1, 0), 0));

        //then
        ASSERT_EQ(nextResult, pair(1, 0));

        //when
        nextResult = searchResultController->getNextSearchResult(searchedString, TEST_UUID, true, std::make_pair(pair(99, 0), 0), std::make_pair(pair(1, 0), 0));

        //then
        ASSERT_EQ(nextResult, std::nullopt);
    }

    TEST_F(SearchResultsControllerTest, toThreadOneResultBeforeCursorPosCurrentlyScannedBeforeCursorPos_early)
    {
        //before
        std::string searchedString("toThreadOneResultBeforeCursorPosCurrentlyScannedBeforeCursorPos_early");
        auto searchResultController = std::make_unique<SearchResultsController>(searchedString, storageMock, printingOrchestratorMock, TEST_UUID, pair(10, 1));
        searchResultController->addToSecondaryResults(searchedString, pair(2, 0), TEST_UUID);
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosDeleted(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosWithChanges(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::StorageMock *>(storageMock.get()), getCharacterAtPos(::testing::_)).WillRepeatedly(::testing::Return('a'));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getLastValidPosition(TEST_UUID, storageMock))
            .WillOnce(::testing::Return(pair(100, 0)));

        //when
        auto prevResult = searchResultController->getPrevSearchResult(searchedString, TEST_UUID, true, std::make_pair(pair(1, 0), 0));

        //then
        ASSERT_EQ(prevResult, pair(2, 0));

        //when
        prevResult = searchResultController->getPrevSearchResult(searchedString, TEST_UUID, true, std::make_pair(pair(1, 0), 0));

        //then
        ASSERT_EQ(prevResult, std::nullopt);
    }

    TEST_F(SearchResultsControllerTest, toThreadOneResultAfterCursorPosCurrentlyScannedAfterPos_early)
    {
        //before
        std::string searchedString("toThreadOneResultAfterCursorPosCurrentlyScannedAfterPos_early");
        auto searchResultController = std::make_unique<SearchResultsController>(searchedString, storageMock, printingOrchestratorMock, TEST_UUID, pair(10, 1));
        searchResultController->addToSecondaryResults(searchedString, pair(99, 0), TEST_UUID);
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosDeleted(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosWithChanges(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::StorageMock *>(storageMock.get()), getCharacterAtPos(::testing::_)).WillRepeatedly(::testing::Return('a'));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getLastValidPosition(TEST_UUID, storageMock))
            .WillOnce(::testing::Return(pair(100, 0)));

        //when
        auto prevResult = searchResultController->getPrevSearchResult(searchedString, TEST_UUID, true, std::make_pair(pair(98, 0), 0), std::make_pair(pair(11, 1), 0));

        //then
        ASSERT_EQ(prevResult, pair(99, 0));

        //when
        prevResult = searchResultController->getPrevSearchResult(searchedString, TEST_UUID, true, std::make_pair(pair(90, 0), 0), std::make_pair(pair(11, 1), 0));

        //then
        ASSERT_EQ(prevResult, std::nullopt);
    }

    TEST_F(SearchResultsControllerTest, toThreadOneResultBeforeCursorPosCurrentlyScannedAfterPos_early)
    {
        //before
        std::string searchedString("toThreadOneResultBeforeCursorPosCurrentlyScannedAfterPos_early");
        auto searchResultController = std::make_unique<SearchResultsController>(searchedString, storageMock, printingOrchestratorMock, TEST_UUID, pair(10, 1));
        searchResultController->addToSecondaryResults(searchedString, pair(1, 1), TEST_UUID);
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosDeleted(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosWithChanges(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::StorageMock *>(storageMock.get()), getCharacterAtPos(::testing::_)).WillRepeatedly(::testing::Return('a'));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getLastValidPosition(TEST_UUID, storageMock))
            .WillOnce(::testing::Return(pair(100, 0)));

        //when
        auto prevResult = searchResultController->getPrevSearchResult(searchedString, TEST_UUID, true, std::make_pair(pair(98, 0), 0), std::make_pair(pair(10, 2), 0));

        //then
        ASSERT_EQ(prevResult, pair(1, 1));

        //when
        prevResult = searchResultController->getPrevSearchResult(searchedString, TEST_UUID, true, std::make_pair(pair(90, 0), 0), std::make_pair(pair(10, 2), 0));

        //then
        ASSERT_EQ(prevResult, std::nullopt);

        //when
        prevResult = searchResultController->getNextSearchResult(searchedString, TEST_UUID, true, std::make_pair(pair(90, 0), 0));

        //then
        ASSERT_EQ(prevResult, std::nullopt);

        //when
        prevResult = searchResultController->getNextSearchResult(searchedString, TEST_UUID, true, std::make_pair(pair(90, 0), 0));

        //then
        ASSERT_EQ(prevResult, std::nullopt);
    }

    TEST_F(SearchResultsControllerTest, fromThreadOneResultAfterCursorPosCurrentlyScannedAfterPos_early)
    {
        //before
        std::string searchedString("fromThreadOneResultAfterCursorPosCurrentlyScannedAfterPos_early");
        auto searchResultController = std::make_unique<SearchResultsController>(searchedString, storageMock, printingOrchestratorMock, TEST_UUID, pair(10, 1));
        searchResultController->addToSecondaryResults(searchedString, std::make_pair(pair(1, 1), 0), TEST_UUID);
        searchResultController->addToSecondaryResults(searchedString, std::make_pair(pair(11, 1), 0), TEST_UUID);
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosDeleted(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosWithChanges(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::StorageMock *>(storageMock.get()), getCharacterAtPos(::testing::_)).WillRepeatedly(::testing::Return('a'));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getLastValidPosition(TEST_UUID, storageMock))
            .WillRepeatedly(::testing::Return(pair(100, 0)));

        //when
        auto nextResult = searchResultController->getNextSearchResult(searchedString, TEST_UUID, true, std::make_pair(pair(9, 1), 0), std::make_pair(pair(12, 0), 0));

        //then
        ASSERT_EQ(nextResult, pair(11, 1));

        //when
        nextResult = searchResultController->getNextSearchResult(searchedString, TEST_UUID, true, std::make_pair(pair(8, 0), 0), std::make_pair(pair(13, 0), 0));

        //then
        ASSERT_EQ(nextResult, std::nullopt);

        //when
        nextResult = searchResultController->getNextSearchResult(searchedString, TEST_UUID, true, std::make_pair(pair(7, 0), 0), std::make_pair(pair(1, 2), 0));

        //then
        ASSERT_EQ(nextResult, pair(1, 1));

        //when
        nextResult = searchResultController->getNextSearchResult(searchedString, TEST_UUID, true, std::make_pair(pair(4, 0), 0), std::make_pair(pair(1, 2), 0));

        //then
        ASSERT_EQ(nextResult, std::nullopt);

        //when
        nextResult = searchResultController->getPrevSearchResult(searchedString, TEST_UUID, true, std::make_pair(pair(4, 0), 0), std::make_pair(pair(1, 2), 0));

        //then
        ASSERT_EQ(nextResult, pair(11, 1));

        //when
        nextResult = searchResultController->getPrevSearchResult(searchedString, TEST_UUID, true, std::make_pair(pair(4, 0), 0), std::make_pair(pair(1, 2), 0));

        //then
        ASSERT_EQ(nextResult, std::nullopt);
    }

    TEST_F(SearchResultsControllerTest, cantGoToNextPrevIfInLastPos_early)
    {
        //before
        std::string searchedString("cantGoToNextIfInLastPos_early");
        auto searchResultController = std::make_unique<SearchResultsController>(searchedString, storageMock, printingOrchestratorMock, TEST_UUID, pair(10, 1));
        searchResultController->addResult(searchedString, std::make_pair(pair(10, 1), 0), TEST_UUID);
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosDeleted(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosWithChanges(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::StorageMock *>(storageMock.get()), getCharacterAtPos(::testing::_)).WillRepeatedly(::testing::Return('a'));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getLastValidPosition(TEST_UUID, storageMock))
            .WillRepeatedly(::testing::Return(pair(100, 0)));

        //when
        auto nextResult = searchResultController->getNextSearchResult(searchedString, TEST_UUID, true, std::make_pair(pair(9, 1), 0), std::make_pair(pair(11, 1), 0));

        //then
        ASSERT_EQ(nextResult, pair(10, 1));

        //when
        nextResult = searchResultController->getNextSearchResult(searchedString, TEST_UUID, true, std::make_pair(pair(9, 1), 0), std::make_pair(pair(12, 0), 0));

        //then
        ASSERT_EQ(nextResult, std::nullopt);

        //when
        searchResultController->addResult(searchedString, std::make_pair(pair(9, 0), 0), TEST_UUID);
        nextResult = searchResultController->getPrevSearchResult(searchedString, TEST_UUID, true, std::make_pair(pair(99, 0), 0), std::make_pair(pair(12, 0), 0));

        //then
        ASSERT_EQ(nextResult, pair(9, 0));

        //when
        nextResult = searchResultController->getPrevSearchResult(searchedString, TEST_UUID, true, std::make_pair(pair(99, 0), 0), std::make_pair(pair(12, 0), 0));

        //then
        ASSERT_EQ(nextResult, std::nullopt);
    }

    TEST_F(SearchResultsControllerTest, toThreadStartedNoResults_withSecondaryResultBeforeWrapButAlreadyScannedSecondaryShouldBeAbleToGoBack_early)
    {
        //before
        std::string searchedString("toThreadStartedNoResults_withSecondaryResultBeforeWrapButAlreadyScannedSecondaryShouldBeAbleToGoBack_early");
        auto searchResultController = std::make_unique<SearchResultsController>(searchedString, storageMock, printingOrchestratorMock, TEST_UUID, pair(1, 1));
        searchResultController->addToSecondaryResults(searchedString, std::make_pair(pair(0, 0), 0), TEST_UUID);
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosDeleted(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosWithChanges(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::StorageMock *>(storageMock.get()), getCharacterAtPos(::testing::_)).WillRepeatedly(::testing::Return('a'));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getLastValidPosition(TEST_UUID, storageMock))
            .WillRepeatedly(::testing::Return(pair(100, 0)));

        //when
        auto prevResult = searchResultController->getPrevSearchResult(searchedString, TEST_UUID, true, std::make_pair(pair(99, 0), 0), std::make_pair(pair(1, 2), 0));

        //then
        ASSERT_EQ(prevResult, pair(0, 0));

        //when
        prevResult = searchResultController->getPrevSearchResult(searchedString, TEST_UUID, true, std::make_pair(pair(99, 0), 0), std::make_pair(pair(1, 2), 0));

        //then
        ASSERT_EQ(prevResult, std::nullopt);

        //when
        searchResultController->addResult(searchedString, std::make_pair(pair(98, 0), 0), TEST_UUID);
        prevResult = searchResultController->getPrevSearchResult(searchedString, TEST_UUID, true, std::make_pair(pair(99, 0), 0), std::make_pair(pair(1, 2), 0));

        //then
        ASSERT_EQ(prevResult, std::nullopt);

        //when
        prevResult = searchResultController->getPrevSearchResult(searchedString, TEST_UUID, true, std::make_pair(pair(97, 0), 0), std::make_pair(pair(1, 2), 0));

        //then
        ASSERT_EQ(prevResult, pair(98, 0));

        //when
        prevResult = searchResultController->getPrevSearchResult(searchedString, TEST_UUID, true, std::make_pair(pair(97, 0), 0), std::make_pair(pair(1, 2), 0));

        //then
        ASSERT_EQ(prevResult, std::nullopt);

        //when
        auto nextResult = searchResultController->getNextSearchResult(searchedString, TEST_UUID, true, std::make_pair(pair(97, 0), 0), std::make_pair(pair(1, 2), 0));

        //then
        ASSERT_EQ(nextResult, pair(0, 0));
    }

    TEST_F(SearchResultsControllerTest, bothToAndFromResults_early)
    {
        //before
        std::string searchedString("bothToAndFromResults_early");
        auto searchResultController = std::make_unique<SearchResultsController>(searchedString, storageMock, printingOrchestratorMock, TEST_UUID, pair(10, 1));
        searchResultController->addToSecondaryResults(searchedString, std::make_pair(pair(0, 0), 0), TEST_UUID);
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosDeleted(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosWithChanges(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::StorageMock *>(storageMock.get()), getCharacterAtPos(::testing::_)).WillRepeatedly(::testing::Return('a'));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getLastValidPosition(TEST_UUID, storageMock))
            .WillRepeatedly(::testing::Return(pair(100, 0)));

        //when
        auto prevResult = searchResultController->getPrevSearchResult(searchedString, TEST_UUID, true, std::make_pair(pair(99, 0), 0), std::make_pair(pair(10, 1), 0));

        //then
        ASSERT_EQ(prevResult, pair(0, 0));

        //when
        prevResult = searchResultController->getPrevSearchResult(searchedString, TEST_UUID, true, std::make_pair(pair(99, 0), 0), std::make_pair(pair(10, 1), 0));

        //then
        ASSERT_EQ(prevResult, std::nullopt);

        //when
        auto nextResult = searchResultController->getNextSearchResult(searchedString, TEST_UUID, true, std::make_pair(pair(10, 0), 0));

        //then
        ASSERT_EQ(nextResult, std::nullopt);
    }

    TEST_F(SearchResultsControllerTest, getFirstPosAfter_CursorBeforeAllResults)
    {
        //before
        std::string searchedString("getFirstPosAfter_CursorBeforeAllResults");
        auto searchResultController = std::make_unique<SearchResultsController>(searchedString, storageMock, printingOrchestratorMock, TEST_UUID, pair(10, 1));
        searchResultController->addToSecondaryResults(searchedString, pair(10, 1), TEST_UUID);
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosDeleted(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosWithChanges(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::StorageMock *>(storageMock.get()), getCharacterAtPos(::testing::_)).WillRepeatedly(::testing::Return('a'));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getLastValidPosition(TEST_UUID, storageMock))
            .WillRepeatedly(::testing::Return(pair(100, 0)));

        //when
        auto posAfter = searchResultController->getFirstPosAfter(searchedString, pair(10, 1), true, std::make_pair(pair(10, 1), 0), TEST_UUID);

        //then
        ASSERT_EQ(*posAfter, pair(10, 1));

        //when
        posAfter = searchResultController->getFirstPosAfter(searchedString, pair(10, 1), true, std::make_pair(pair(10, 2), 0), TEST_UUID);

        //then
        ASSERT_EQ(*posAfter, pair(10, 1));
    }

    TEST_F(SearchResultsControllerTest, getFirstPosAfter_CursorAfterAllResultsButSearchAlreadyWrapped)
    {
        //before
        std::string searchedString("getFirstPosAfter_CursorAfterAllResultsButSearchAlreadyWrapped");
        auto searchResultController = std::make_unique<SearchResultsController>(searchedString, storageMock, printingOrchestratorMock, TEST_UUID, pair(10, 1));
        searchResultController->addToSecondaryResults(searchedString, pair(9, 0), TEST_UUID);
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosDeleted(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosWithChanges(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::StorageMock *>(storageMock.get()), getCharacterAtPos(::testing::_)).WillRepeatedly(::testing::Return('a'));

        //when
        auto posAfter = searchResultController->getFirstPosAfter(searchedString, pair(10, 1), true, std::make_pair(pair(1, 0), 0), TEST_UUID);

        //then
        ASSERT_TRUE(searchResultController->isEndIterator(searchedString, posAfter, TEST_UUID));

        //when
        posAfter = searchResultController->getFirstPosAfter(searchedString, pair(10, 1), true, std::make_pair(pair(10, 0), 0), TEST_UUID);

        //then
        ASSERT_EQ(*posAfter, pair(9, 0));
    }

    TEST_F(SearchResultsControllerTest, getFirstPosAfter_CursorAfterAllResultsSearchDidntWrapped)
    {
        //before
        std::string searchedString("getFirstPosAfter_CursorAfterAllResultsSearchDidntWrapped");
        auto searchResultController = std::make_unique<SearchResultsController>(searchedString, storageMock, printingOrchestratorMock, TEST_UUID, pair(10, 1));
        searchResultController->addToSecondaryResults(searchedString, std::make_pair(pair(9, 0), 0), TEST_UUID);
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosDeleted(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosWithChanges(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::StorageMock *>(storageMock.get()), getCharacterAtPos(::testing::_)).WillRepeatedly(::testing::Return('a'));

        //when
        auto posAfter = searchResultController->getFirstPosAfter(searchedString, pair(10, 1), true, std::make_pair(pair(11, 0), 0), TEST_UUID);

        //then
        ASSERT_TRUE(searchResultController->isEndIterator(searchedString, posAfter, TEST_UUID));
    }

    TEST_F(SearchResultsControllerTest, getFirstPosAfter)
    {
        //before
        std::string searchedString("getFirstPosAfter_CursorAfterAllResultsSearchDidntWrapped");
        auto searchResultController = std::make_unique<SearchResultsController>(searchedString, storageMock, printingOrchestratorMock, TEST_UUID, pair(10, 2));
        searchResultController->addToSecondaryResults(searchedString, std::make_pair(pair(10, 1), 0), TEST_UUID);
        searchResultController->addToSecondaryResults(searchedString, std::make_pair(pair(10, 2), 0), TEST_UUID);
        searchResultController->addToSecondaryResults(searchedString, std::make_pair(pair(10, 5), 0), TEST_UUID);
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosDeleted(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosWithChanges(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::StorageMock *>(storageMock.get()), getCharacterAtPos(::testing::_)).WillRepeatedly(::testing::Return('a'));

        //when
        auto posAfter = searchResultController->getFirstPosAfter(searchedString, pair(10, 2), true, std::make_pair(pair(10, 2), 0), TEST_UUID);

        //then
        ASSERT_EQ(*posAfter, pair(10, 2));

        //when
        posAfter = searchResultController->getFirstPosAfter(searchedString, pair(10, 3), true, std::make_pair(pair(10, 4), 0), TEST_UUID);

        //then
        ASSERT_TRUE(searchResultController->isEndIterator(searchedString, posAfter, TEST_UUID));

        //when
        posAfter = searchResultController->getFirstPosAfter(searchedString, pair(10, 3), true, std::make_pair(pair(10, 5), 0), TEST_UUID);

        //then
        ASSERT_EQ(*posAfter, pair(10, 5));
    }

    TEST_F(SearchResultsControllerTest, getFirstPosBefore_CursorAfterAllResultsSearchDidntWrapped)
    {
        //before
        std::string searchedString("getFirstPosBefore_CursorAfterAllResultsSearchDidntWrapped");
        auto searchResultController = std::make_unique<SearchResultsController>(searchedString, storageMock, printingOrchestratorMock, TEST_UUID, pair(9, 0));
        searchResultController->addToSecondaryResults(searchedString, std::make_pair(pair(10, 1), 0), TEST_UUID);
        searchResultController->addToSecondaryResults(searchedString, std::make_pair(pair(10, 2), 0), TEST_UUID);
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosDeleted(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosWithChanges(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::StorageMock *>(storageMock.get()), getCharacterAtPos(::testing::_)).WillRepeatedly(::testing::Return('a'));

        //when
        auto posBefore = searchResultController->getFirstPosBefore(searchedString, pair(9, 0), true, std::make_pair(pair(1, 0), 0), TEST_UUID);

        //then
        ASSERT_TRUE(searchResultController->isEndIterator(searchedString, posBefore, TEST_UUID));
    }

    TEST_F(SearchResultsControllerTest, getFirstPosBefore_CursorAfterAllResultsSearchWrapped)
    {
        //before
        std::string searchedString("getFirstPosBefore_CursorAfterAllResultsSearchDidntWrapped");
        auto searchResultController = std::make_unique<SearchResultsController>(searchedString, storageMock, printingOrchestratorMock, TEST_UUID, pair(9, 0));
        searchResultController->addToSecondaryResults(searchedString, std::make_pair(pair(10, 1), 0), TEST_UUID);
        searchResultController->addToSecondaryResults(searchedString, std::make_pair(pair(10, 2), 0), TEST_UUID);
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosDeleted(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosWithChanges(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::StorageMock *>(storageMock.get()), getCharacterAtPos(::testing::_)).WillRepeatedly(::testing::Return('a'));

        //when
        auto posBefore = searchResultController->getFirstPosBefore(searchedString, pair(9, 0), true, std::make_pair(pair(10, 3), 0), TEST_UUID);

        //then
        ASSERT_TRUE(searchResultController->isEndIterator(searchedString, posBefore, TEST_UUID));

        //when
        posBefore = searchResultController->getFirstPosBefore(searchedString, pair(9, 0), true, std::make_pair(pair(10, 2), 0), TEST_UUID);

        //then
        ASSERT_EQ(*posBefore, pair(10, 2));
    }

    TEST_F(SearchResultsControllerTest, getFirstPosBefore_CursorAfterAllResults)
    {
        //before
        std::string searchedString("getFirstPosAfter_CursorAfterAllResults");
        auto searchResultController = std::make_unique<SearchResultsController>(searchedString, storageMock, printingOrchestratorMock, TEST_UUID, pair(10, 3));
        searchResultController->addToSecondaryResults(searchedString, std::make_pair(pair(10, 1), 0), TEST_UUID);
        searchResultController->addToSecondaryResults(searchedString, std::make_pair(pair(10, 2), 0), TEST_UUID);
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosDeleted(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosWithChanges(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::StorageMock *>(storageMock.get()), getCharacterAtPos(::testing::_)).WillRepeatedly(::testing::Return('a'));

        //when
        auto posBefore = searchResultController->getFirstPosBefore(searchedString, pair(10, 3), true, std::make_pair(pair(10, 3), 0), TEST_UUID);

        //then
        ASSERT_TRUE(searchResultController->isEndIterator(searchedString, posBefore, TEST_UUID));

        //when
        posBefore = searchResultController->getFirstPosBefore(searchedString, pair(10, 3), true, std::make_pair(pair(10, 2), 0), TEST_UUID);

        //then
        ASSERT_EQ(*posBefore, pair(10, 2));
    }

    TEST_F(SearchResultsControllerTest, getFirstPosBefore)
    {
        //before
        std::string searchedString("getFirstPosAfter_CursorAfterAllResultsSearchDidntWrapped");
        auto searchResultController = std::make_unique<SearchResultsController>(searchedString, storageMock, printingOrchestratorMock, TEST_UUID, pair(10, 4));
        searchResultController->addToSecondaryResults(searchedString, std::make_pair(pair(10, 1), 0), TEST_UUID);
        searchResultController->addToSecondaryResults(searchedString, std::make_pair(pair(10, 2), 0), TEST_UUID);
        searchResultController->addToSecondaryResults(searchedString, std::make_pair(pair(10, 5), 0), TEST_UUID);
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosDeleted(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosWithChanges(::testing::_, TEST_UUID)).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(*dynamic_cast<testing::mocks::StorageMock *>(storageMock.get()), getCharacterAtPos(::testing::_)).WillRepeatedly(::testing::Return('a'));

        //when
        auto posBefore = searchResultController->getFirstPosBefore(searchedString, pair(10, 4), true, std::make_pair(pair(10, 3), 0), TEST_UUID);

        //then
        ASSERT_TRUE(searchResultController->isEndIterator(searchedString, posBefore, TEST_UUID));

        //when
        posBefore = searchResultController->getFirstPosBefore(searchedString, pair(10, 4), true, std::make_pair(pair(10, 2), 0), TEST_UUID);

        //then
        ASSERT_TRUE(searchResultController->isEndIterator(searchedString, posBefore, TEST_UUID));

        //when
        posBefore = searchResultController->getFirstPosBefore(searchedString, pair(10, 1), true, std::make_pair(pair(9, 5), 0), TEST_UUID);

        //then
        ASSERT_TRUE(searchResultController->isEndIterator(searchedString, posBefore, TEST_UUID));
    }

    //LAST TEST
    TEST_F(SearchResultsControllerTest, getFirstPosBefore)
    {
        //before
        auto searchResultController = std::make_unique<SearchResultsController>(pair(0, 0));
        searchResultController->initSearchResult(searchedString);

        //when
        auto result = searchResultController->getFirstPosBefore<size_t, 0>(searchedString, 0);

        //then
        ASSERT_TRUE((searchResultController->isEndIterator<size_t, 0>(searchedString, result)));

        //when
        searchResultController->addResult(searchedString, 5, utils::Direction::UP);
        result = searchResultController->getFirstPosBefore<size_t, 0>(searchedString, 5);

        //then
        ASSERT_EQ((searchResultController->getCurrInitial(searchedString)), (std::end(std::get<0>(searchResultController->results[searchedString]).first)));

        //when
        result = searchResultController->getFirstPosBefore<size_t, 0>(searchedString, 6);

        //then
        ASSERT_EQ(*result, 5);

        //when
        searchResultController->addResult(searchedString, 4, utils::Direction::UP);
        searchResultController->addResult(searchedString, 2, utils::Direction::UP);
        searchResultController->addResult(searchedString, 20, utils::Direction::UP);
        result = searchResultController->getFirstPosBefore<size_t, 0>(searchedString, 3);

        //then
        ASSERT_EQ(*result, 2);

        //when
        result = searchResultController->getFirstPosBefore<size_t, 0>(searchedString, 1);
        ASSERT_EQ((searchResultController->getCurrInitial(searchedString)), (std::end(std::get<0>(searchResultController->results[searchedString]).first)));
    }

    TEST_F(SearchResultsControllerTest, getFirstPosAfter)
    {
        //before
        auto searchResultController = std::make_unique<SearchResultsController>(pair(0, 0));
        searchResultController->initSearchResult(searchedString);

        //when
        auto result = searchResultController->getFirstPosAfter<size_t, 0>(searchedString, 0);

        //then
        ASSERT_TRUE((searchResultController->isEndIterator<size_t, 0>(searchedString, result)));

        //when
        searchResultController->addResult(searchedString, 5, utils::Direction::UP);
        result = searchResultController->getFirstPosAfter<size_t, 0>(searchedString, 5);

        //then
        ASSERT_EQ(result, (std::end(std::get<0>(searchResultController->results[searchedString]).first)));

        //when
        searchResultController->addResult(searchedString, 4, utils::Direction::UP);
        searchResultController->addResult(searchedString, 2, utils::Direction::UP);
        searchResultController->addResult(searchedString, 20, utils::Direction::UP);
        result = searchResultController->getFirstPosAfter<size_t, 0>(searchedString, 5);

        //then
        ASSERT_EQ(*result, 20);

        //when
        result = searchResultController->getFirstPosAfter<size_t, 0>(searchedString, 1);
        ASSERT_EQ(*result, 2);
    }

    TEST_F(SearchResultsControllerTest, getPrevSearchResultNoResults)
    {
        //before
        auto searchResultController = std::make_unique<SearchResultsController>(pair(11, 0));
        searchResultController->initSearchResult(searchedString);

        //when
        auto prevResult = searchResultController->getPrevSearchResult(searchedString);

        //then
        ASSERT_FALSE(prevResult.has_value());

        //when
        prevResult = searchResultController->getPrevSearchResult(searchedString);

        //then
        ASSERT_FALSE(prevResult.has_value());
    }

    TEST_F(SearchResultsControllerTest, getNextSearchResultNoResults)
    {
        //before
        auto searchResultController = std::make_unique<SearchResultsController>(pair(11, 0));
        searchResultController->initSearchResult(searchedString);

        //when
        auto nextResult = searchResultController->getNextSearchResult(searchedString, true);

        //then
        ASSERT_FALSE(nextResult.has_value());

        //when
        nextResult = searchResultController->getNextSearchResult(searchedString, true);

        //then
        ASSERT_FALSE(nextResult.has_value());
    }

    TEST_F(SearchResultsControllerTest, getPrevSearchResultOneResultNoSecondarySearchResults)
    {
        //before
        auto searchResultController = std::make_unique<SearchResultsController>(pair(11, 0));
        searchResultController->initSearchResult(searchedString);

        //when
        searchResultController->addResult(searchedString, 10, utils::Direction::UP);
        auto prevResult = searchResultController->getPrevSearchResult(searchedString, true);

        //then
        ASSERT_TRUE(prevResult.has_value());
        ASSERT_EQ(prevResult.value(), pair(10, 0));
        ASSERT_EQ((*searchResultController->getCurrInitial(searchedString)), 10);

        //when
        prevResult = searchResultController->getPrevSearchResult(searchedString, true);

        //then
        ASSERT_FALSE(prevResult.has_value());
    }

    TEST_F(SearchResultsControllerTest, getNextSearchResultOneResultNoSecondarySearchResults)
    {
        //before
        auto searchResultController = std::make_unique<SearchResultsController>(pair(11, 0));
        searchResultController->initSearchResult(searchedString);

        //when
        searchResultController->addResult(searchedString, 11, utils::Direction::DOWN);
        auto nextResult = searchResultController->getNextSearchResult(searchedString, true);

        //then
        ASSERT_TRUE(nextResult.has_value());
        ASSERT_EQ(nextResult.value(), pair(11, 0));
        ASSERT_EQ((*searchResultController->getCurrInitial(searchedString)), 11);
        ASSERT_EQ((searchResultController->getCurrSecondary(searchedString)), (std::end(std::get<1>(searchResultController->results[searchedString]).first)));

        //when
        nextResult = searchResultController->getNextSearchResult(searchedString, true);

        //then
        ASSERT_FALSE(nextResult.has_value());
    }

    TEST_F(SearchResultsControllerTest, getPrevSearchResultNoResultsOneSecondarySearchResult)
    {
        //before
        auto searchResultController = std::make_unique<SearchResultsController>(pair(11, 0));
        searchResultController->initSearchResult(searchedString);

        //when
        searchResultController->addToSecondaryResults(searchedString, pair(3, 0));
        auto prevResult = searchResultController->getPrevSearchResult(searchedString, true);

        //then
        ASSERT_FALSE(prevResult.has_value());

        //when
        prevResult = searchResultController->getPrevSearchResult(searchedString, true);

        //then
        ASSERT_FALSE(prevResult.has_value());
    }

    TEST_F(SearchResultsControllerTest, getNextSearchResultNoResultsOneSecondarySearchResult)
    {
        //before
        auto searchResultController = std::make_unique<SearchResultsController>(pair(11, 0));
        searchResultController->initSearchResult(searchedString);

        //when
        searchResultController->addToSecondaryResults(searchedString, pair(3, 0));
        auto nextResult = searchResultController->getNextSearchResult(searchedString, true);

        //then
        ASSERT_FALSE(nextResult.has_value());

        //when
        nextResult = searchResultController->getNextSearchResult(searchedString, true);

        //then
        ASSERT_FALSE(nextResult.has_value());
    }

    TEST_F(SearchResultsControllerTest, getPrevSearchResultOneToResultBeforeOneSecondarySearchResult)
    {
        //before
        auto searchResultController = std::make_unique<SearchResultsController>(pair(11, 0));
        searchResultController->initSearchResult(searchedString);

        //when
        searchResultController->addResult(searchedString, 10, utils::Direction::UP);
        searchResultController->addToSecondaryResults(searchedString, pair(3, 0));
        auto prevResult = searchResultController->getPrevSearchResult(searchedString, true);

        //then
        ASSERT_TRUE(prevResult.has_value());
        ASSERT_EQ(prevResult.value(), pair(10, 0));
        ASSERT_EQ((*searchResultController->getCurrInitial(searchedString)), 10);
        ASSERT_EQ((searchResultController->getCurrSecondary(searchedString)), (std::end(std::get<1>(searchResultController->results[searchedString]).first)));

        //when
        prevResult = searchResultController->getPrevSearchResult(searchedString, true);

        //then
        ASSERT_FALSE(prevResult.has_value());
    }

    TEST_F(SearchResultsControllerTest, getNextSearchResultOneToResultBeforeOneSecondarySearchResult)
    {
        //before
        auto searchResultController = std::make_unique<SearchResultsController>(pair(10, 0));
        searchResultController->initSearchResult(searchedString);

        //when
        searchResultController->addResult(searchedString, 10, utils::Direction::DOWN);
        searchResultController->addToSecondaryResults(searchedString, pair(3, 0));
        auto prevResult = searchResultController->getNextSearchResult(searchedString, true);

        //then
        ASSERT_TRUE(prevResult.has_value());
        ASSERT_EQ(prevResult.value(), pair(10, 0));
        ASSERT_EQ((*searchResultController->getCurrInitial(searchedString)), 10);
        ASSERT_EQ((searchResultController->getCurrSecondary(searchedString)), (std::end(std::get<1>(searchResultController->results[searchedString]).first)));

        //when
        //prevResult = searchResultController->getNextSearchResult(searchedString, true);

        //then
        //ASSERT_FALSE(prevResult.has_value());
    }

        TEST_F(SearchResultsControllerTest, getNextSearchResultOneToResultBeforeOneSecondarySearchResult_2)
    {
        //before
        auto searchResultController = std::make_unique<SearchResultsController>(pair(10, 0));
        searchResultController->initSearchResult(searchedString);

        //when
        searchResultController->addResult(searchedString, 10, utils::Direction::DOWN);
        searchResultController->addResult(searchedString, 15, utils::Direction::DOWN);
        searchResultController->addResult(searchedString, 3, utils::Direction::UP);
        searchResultController->addToSecondaryResults(searchedString, pair(4, 0));
        auto nextResult = searchResultController->getNextSearchResult(searchedString, true);

        //then
        ASSERT_TRUE(nextResult.has_value());
        ASSERT_EQ(nextResult.value(), pair(10, 0));
        ASSERT_EQ((*searchResultController->getCurrInitial(searchedString)), 10);
        ASSERT_EQ((*searchResultController->getCurrSecondary(searchedString)), pair(4, 0));

        //when
        nextResult = searchResultController->getNextSearchResult(searchedString, true);

        //then
        ASSERT_TRUE(nextResult.has_value());
        ASSERT_EQ(nextResult.value(), pair(15, 0));
        ASSERT_EQ((*searchResultController->getCurrInitial(searchedString)), 15);
        ASSERT_EQ((*searchResultController->getCurrSecondary(searchedString)), pair(4, 0));
        //ASSERT_EQ((searchResultController->getCurrSecondary(searchedString)), (std::end(std::get<1>(searchResultController->results[searchedString]).first)));

        //when
        nextResult = searchResultController->getNextSearchResult(searchedString, true);

        //then
        ASSERT_FALSE(nextResult.has_value());
    }

            TEST_F(SearchResultsControllerTest, getNextSearchResultOneToResultBeforeOneSecondarySearchResult_3)
    {
        //before
        auto searchResultController = std::make_unique<SearchResultsController>(pair(10, 0));
        searchResultController->initSearchResult(searchedString);

        //when
        searchResultController->addResult(searchedString, 10, utils::Direction::DOWN);
        searchResultController->addResult(searchedString, 15, utils::Direction::DOWN);
        searchResultController->addResult(searchedString, 4, utils::Direction::UP);
        searchResultController->addToSecondaryResults(searchedString, pair(3, 0));
        auto nextResult = searchResultController->getNextSearchResult(searchedString, true);

        //then
        ASSERT_TRUE(nextResult.has_value());
        ASSERT_EQ(nextResult.value(), pair(10, 0));
        ASSERT_EQ((*searchResultController->getCurrInitial(searchedString)), 10);
        ASSERT_EQ((*searchResultController->getCurrSecondary(searchedString)), pair(3, 0));

        //when
        nextResult = searchResultController->getNextSearchResult(searchedString, true);

        //then
        ASSERT_TRUE(nextResult.has_value());
        ASSERT_EQ(nextResult.value(), pair(15, 0));
        ASSERT_EQ((*searchResultController->getCurrInitial(searchedString)), 15);
        ASSERT_EQ((*searchResultController->getCurrSecondary(searchedString)), pair(3, 0));
        //ASSERT_EQ((searchResultController->getCurrSecondary(searchedString)), (std::end(std::get<1>(searchResultController->results[searchedString]).first)));

        //when
        nextResult = searchResultController->getNextSearchResult(searchedString, true);

        //then
        ASSERT_FALSE(nextResult.has_value());
    }

    TEST_F(SearchResultsControllerTest, getPrevSearchResultOneToResultAfterOneSecondarySearchResult)
    {
        //before
        auto searchResultController = std::make_unique<SearchResultsController>(pair(11, 0));
        searchResultController->initSearchResult(searchedString);

        //when
        searchResultController->addResult(searchedString, 3, utils::Direction::UP);
        searchResultController->addToSecondaryResults(searchedString, pair(10, 0));
        auto prevResult = searchResultController->getPrevSearchResult(searchedString, true);

        //then
        ASSERT_TRUE(prevResult.has_value());
        ASSERT_EQ(prevResult.value(), pair(10, 0));
        ASSERT_EQ((*searchResultController->getCurrSecondary(searchedString)), pair(10, 0));
        ASSERT_EQ((searchResultController->getCurrInitial(searchedString)), (std::end(std::get<0>(searchResultController->results[searchedString]).first)));

        //when
        prevResult = searchResultController->getPrevSearchResult(searchedString, true);

        //then
        ASSERT_FALSE(prevResult.has_value());
    }

    TEST_F(SearchResultsControllerTest, getNextSearchResultOneToResultAfterOneSecondarySearchResult)
    {
        //before
        auto searchResultController = std::make_unique<SearchResultsController>(pair(11, 0));
        searchResultController->initSearchResult(searchedString);

        //when
        searchResultController->addResult(searchedString, 12, utils::Direction::DOWN);
        searchResultController->addToSecondaryResults(searchedString, pair(11, 0));
        auto nextResult = searchResultController->getNextSearchResult(searchedString, true);

        //then
        ASSERT_TRUE(nextResult.has_value());
        ASSERT_EQ(nextResult.value(), pair(11, 0));
        ASSERT_EQ((searchResultController->getCurrInitial(searchedString)), (std::end(std::get<0>(searchResultController->results[searchedString]).first)));
        ASSERT_EQ((*searchResultController->getCurrSecondary(searchedString)), pair(11, 0));

        //when
        nextResult = searchResultController->getNextSearchResult(searchedString, true);

        //then
        ASSERT_FALSE(nextResult.has_value());
    }

    TEST_F(SearchResultsControllerTest, getPrevSearchResultMultipleResultsNoSecondarySearchResult)
    {
        //before
        auto searchResultController = std::make_unique<SearchResultsController>(pair(11, 0));
        searchResultController->initSearchResult(searchedString);

        //when
        searchResultController->addResult(searchedString, 10, utils::Direction::UP);
        searchResultController->addResult(searchedString, 0, utils::Direction::UP);
        searchResultController->addResult(searchedString, 199, utils::Direction::UP);
        auto prevResult = searchResultController->getPrevSearchResult(searchedString, true);

        //then
        ASSERT_TRUE(prevResult.has_value());
        ASSERT_EQ(prevResult.value(), pair(10, 0));
        ASSERT_EQ((*searchResultController->getCurrInitial(searchedString)), 10);
        ASSERT_EQ((searchResultController->getCurrSecondary(searchedString)), (std::end(std::get<1>(searchResultController->results[searchedString]).first)));

        //when
        prevResult = searchResultController->getPrevSearchResult(searchedString, true);

        //then
        ASSERT_TRUE(prevResult.has_value());
        ASSERT_EQ(prevResult.value(), pair(0, 0));
        ASSERT_EQ((*searchResultController->getCurrInitial(searchedString)), 0);
        ASSERT_EQ((searchResultController->getCurrSecondary(searchedString)), (std::end(std::get<1>(searchResultController->results[searchedString]).first)));

        //when
        prevResult = searchResultController->getPrevSearchResult(searchedString, true);

        //then
        ASSERT_TRUE(prevResult.has_value());
        ASSERT_EQ(prevResult.value(), pair(199, 0));
        ASSERT_EQ((*searchResultController->getCurrInitial(searchedString)), 199);
        ASSERT_EQ((searchResultController->getCurrSecondary(searchedString)), (std::end(std::get<1>(searchResultController->results[searchedString]).first)));

        //when
        prevResult = searchResultController->getPrevSearchResult(searchedString, true);

        //then
        ASSERT_FALSE(prevResult.has_value());
    }

    TEST_F(SearchResultsControllerTest, getNextSearchResultMultipleResultsNoSecondarySearchResult)
    {
        //before
        auto searchResultController = std::make_unique<SearchResultsController>(pair(11, 0));
        searchResultController->initSearchResult(searchedString);

        //when
        searchResultController->addResult(searchedString, 10, utils::Direction::DOWN);
        searchResultController->addResult(searchedString, 0, utils::Direction::DOWN);
        searchResultController->addResult(searchedString, 199, utils::Direction::DOWN);
        auto nextResult = searchResultController->getNextSearchResult(searchedString, true);

        //then
        ASSERT_TRUE(nextResult.has_value());
        ASSERT_EQ(nextResult.value(), pair(199, 0));
        ASSERT_EQ((*searchResultController->getCurrInitial(searchedString)), 199);
        ASSERT_EQ((searchResultController->getCurrSecondary(searchedString)), (std::end(std::get<1>(searchResultController->results[searchedString]).first)));

        //when
        nextResult = searchResultController->getNextSearchResult(searchedString, true);

        //then
        ASSERT_TRUE(nextResult.has_value());
        ASSERT_EQ(nextResult.value(), pair(0, 0));
        ASSERT_EQ((*searchResultController->getCurrInitial(searchedString)), 0);
        ASSERT_EQ((searchResultController->getCurrSecondary(searchedString)), (std::end(std::get<1>(searchResultController->results[searchedString]).first)));

        //when
        nextResult = searchResultController->getNextSearchResult(searchedString, true);

        //then
        ASSERT_FALSE(nextResult.has_value());
    }

    TEST_F(SearchResultsControllerTest, getPrevSearchResultOneFromResultBeforeOneSecondarySearchResult)
    {
        //before
        auto searchResultController = std::make_unique<SearchResultsController>(pair(11, 0));
        searchResultController->initSearchResult(searchedString);

        //when
        searchResultController->addResult(searchedString, 11, utils::Direction::UP);
        searchResultController->addToSecondaryResults(searchedString, pair(11, 0));
        auto prevResult = searchResultController->getPrevSearchResult(searchedString, true);

        //then
        ASSERT_TRUE(prevResult.has_value());
        ASSERT_EQ(prevResult.value(), pair(11, 0));
        ASSERT_EQ((*searchResultController->getCurrInitial(searchedString)), 11);
        ASSERT_EQ((searchResultController->getCurrSecondary(searchedString)), (std::end(std::get<1>(searchResultController->results[searchedString]).first)));

        //when
        prevResult = searchResultController->getPrevSearchResult(searchedString);

        //then
        ASSERT_FALSE(prevResult.has_value());
        //ASSERT_EQ(nextResult.value(), pair(11, 0));
        //ASSERT_EQ((*searchResultController->getCurrResult<size_t, 1>(searchedString)), 11);
        //ASSERT_EQ((*searchResultController->getCurrResult<pair, 2>(searchedString)), pair(11, 0));

        //when
        //nextResult = searchResultController->getNextSearchResult(searchedString);

        //then
        //ASSERT_FALSE(nextResult.has_value());
    }

    TEST_F(SearchResultsControllerTest, getPrevSearchResult)
    {
        //before
        auto searchResultController = std::make_unique<SearchResultsController>(pair(11, 0));
        searchResultController->initSearchResult(searchedString);

        //when
        searchResultController->addResult(searchedString, 10, utils::Direction::UP);
        searchResultController->addResult(searchedString, 5, utils::Direction::UP);
        searchResultController->addResult(searchedString, 2, utils::Direction::UP);
        searchResultController->addResult(searchedString, 20, utils::Direction::UP);

        searchResultController->addToSecondaryResults(searchedString, pair(3, 0));
        searchResultController->addToSecondaryResults(searchedString, pair(5, 0));
        searchResultController->addToSecondaryResults(searchedString, pair(20, 0));
        auto prevResult = searchResultController->getPrevSearchResult(searchedString, true);

        //then
        ASSERT_TRUE(prevResult.has_value());
        ASSERT_EQ(prevResult.value(), pair(10, 0));
        ASSERT_EQ((*searchResultController->getCurrInitial(searchedString)), 10);
        ASSERT_EQ((*searchResultController->getCurrSecondary(searchedString)), pair(20, 0));

        //when
        prevResult = searchResultController->getPrevSearchResult(searchedString, true);

        //then
        ASSERT_TRUE(prevResult.has_value());
        ASSERT_EQ(prevResult.value(), pair(5, 0));
        ASSERT_EQ((*searchResultController->getCurrInitial(searchedString)), 5);
        ASSERT_EQ((*searchResultController->getCurrSecondary(searchedString)), pair(20, 0));

        prevResult = searchResultController->getPrevSearchResult(searchedString, true);

        //then
        ASSERT_TRUE(prevResult.has_value());
        ASSERT_EQ(prevResult.value(), pair(5, 0));
        ASSERT_EQ((*searchResultController->getCurrInitial(searchedString)), 5);
        ASSERT_EQ((*searchResultController->getCurrSecondary(searchedString)), pair(5, 0));

        prevResult = searchResultController->getPrevSearchResult(searchedString, true);

        //then
        ASSERT_TRUE(prevResult.has_value());
        ASSERT_EQ(prevResult.value(), pair(3, 0));
        ASSERT_EQ((*searchResultController->getCurrInitial(searchedString)), 5);
        ASSERT_EQ((*searchResultController->getCurrSecondary(searchedString)), pair(3, 0));

        prevResult = searchResultController->getPrevSearchResult(searchedString, true);

        //then
        ASSERT_TRUE(prevResult.has_value());
        ASSERT_EQ(prevResult.value(), pair(2, 0));
        ASSERT_EQ((*searchResultController->getCurrInitial(searchedString)), 2);
        ASSERT_EQ((*searchResultController->getCurrSecondary(searchedString)), pair(3, 0));

        //when
        prevResult = searchResultController->getPrevSearchResult(searchedString, true);

        //then
        ASSERT_TRUE(prevResult.has_value());
        ASSERT_EQ(prevResult.value(), pair(20, 0));
        ASSERT_EQ((*searchResultController->getCurrInitial(searchedString)), 20);
        ASSERT_EQ((searchResultController->getCurrSecondary(searchedString)), (std::end(std::get<1>(searchResultController->results[searchedString]).first)));

        //when
        prevResult = searchResultController->getPrevSearchResult(searchedString, true);

        //then
        ASSERT_FALSE(prevResult.has_value());
        ASSERT_EQ((*searchResultController->getCurrInitial(searchedString)), 20);
        ASSERT_EQ((searchResultController->getCurrSecondary(searchedString)), (std::end(std::get<1>(searchResultController->results[searchedString]).first)));

        //when
        prevResult = searchResultController->getPrevSearchResult(searchedString, true);

        //then
        ASSERT_FALSE(prevResult.has_value());

        //when
        prevResult = searchResultController->getPrevSearchResult(searchedString, true);

        //then
        ASSERT_FALSE(prevResult.has_value());
    }

    TEST_F(SearchResultsControllerTest, getPrevSearchResult_2)
    {
        //before
        auto searchResultController = std::make_unique<SearchResultsController>(pair(11, 0));
        searchResultController->initSearchResult(searchedString);

        //when
        searchResultController->addResult(searchedString, 10, utils::Direction::UP);
        searchResultController->addResult(searchedString, 2, utils::Direction::UP);
        searchResultController->addResult(searchedString, 20, utils::Direction::UP);

        searchResultController->addToSecondaryResults(searchedString, pair(3, 0));
        auto prevResult = searchResultController->getPrevSearchResult(searchedString, true);

        //then
        ASSERT_TRUE(prevResult.has_value());
        ASSERT_EQ(prevResult.value(), pair(10, 0));
        ASSERT_EQ((*searchResultController->getCurrInitial(searchedString)), 10);
        ASSERT_EQ((searchResultController->getCurrSecondary(searchedString)), (std::end(std::get<1>(searchResultController->results[searchedString]).first)));

        //when
        prevResult = searchResultController->getPrevSearchResult(searchedString, true);

        //then
        ASSERT_TRUE(prevResult.has_value());
        ASSERT_EQ(prevResult.value(), pair(3, 0));
        ASSERT_EQ((*searchResultController->getCurrInitial(searchedString)), 10);
        ASSERT_EQ((*searchResultController->getCurrSecondary(searchedString)), pair(3, 0));

        //when
        prevResult = searchResultController->getPrevSearchResult(searchedString, true);

        //then
        ASSERT_TRUE(prevResult.has_value());
        ASSERT_EQ(prevResult.value(), pair(2, 0));
        ASSERT_EQ((*searchResultController->getCurrInitial(searchedString)), 2);
        ASSERT_EQ((*searchResultController->getCurrSecondary(searchedString)), pair(3, 0));

        //when
        prevResult = searchResultController->getPrevSearchResult(searchedString, true);

        //then
        ASSERT_TRUE(prevResult.has_value());
        ASSERT_EQ(prevResult.value(), pair(20, 0));
        ASSERT_EQ((*searchResultController->getCurrInitial(searchedString)), 20);
        ASSERT_EQ((searchResultController->getCurrSecondary(searchedString)), (std::end(std::get<1>(searchResultController->results[searchedString]).first)));

        //when
        prevResult = searchResultController->getPrevSearchResult(searchedString, true);

        //then
        ASSERT_FALSE(prevResult.has_value());
    }

    TEST_F(SearchResultsControllerTest, getNextSearchResult)
    {
        //before
        auto searchResultController = std::make_unique<SearchResultsController>(pair(11, 0));
        searchResultController->initSearchResult(searchedString);

        //when
        searchResultController->addResult(searchedString, 11, utils::Direction::DOWN);
        searchResultController->addResult(searchedString, 12, utils::Direction::DOWN);
        searchResultController->addResult(searchedString, 20, utils::Direction::DOWN);
        searchResultController->addResult(searchedString, 0, utils::Direction::DOWN);

        searchResultController->addToSecondaryResults(searchedString, pair(3, 0));
        searchResultController->addToSecondaryResults(searchedString, pair(5, 0));
        searchResultController->addToSecondaryResults(searchedString, pair(20, 0));
        auto nextResult = searchResultController->getNextSearchResult(searchedString);

        //then
        ASSERT_TRUE(nextResult.has_value());
        ASSERT_EQ(nextResult.value(), pair(11, 0));
        ASSERT_EQ((*searchResultController->getCurrInitial(searchedString)), 11);
        ASSERT_EQ((*searchResultController->getCurrSecondary(searchedString)), pair(5, 0));

        //when
        nextResult = searchResultController->getNextSearchResult(searchedString, true);

        //then
        ASSERT_TRUE(nextResult.has_value());
        ASSERT_EQ(nextResult.value(), pair(12, 0));
        ASSERT_EQ((*searchResultController->getCurrInitial(searchedString)), 12);
        ASSERT_EQ((*searchResultController->getCurrSecondary(searchedString)), pair(5, 0));

        nextResult = searchResultController->getNextSearchResult(searchedString, true);

        //then
        ASSERT_TRUE(nextResult.has_value());
        ASSERT_EQ(nextResult.value(), pair(20, 0));
        ASSERT_EQ((*searchResultController->getCurrInitial(searchedString)), 12);
        ASSERT_EQ((*searchResultController->getCurrSecondary(searchedString)), pair(20, 0));

        nextResult = searchResultController->getNextSearchResult(searchedString, true);

        //then
        ASSERT_TRUE(nextResult.has_value());
        ASSERT_EQ(nextResult.value(), pair(20, 0));
        ASSERT_EQ((*searchResultController->getCurrInitial(searchedString)), 20);
        ASSERT_EQ((*searchResultController->getCurrSecondary(searchedString)), pair(20, 0));

        nextResult = searchResultController->getNextSearchResult(searchedString, true);

        //then
        ASSERT_TRUE(nextResult.has_value());
        ASSERT_EQ(nextResult.value(), pair(0, 0));
        ASSERT_EQ((*searchResultController->getCurrInitial(searchedString)), 0);
        ASSERT_EQ((searchResultController->getCurrSecondary(searchedString)), (std::end(std::get<1>(searchResultController->results[searchedString]).first)));

        //when
        nextResult = searchResultController->getNextSearchResult(searchedString, true);

        //then
        ASSERT_FALSE(nextResult.has_value());
        ASSERT_EQ((*searchResultController->getCurrInitial(searchedString)), 0);
        ASSERT_EQ((searchResultController->getCurrSecondary(searchedString)), (std::end(std::get<1>(searchResultController->results[searchedString]).first)));
    }
    
    TEST_F(SearchResultsControllerTest, getPrevNextSearchResult)
    {
        //before
        auto searchResultController = std::make_unique<SearchResultsController>(pair(11, 0));
        searchResultController->initSearchResult(searchedString);

        //when
        searchResultController->addResult(searchedString, 11, utils::Direction::DOWN);
        searchResultController->addResult(searchedString, 12, utils::Direction::DOWN);
        searchResultController->addResult(searchedString, 20, utils::Direction::DOWN);
        searchResultController->addResult(searchedString, 0, utils::Direction::DOWN);

        searchResultController->addToSecondaryResults(searchedString, pair(3, 0));
        searchResultController->addToSecondaryResults(searchedString, pair(5, 0));
        searchResultController->addToSecondaryResults(searchedString, pair(20, 0));
        auto currResult = searchResultController->getNextSearchResult(searchedString);

        //then
        ASSERT_TRUE(nextResult.has_value());
        ASSERT_EQ(nextResult.value(), pair(11, 0));
        ASSERT_EQ((*searchResultController->getCurrInitial(searchedString)), 11);
        ASSERT_EQ((*searchResultController->getCurrSecondary(searchedString)), pair(5, 0));

        //when
        currResult = searchResultController->getNextSearchResult(searchedString, true);

        //then
        ASSERT_TRUE(nextResult.has_value());
        ASSERT_EQ(nextResult.value(), pair(12, 0));
        ASSERT_EQ((*searchResultController->getCurrInitial(searchedString)), 12);
        ASSERT_EQ((*searchResultController->getCurrSecondary(searchedString)), pair(5, 0));

        currResult = searchResultController->getNextSearchResult(searchedString, true);

        //then
        ASSERT_TRUE(nextResult.has_value());
        ASSERT_EQ(nextResult.value(), pair(20, 0));
        ASSERT_EQ((*searchResultController->getCurrInitial(searchedString)), 12);
        ASSERT_EQ((*searchResultController->getCurrSecondary(searchedString)), pair(20, 0));

        currResult = searchResultController->getNextSearchResult(searchedString, true);

        //then
        ASSERT_TRUE(nextResult.has_value());
        ASSERT_EQ(nextResult.value(), pair(20, 0));
        ASSERT_EQ((*searchResultController->getCurrInitial(searchedString)), 20);
        ASSERT_EQ((*searchResultController->getCurrSecondary(searchedString)), pair(20, 0));

        currResult = searchResultController->getNextSearchResult(searchedString, true);

        //then
        ASSERT_TRUE(nextResult.has_value());
        ASSERT_EQ(nextResult.value(), pair(0, 0));
        ASSERT_EQ((*searchResultController->getCurrInitial(searchedString)), 0);
        ASSERT_EQ((searchResultController->getCurrSecondary(searchedString)), (std::end(std::get<1>(searchResultController->results[searchedString]).first)));

        //when
        currResult = searchResultController->getNextSearchResult(searchedString, true);

        //then
        ASSERT_FALSE(nextResult.has_value());
        ASSERT_EQ((*searchResultController->getCurrInitial(searchedString)), 0);
        ASSERT_EQ((searchResultController->getCurrSecondary(searchedString)), (std::end(std::get<1>(searchResultController->results[searchedString]).first)));
    }
    */
}