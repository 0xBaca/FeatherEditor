#include "helpers/TestBase.hpp"
#include "mocks/BufferFillerMock.hpp"
#include "mocks/NCursesMainWindowDecoratorMock.hpp"
#include "mocks/NCursesBottomBarWindowDecoratorMock.hpp"
#include "mocks/PrintingOrchestratorMock.hpp"
#include "mocks/PrintingVisitorMock.hpp"
#include "mocks/WindowsManagerMock.hpp"
#include "printer/FilePrintingStorage.hpp"
#include "printer/PrintingOrchestrator.hpp"
#include "utils/ProgramOptionsParser.hpp"
#include "utils/algorithm/FastSearch.hpp"
#include "utils/datatypes/Strings.hpp"
#include "utils/helpers/Conversion.hpp"
#include "utils/storage/AbstractStorage.hpp"
#include "utils/storage/FileStorageFactory.hpp"
#include "utils/storage/InMemoryStorage.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace ::testing::mocks;

namespace feather::utils::algorithm
{
    class FastSearchIntegTest : public ::testing::Test, public ::feather::test::TestBase
    {
    protected:
        std::unique_ptr<utils::BufferFillerInterface> bufferFillerMock;
        std::shared_ptr<MainWindowInterface> nCursesMainWindowDecoratorMock;
        std::unique_ptr<feather::windows::SubWindowInterface> bottomBarWindowMock;
        std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator;
        std::unique_ptr<printer::PrintingVisitorInterface> printingVisitorMock, halfFrameUpVisitorMock;
        std::shared_ptr<feather::utils::storage::AbstractStorageFactory> storageFactory;
        std::shared_ptr<PrintingStorageInterface> filePrintingStorage;
        std::shared_ptr<WindowsManager> windowsManagerMock;
        utils::FEATHER_MODE currentFeatherMode;
        std::list<pair> breakPoints;
        std::list<pair>::iterator currentBreakPoint;
        std::set<pair> breakPointsAdded;

    public:
        void SetUp() override
        {
            bufferFillerMock = std::make_unique<BufferFillerMock>();
            nCursesMainWindowDecoratorMock = std::make_shared<NCursesMainWindowDecoratorMock>();
            bottomBarWindowMock = std::make_unique<NCursesBottomBarWindowDecoratorMock>();
            filePrintingStorage = std::make_shared<FilePrintingStorage>(inMemoryStorageFactory);
            printingOrchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, filePrintingStorage, nullptr, changesStack, filesystemMock);
            printingVisitorMock = std::unique_ptr<PrintingVisitorMock>(new PrintingVisitorMock());
            halfFrameUpVisitorMock = std::unique_ptr<PrintingVisitorMock>(new PrintingVisitorMock());
            storageFactory = std::make_shared<feather::utils::storage::FileStorageFactory>();
            windowsManagerMock = std::make_shared<WindowsManagerMock>();
            currentFeatherMode = utils::FEATHER_MODE::SEARCHING_IN_PROGRESS_DOWN;
            bottomBarWindowMock->setNewParent(nCursesMainWindowDecoratorMock);
            currentBreakPoint = breakPoints.end();
            EXPECT_CALL(*dynamic_cast<NCursesBottomBarWindowDecoratorMock *>(bottomBarWindowMock.get()), getParentWindowHandler())
                .WillRepeatedly(::testing::Return(nCursesMainWindowDecoratorMock));
        }

        void TearDown() override
        {
            printingOrchestrator->removeChangesRelatedToWindow(TEST_UUID);
            removeAllStorages();
            SearchResultsController::clearResults(TEST_UUID);
            SearchResultsController::clearResults(TEST_UUID_4);
        }
    };

    TEST_F(FastSearchIntegTest, searchOnEmptyStorage)
    {
        for (int i = 0; i < TEST_LOOP_COUNT; ++i)
        {
            printingOrchestrator->removeChangesRelatedToWindow(TEST_UUID);
            // before
            const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
            std::u32string searchedString(utils::helpers::Lambda::mapStringToWideString(std::string(TWO_BYTE_UTF8_CHARACTER.begin(), TWO_BYTE_UTF8_CHARACTER.end())));
            utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
            configuration->overrideConfiguration();

            std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createEmptyStorage("getFirstSearchIntervalEmptyStorage.fea");
            EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
                .WillRepeatedly(::testing::ReturnRef(storage));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getStorage())
                .WillRepeatedly(::testing::ReturnRef(storage));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getUUID(false))
                .WillRepeatedly(::testing::ReturnRef(TEST_UUID));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getCursorRealPosition(::testing::_))
                .WillRepeatedly(::testing::Return(pair(0, 0)));
            std::unique_ptr<FastSearch> fastSearch = std::make_unique<FastSearch>(TEST_UUID, std::u32string(searchedString), fileStorageFactory, inMemoryStorageFactory, printingOrchestrator, bufferFillerMock, pair(0, 0), breakPoints, currentBreakPoint, breakPointsAdded);

            // when
            auto result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

            // then
            ASSERT_FALSE(result.first.has_value());

            // when
            printingOrchestrator->addChanges(TEST_UUID, storage, pair(0, 0), TWO_BYTE_UTF8_CHARACTER, 2UL);
            result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::UP);

            // then
            ASSERT_TRUE(result.first.has_value());
            ASSERT_EQ(result.first.value(), std::make_pair(pair(0UL, 0UL), 1UL));
            ASSERT_EQ(result.second, utils::datatypes::ERROR::NONE);
        }
    }

    TEST_F(FastSearchIntegTest, nextChangeWhenInTheMiddleOfChanges)
    {
        // before
        printingOrchestrator->removeChangesRelatedToWindow(TEST_UUID);
        utils::Direction searchDirection = utils::Direction::UP;
        const char *commandLineArguments[] = {"./feather", "file", "--relax=124", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4UL, commandLineArguments);
        configuration->overrideConfiguration();
        std::u32string searchedString(utils::helpers::Lambda::mapStringToWideString(std::string(TWO_BYTE_UTF8_CHARACTER.begin(), TWO_BYTE_UTF8_CHARACTER.end())));

        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("nextChangeWhenInTheMiddleOfChanges.fea", FIVE_BYTE_UTF8_CHARACTER);
        EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
            .WillRepeatedly(::testing::ReturnRef(storage));
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getStorage())
            .WillRepeatedly(::testing::ReturnRef(storage));
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getUUID(false))
            .WillRepeatedly(::testing::ReturnRef(TEST_UUID));
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getCursorRealPosition(::testing::_))
            .WillRepeatedly(::testing::Return(pair(0, 0)));

        std::unique_ptr<FastSearch> fastSearch = std::make_unique<FastSearch>(TEST_UUID, std::u32string(searchedString), fileStorageFactory, inMemoryStorageFactory, printingOrchestrator, bufferFillerMock, pair(0, 0), breakPoints, currentBreakPoint, breakPointsAdded);

        // when
        printingOrchestrator->addChanges(TEST_UUID, storage, pair(5, 0), TEN_BYTE_UTF8_CHARACTER, 10UL);
        printingOrchestrator->removeCharacters(TEST_UUID, storage, pair(5, 6), pair(5, 10));
        auto result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, searchDirection);

        // then
        ASSERT_TRUE(result.first.has_value());
        ASSERT_EQ(result.first.value(), std::make_pair(pair(3UL, 0UL), 2UL));
        ASSERT_EQ(result.second, utils::datatypes::ERROR::NONE);

        // when
        result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, searchDirection);

        // then
        ASSERT_TRUE(result.first.has_value());
        ASSERT_EQ(result.first.value(), std::make_pair(pair(1UL, 0UL), 2UL));
    }

    TEST_F(FastSearchIntegTest, searchBetweenNewChanges)
    {
        for (int i = 0; i < 100; ++i)
        {
            // before
            printingOrchestrator->removeChangesRelatedToWindow(TEST_UUID);
            utils::Direction searchDirection = utils::Direction::DOWN;
            std::u32string searchedString(utils::helpers::Lambda::mapStringToWideString(std::string(TWO_BYTE_UTF8_CHARACTER.begin(), TWO_BYTE_UTF8_CHARACTER.end())));
            const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
            utils::ProgramOptionsParser::parseArguments(4UL, commandLineArguments);
            configuration->overrideConfiguration();

            std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("searchBetweenNewChanges.fea", TWENTY_BYTE_UTF8_CHARACTER);
            EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
                .WillRepeatedly(::testing::ReturnRef(storage));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getStorage())
                .WillRepeatedly(::testing::ReturnRef(storage));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getUUID(false))
                .WillRepeatedly(::testing::ReturnRef(TEST_UUID));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getCursorRealPosition(::testing::_))
                .WillRepeatedly(::testing::Return(pair(0, 0)));
            std::unique_ptr<FastSearch> fastSearch = std::make_unique<FastSearch>(TEST_UUID, std::u32string(searchedString), fileStorageFactory, inMemoryStorageFactory, printingOrchestrator, bufferFillerMock, pair(0, 0), breakPoints, currentBreakPoint, breakPointsAdded);

            // when
            auto result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, searchDirection);

            // then
            ASSERT_FALSE(result.first.has_value());

            // when
            printingOrchestrator->addChanges(TEST_UUID, storage, pair(2, 0), TWO_BYTE_UTF8_CHARACTER, 2UL);
            result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, searchDirection);

            // then
            ASSERT_TRUE(result.first.has_value());
            ASSERT_EQ(result.first.value(), std::make_pair(pair(2UL, 0UL), 1UL));
            ASSERT_EQ(result.second, utils::datatypes::ERROR::NONE);

            // when
            printingOrchestrator->removeCharacters(TEST_UUID, storage, pair(2UL, 0UL), pair(6, 0));
            result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, searchDirection);

            // then
            ASSERT_FALSE(result.first.has_value());

            // when
            printingOrchestrator->addChanges(TEST_UUID, storage, pair(20, 0), TWO_BYTE_UTF8_CHARACTER, 2UL);
            result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, searchDirection);

            // then
            ASSERT_TRUE(result.first.has_value());
            ASSERT_EQ(result.first.value(), std::make_pair(pair(20UL, 0UL), 1UL));
            ASSERT_EQ(result.second, utils::datatypes::ERROR::NONE);
        }
    }
/*
    TEST_F(FastSearchIntegTest, searchBetweenNewChanges_2)
    {
        for (int i = 0; i < 100; ++i)
        {
            // before
            printingOrchestrator->removeChangesRelatedToWindow(TEST_UUID);
            utils::Direction searchDirection = utils::Direction::DOWN;
            std::vector<char> UTF8_STRING = {(char)0xD1, (char)0x81, (char)0xD0, (char)0xBE, (char)0xD0, (char)0xB1, (char)0xD0, (char)0xB5, (char)0xD1, (char)0x80, (char)0xD0, (char)0xB5, (char)0xD1, (char)0x82};
            std::u32string searchedString(utils::helpers::Lambda::mapStringToWideString(std::string(UTF8_STRING.begin(), UTF8_STRING.end())));
            const char *commandLineArguments[] = {"./feather", "file", "--relax=20", "--mode=text"};
            utils::ProgramOptionsParser::parseArguments(4UL, commandLineArguments);
            configuration->overrideConfiguration();

            std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("searchBetweenNewChanges_2.fea", std::make_shared<feather::utils::storage::FileStorage>("./tst/ut/helpers/testfiles/Utf8TestFile"));
            EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
                .WillRepeatedly(::testing::ReturnRef(storage));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getStorage())
                .WillRepeatedly(::testing::ReturnRef(storage));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getUUID(false))
                .WillRepeatedly(::testing::ReturnRef(TEST_UUID));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getCursorRealPosition(::testing::_))
                .WillRepeatedly(::testing::Return(pair(0, 0)));
            std::unique_ptr<FastSearch> fastSearch = std::make_unique<FastSearch>(TEST_UUID, std::u32string(searchedString), fileStorageFactory, inMemoryStorageFactory, printingOrchestrator, bufferFillerMock, pair(0, 0), breakPoints, currentBreakPoint, breakPointsAdded);

            // when
            printingOrchestrator->removeCharacters(TEST_UUID, storage, pair(6036, 0), pair(6039, 0));
            auto result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, searchDirection);

            // then
            ASSERT_TRUE(result.first.has_value());
            ASSERT_EQ(result.first.value(), std::make_pair(pair(6776UL, 0UL), 14UL));
            ASSERT_EQ(result.second, utils::datatypes::ERROR::NONE);

            // when
            result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, searchDirection);

            // then
            ASSERT_TRUE(result.first.has_value());
            ASSERT_EQ(result.first.value(), std::make_pair(pair(6776UL, 0UL), 14UL));
            ASSERT_EQ(result.second, utils::datatypes::ERROR::NONE);
        }
    }
*/
    TEST_F(FastSearchIntegTest, searchBetweenNewChanges_3)
    {
        for (int i = 0; i < 100; ++i)
        {
            // before
            printingOrchestrator->removeChangesRelatedToWindow(TEST_UUID);
            utils::Direction searchDirection = utils::Direction::UP;
            std::u32string searchedString(utils::helpers::Lambda::mapStringToWideString(std::string(FOUR_BYTE_UTF8_CHARACTER.begin(), FOUR_BYTE_UTF8_CHARACTER.end())));
            const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
            utils::ProgramOptionsParser::parseArguments(4UL, commandLineArguments);
            configuration->overrideConfiguration();

            std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("searchBetweenNewChanges_3.fea", std::make_shared<feather::utils::storage::FileStorage>("./tst/ut/helpers/testfiles/Utf8TestFile"));
            EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
                .WillRepeatedly(::testing::ReturnRef(storage));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getStorage())
                .WillRepeatedly(::testing::ReturnRef(storage));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getUUID(false))
                .WillRepeatedly(::testing::ReturnRef(TEST_UUID));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getCursorRealPosition(::testing::_))
                .WillRepeatedly(::testing::Return(pair(0, 0)));
            std::unique_ptr<FastSearch> fastSearch = std::make_unique<FastSearch>(TEST_UUID, std::u32string(searchedString), fileStorageFactory, inMemoryStorageFactory, printingOrchestrator, bufferFillerMock, pair(0, 0), breakPoints, currentBreakPoint, breakPointsAdded);

            // when
            printingOrchestrator->addChanges(TEST_UUID, storage, pair(6036UL, 0UL), TWENTY_BYTE_UTF8_CHARACTER, 20UL);
            printingOrchestrator->removeCharacters(TEST_UUID, storage, pair(6036UL, 2UL), pair(6036UL, 6UL));
            auto result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, searchDirection);

            // then
            ASSERT_TRUE(result.first.has_value());
            ASSERT_EQ(result.first.value(), std::make_pair(pair(6036UL, 16UL), 1UL));
            ASSERT_EQ(result.second, utils::datatypes::ERROR::NONE);

            // when
            result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, searchDirection);

            // then
            ASSERT_TRUE(result.first.has_value());
            ASSERT_EQ(result.first.value(), std::make_pair(pair(6036UL, 12UL), 1UL));
            ASSERT_EQ(result.second, utils::datatypes::ERROR::NONE);

            // when
            result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, searchDirection);

            // then
            ASSERT_TRUE(result.first.has_value());
            ASSERT_EQ(result.first.value(), std::make_pair(pair(6036UL, 6UL), 1UL));
            ASSERT_EQ(result.second, utils::datatypes::ERROR::NONE);
        }
    }

    TEST_F(FastSearchIntegTest, searchBetweenNewChanges_4)
    {
        for (int i = 0; i < 100; ++i)
        {
            // before
            printingOrchestrator->removeChangesRelatedToWindow(TEST_UUID);
            utils::Direction searchDirection = utils::Direction::UP;
            std::u32string searchedString(utils::helpers::Lambda::mapStringToWideString(std::string(THREE_BYTE_UTF8_CHARACTER.begin(), THREE_BYTE_UTF8_CHARACTER.end())));
            const char *commandLineArguments[] = {"./feather", "file", "--relax=5", "--mode=text"};
            utils::ProgramOptionsParser::parseArguments(4UL, commandLineArguments);
            configuration->overrideConfiguration();

            std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("searchBetweenNewChanges_4.fea", FIVE_BYTE_UTF8_CHARACTER);
            EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
                .WillRepeatedly(::testing::ReturnRef(storage));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getStorage())
                .WillRepeatedly(::testing::ReturnRef(storage));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getUUID(false))
                .WillRepeatedly(::testing::ReturnRef(TEST_UUID));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getCursorRealPosition(::testing::_))
                .WillRepeatedly(::testing::Return(pair(0, 0)));
            std::unique_ptr<FastSearch> fastSearch = std::make_unique<FastSearch>(TEST_UUID, std::u32string(searchedString), fileStorageFactory, inMemoryStorageFactory, printingOrchestrator, bufferFillerMock, pair(0, 0), breakPoints, currentBreakPoint, breakPointsAdded);

            // t 0xc5 0x9b 0xc5 0x9b
            //(t 0xc5 0x9b) t 0xc5 0x9b 0xc5 0x9b
            // removed t (0xc5 0x9b t) 0xc5 0x9b 0xc5 0x9b
            // when
            auto cc = printingOrchestrator->getContinousCharacters(pair(0, 0), 20, storage, TEST_UUID);

            printingOrchestrator->addChanges(TEST_UUID, storage, pair(0, 0), THREE_BYTE_UTF8_CHARACTER, 3UL);
            printingOrchestrator->removeCharacters(TEST_UUID, storage, pair(0, 1UL), pair(1UL, 0));
            auto result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, searchDirection);

            // then
            ASSERT_TRUE(result.first.has_value());
            ASSERT_EQ(result.first.value(), std::make_pair(pair(0UL, 0UL), 2UL));
            ASSERT_EQ(result.second, utils::datatypes::ERROR::NONE);

            // when
            printingOrchestrator->addChanges(TEST_UUID, storage, pair(3UL, 0), std::vector<char>{U't'}, 1UL);
            result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, searchDirection);

            // when
            result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, searchDirection);

            // then
            ASSERT_TRUE(result.first.has_value());
            ASSERT_EQ(result.first.value(), std::make_pair(pair(0UL, 0UL), 2UL));
            ASSERT_EQ(result.second, utils::datatypes::ERROR::NONE);

            // when
            result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, searchDirection);

            // then
            ASSERT_TRUE(result.first.has_value());
            ASSERT_EQ(result.first.value(), std::make_pair(pair(3UL, 0UL), 2UL));
            ASSERT_EQ(result.second, utils::datatypes::ERROR::NONE);
        }
    }

    TEST_F(FastSearchIntegTest, getNextSearchResult)
    {
        for (int i = 0; i < TEST_LOOP_COUNT; ++i)
        {
            // before
            printingOrchestrator->removeChangesRelatedToWindow(TEST_UUID);
            utils::Direction searchDirection = utils::Direction::DOWN;
            std::u32string searchedString{U'U', U'n'};
            const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
            utils::ProgramOptionsParser::parseArguments(4UL, commandLineArguments);
            configuration->overrideConfiguration();

            std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("getNextSearchResult.fea", std::make_shared<feather::utils::storage::FileStorage>("./tst/ut/helpers/testfiles/Utf8TestFile"));
            EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
                .WillRepeatedly(::testing::ReturnRef(storage));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getStorage())
                .WillRepeatedly(::testing::ReturnRef(storage));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getUUID(false))
                .WillRepeatedly(::testing::ReturnRef(TEST_UUID));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getCursorRealPosition(::testing::_))
                .WillRepeatedly(::testing::Return(pair(0, 0)));

            std::unique_ptr<FastSearch> fastSearch = std::make_unique<FastSearch>(TEST_UUID, std::u32string(searchedString), fileStorageFactory, inMemoryStorageFactory, printingOrchestrator, bufferFillerMock, pair(0, 0), breakPoints, currentBreakPoint, breakPointsAdded);

            // when
            printingOrchestrator->addChanges(TEST_UUID, storage, pair(2, 0), {0x10}, 1UL);
            auto result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, searchDirection);

            // then
            ASSERT_TRUE(result.first.has_value());
            ASSERT_EQ(result.first.value(), std::make_pair(pair(325UL, 0UL), 2UL));
            ASSERT_EQ(result.second, utils::datatypes::ERROR::NONE);

            result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, searchDirection);

            // then
            ASSERT_TRUE(result.first.has_value());
            ASSERT_EQ(result.first.value(), std::make_pair(pair(409UL, 0UL), 2UL));
            ASSERT_EQ(result.second, utils::datatypes::ERROR::NONE);

            result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, searchDirection);

            // then
            ASSERT_TRUE(result.first.has_value());
            ASSERT_EQ(result.first.value(), std::make_pair(pair(5311UL, 0UL), 2UL));
            ASSERT_EQ(result.second, utils::datatypes::ERROR::NONE);

            result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, searchDirection);

            // then
            ASSERT_TRUE(result.first.has_value());
            ASSERT_EQ(result.first.value(), std::make_pair(pair(5439UL, 0UL), 2UL));
            ASSERT_EQ(result.second, utils::datatypes::ERROR::NONE);

            result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, searchDirection);

            // then
            ASSERT_TRUE(result.first.has_value());
            ASSERT_EQ(result.first.value(), std::make_pair(pair(5956UL, 0UL), 2UL));
            ASSERT_EQ(result.second, utils::datatypes::ERROR::NONE);

            result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, searchDirection);

            // then
            ASSERT_TRUE(result.first.has_value());
            ASSERT_EQ(result.first.value(), std::make_pair(pair(6071UL, 0UL), 2UL));
            ASSERT_EQ(result.second, utils::datatypes::ERROR::NONE);

            result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, searchDirection);

            // then
            ASSERT_TRUE(result.first.has_value());
            ASSERT_EQ(result.first.value(), std::make_pair(pair(6481UL, 0UL), 2UL));
            ASSERT_EQ(result.second, utils::datatypes::ERROR::NONE);

            result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, searchDirection);

            // then
            ASSERT_TRUE(result.first.has_value());
            ASSERT_EQ(result.first.value(), std::make_pair(pair(6640UL, 0UL), 2UL));
            ASSERT_EQ(result.second, utils::datatypes::ERROR::NONE);

            result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, searchDirection);

            // then
            ASSERT_TRUE(result.first.has_value());
            ASSERT_EQ(result.first.value(), std::make_pair(pair(6904UL, 0UL), 2UL));
            ASSERT_EQ(result.second, utils::datatypes::ERROR::NONE);

            result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, searchDirection);

            // then
            ASSERT_TRUE(result.first.has_value());
            ASSERT_EQ(result.first.value(), std::make_pair(pair(7026UL, 0UL), 2UL));
            ASSERT_EQ(result.second, utils::datatypes::ERROR::NONE);
        }
    }

    TEST_F(FastSearchIntegTest, getPreviousSearchResult)
    {
        // before
        utils::Direction searchDirection = utils::Direction::UP;
        std::u32string searchedString{U'F', U'\n'};
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4UL, commandLineArguments);
        configuration->overrideConfiguration();

        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("getNextSearchResult.fea", {(char)0x10, (char)0x4A, (char)0x46, (char)0x49, (char)0x46, (char)0xC3, (char)0x9F, (char)0x00, (char)0x01, (char)0x01, (char)0x0A});
        EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
            .WillRepeatedly(::testing::ReturnRef(storage));
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getStorage())
            .WillRepeatedly(::testing::ReturnRef(storage));
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getUUID(false))
            .WillRepeatedly(::testing::ReturnRef(TEST_UUID));
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getCursorRealPosition(::testing::_))
            .WillRepeatedly(::testing::Return(pair(0, 0)));

        std::unique_ptr<FastSearch> fastSearch = std::make_unique<FastSearch>(TEST_UUID, std::u32string(searchedString), fileStorageFactory, inMemoryStorageFactory, printingOrchestrator, bufferFillerMock, pair(0, 0), breakPoints, currentBreakPoint, breakPointsAdded);

        // when
        printingOrchestrator->addChanges(TEST_UUID, storage, pair(3, 0), {0xA}, 1UL);
        auto result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, searchDirection);

        // then
        ASSERT_TRUE(result.first.has_value());
        ASSERT_EQ(result.first.value(), std::make_pair(std::pair(2UL, 0UL), 2UL));
        ASSERT_EQ(result.second, utils::datatypes::ERROR::NONE);
    }

    TEST_F(FastSearchIntegTest, getNextAndPreviousWhenStartingInMiddleOfSearchedStringSearchResult)
    {
        // before
        printingOrchestrator->removeChangesRelatedToWindow(TEST_UUID);
        std::u32string searchedString{0x4A, 0x46, 0x49};
        auto flatteredSearchedString = utils::helpers::Conversion::squeezeu32String(searchedString);
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text", "--storage=./tst/ut/helpers/testfiles"};
        utils::ProgramOptionsParser::parseArguments(5UL, commandLineArguments);
        configuration->overrideConfiguration();

        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("getNextAndPreviousWhenStartingInMiddleOfSearchedStringSearchResult.fea", {(char)0x10, (char)0x4A, (char)0x46, (char)0x49, (char)0x46, (char)0xC3, (char)0x9F, (char)0x00, (char)0x4A, (char)0x46, (char)0x49, (char)0x01, (char)0x01, (char)0x0A});
        EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
            .WillRepeatedly(::testing::ReturnRef(storage));
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getStorage())
            .WillRepeatedly(::testing::ReturnRef(storage));
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getUUID(false))
            .WillRepeatedly(::testing::ReturnRef(TEST_UUID));
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getCursorRealPosition(::testing::_))
            .WillRepeatedly(::testing::Return(pair(9UL, 0)));
        std::unique_ptr<FastSearch> fastSearch = std::make_unique<FastSearch>(TEST_UUID, std::u32string(searchedString), fileStorageFactory, inMemoryStorageFactory, printingOrchestrator, bufferFillerMock, pair(9UL, 0), breakPoints, currentBreakPoint, breakPointsAdded);

        // when
        auto result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

        // then
        ASSERT_TRUE(result.first.has_value());
        ASSERT_EQ(result.first.value(), std::make_pair(std::pair(1UL, 0UL), 3UL));
        ASSERT_EQ(result.second, utils::datatypes::ERROR::NONE);

        // when
        result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

        // then
        ASSERT_TRUE(result.first.has_value());
        ASSERT_EQ(result.first.value(), std::make_pair(std::pair(8UL, 0UL), 3UL));
        ASSERT_EQ(result.second, utils::datatypes::ERROR::NONE);

        // when
        result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::UP);

        // then
        ASSERT_TRUE(result.first.has_value());
        ASSERT_EQ(result.first.value(), std::make_pair(std::pair(1UL, 0UL), 3UL));
        ASSERT_EQ(result.second, utils::datatypes::ERROR::NONE);

        // when
        result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::UP);

        // then
        ASSERT_TRUE(result.first.has_value());
        ASSERT_EQ(result.first.value(), std::make_pair(std::pair(8UL, 0UL), 3UL));
        ASSERT_EQ(result.second, utils::datatypes::ERROR::NONE);

        // when
        result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

        // then
        ASSERT_TRUE(result.first.has_value());
        ASSERT_EQ(result.first.value(), std::make_pair(std::pair(1UL, 0UL), 3UL));
        ASSERT_EQ(result.second, utils::datatypes::ERROR::NONE);

        // when
        result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

        // then
        ASSERT_TRUE(result.first.has_value());
        ASSERT_EQ(result.first.value(), std::make_pair(std::pair(8UL, 0UL), 3UL));
        ASSERT_EQ(result.second, utils::datatypes::ERROR::NONE);
    }

    TEST_F(FastSearchIntegTest, getNextAndPreviousSearchResultWithOneResult)
    {
        for (int i = 0; i < 100; ++i)
        {
            // before
            printingOrchestrator->removeChangesRelatedToWindow(TEST_UUID);
            std::u32string searchedString{0x01, 0x0A};
            auto flatteredSearchedString = utils::helpers::Conversion::squeezeu32String(searchedString);
            const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text", "--storage=./tst/ut/helpers/testfiles"};
            utils::ProgramOptionsParser::parseArguments(5UL, commandLineArguments);
            configuration->overrideConfiguration();

            std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("getNextAndPreviousSearchResult.fea", {(char)0x10, (char)0x4A, (char)0x46, (char)0x49, (char)0x46, (char)0xC3, (char)0x9F, (char)0x00, (char)0x01, (char)0x01, (char)0x0A});
            EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
                .WillRepeatedly(::testing::ReturnRef(storage));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getStorage())
                .WillRepeatedly(::testing::ReturnRef(storage));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getUUID(false))
                .WillRepeatedly(::testing::ReturnRef(TEST_UUID));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getCursorRealPosition(::testing::_))
                .WillRepeatedly(::testing::Return(pair(9UL, 0)));
            std::unique_ptr<FastSearch> fastSearch = std::make_unique<FastSearch>(TEST_UUID, std::u32string(searchedString), fileStorageFactory, inMemoryStorageFactory, printingOrchestrator, bufferFillerMock, pair(9UL, 0), breakPoints, currentBreakPoint, breakPointsAdded);

            // when
            auto result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

            // then
            ASSERT_TRUE(result.first.has_value());
            ASSERT_EQ(result.first.value(), std::make_pair(std::pair(9UL, 0UL), 2UL));
            ASSERT_EQ(result.second, utils::datatypes::ERROR::NONE);

            // when
            result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

            // then
            ASSERT_TRUE(result.first.has_value());
            ASSERT_EQ(result.first.value(), std::make_pair(std::pair(9UL, 0UL), 2UL));
            ASSERT_EQ(result.second, utils::datatypes::ERROR::NONE);

            // when
            printingOrchestrator->addChanges(TEST_UUID, storage, pair(10, 0), {0xB}, 1UL);
            result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

            // then
            ASSERT_FALSE(result.first.has_value());

            // when
            printingOrchestrator->removeCharacters(TEST_UUID, storage, pair(0, 0UL), pair(11UL, 0));
            result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

            // then
            ASSERT_FALSE(result.first.has_value());

            // when
            result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

            // then
            ASSERT_FALSE(result.first.has_value());

            // when
            printingOrchestrator->addChanges(TEST_UUID, storage, pair(11, 0), {0x01, 0x0A}, 2UL);
            result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

            // then
            ASSERT_TRUE(result.first.has_value());
            ASSERT_EQ(result.first.value(), std::make_pair(std::pair(11UL, 0UL), 2UL));
            ASSERT_EQ(result.second, utils::datatypes::ERROR::NONE);
        }
    }

    TEST_F(FastSearchIntegTest, getNextMultipleTimes)
    {
        for (int i = 0; i < TEST_LOOP_COUNT; ++i)
        {
            printingOrchestrator->removeChangesRelatedToWindow(TEST_UUID);
            // before
            std::u32string searchedString{0x10};
            auto flatteredSearchedString = utils::helpers::Conversion::squeezeu32String(searchedString);
            const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text", "--storage=./tst/ut/helpers/testfiles"};
            utils::ProgramOptionsParser::parseArguments(5UL, commandLineArguments);
            configuration->overrideConfiguration();

            std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("getNextAndThenPreviousFromSearchOnly.fea", {(char)0x10, (char)0x4A, (char)0x46, (char)0x49, (char)0x46, (char)0xC3, (char)0x9F, (char)0x00, (char)0x01, (char)0x01, (char)0x0A});
            EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
                .WillRepeatedly(::testing::ReturnRef(storage));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getStorage())
                .WillRepeatedly(::testing::ReturnRef(storage));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getUUID(false))
                .WillRepeatedly(::testing::ReturnRef(TEST_UUID));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getCursorRealPosition(::testing::_))
                .WillRepeatedly(::testing::Return(pair(9, 0)));

            std::unique_ptr<FastSearch> fastSearch = std::make_unique<FastSearch>(TEST_UUID, std::u32string(searchedString), fileStorageFactory, inMemoryStorageFactory, printingOrchestrator, bufferFillerMock, pair(9UL, 0), breakPoints, currentBreakPoint, breakPointsAdded);

            // when
            auto result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

            // then
            ASSERT_TRUE(result.first.has_value());
            ASSERT_EQ(result.first.value(), std::make_pair(std::pair(0UL, 0UL), 1UL));
            ASSERT_EQ(result.second, utils::datatypes::ERROR::NONE);

            // when
            result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

            // then
            ASSERT_TRUE(result.first.has_value());
            ASSERT_EQ(result.first.value(), std::make_pair(std::pair(0UL, 0UL), 1UL));
            ASSERT_EQ(result.second, utils::datatypes::ERROR::NONE);

            // when
            result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

            // then
            ASSERT_TRUE(result.first.has_value());
            ASSERT_EQ(result.first.value(), std::make_pair(std::pair(0UL, 0UL), 1UL));
            ASSERT_EQ(result.second, utils::datatypes::ERROR::NONE);
        }
    }

    TEST_F(FastSearchIntegTest, resetResultsIfFileWasModified)
    {
        // before
        std::u32string searchedString{0x01, 0x01};
        auto flatteredSearchedString = utils::helpers::Conversion::squeezeu32String(searchedString);
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text", "--storage=./tst/ut/helpers/testfiles"};
        utils::ProgramOptionsParser::parseArguments(5UL, commandLineArguments);
        configuration->overrideConfiguration();

        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("getNextAndPreviousSearchResult.fea", {(char)0x10, (char)0x4A, (char)0x46, (char)0x49, (char)0x46, (char)0xC3, (char)0x9F, (char)0x00, (char)0x01, (char)0x01, (char)0x0A});
        EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
            .WillRepeatedly(::testing::ReturnRef(storage));
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getStorage())
            .WillRepeatedly(::testing::ReturnRef(storage));
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getUUID(false))
            .WillRepeatedly(::testing::ReturnRef(TEST_UUID));
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getCursorRealPosition(::testing::_))
            .WillRepeatedly(::testing::Return(pair(8, 0)));
        std::unique_ptr<FastSearch> fastSearch = std::make_unique<FastSearch>(TEST_UUID, std::u32string(searchedString), fileStorageFactory, inMemoryStorageFactory, printingOrchestrator, bufferFillerMock, pair(9UL, 0), breakPoints, currentBreakPoint, breakPointsAdded);

        // when
        auto result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

        // then
        ASSERT_TRUE(result.first.has_value());
        ASSERT_EQ(result.first.value(), std::make_pair(std::pair(8UL, 0UL), 2UL));
        ASSERT_EQ(result.second, utils::datatypes::ERROR::NONE);

        // when
        static_cast<utils::storage::InMemoryStorage *>(storage.get())->mockReloadStorageContent({(char)0x10, (char)0x4A, (char)0x46, (char)0x49, (char)0x46, (char)0xC3, (char)0x9F, (char)0x00, (char)0x01, (char)0x01, (char)0x0A, (char)0x01, (char)0x01});
        static_cast<utils::storage::InMemoryStorage *>(fastSearch->fromCursorStorage.get())->mockReloadStorageContent({(char)0x10, (char)0x4A, (char)0x46, (char)0x49, (char)0x46, (char)0xC3, (char)0x9F, (char)0x00, (char)0x01, (char)0x01, (char)0x0A, (char)0x01, (char)0x01});
        static_cast<utils::storage::InMemoryStorage *>(fastSearch->toCursorStorage.get())->mockReloadStorageContent({(char)0x10, (char)0x4A, (char)0x46, (char)0x49, (char)0x46, (char)0xC3, (char)0x9F, (char)0x00, (char)0x01, (char)0x01, (char)0x0A, (char)0x01, (char)0x01});
        result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

        // then
        ASSERT_TRUE(result.first.has_value());
        ASSERT_EQ(result.first.value(), std::make_pair(std::pair(11UL, 0UL), 2UL));
        ASSERT_EQ(result.second, utils::datatypes::ERROR::NONE);

        // when
        result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

        // then
        ASSERT_TRUE(result.first.has_value());
        ASSERT_EQ(result.first.value(), std::make_pair(std::pair(8UL, 0UL), 2UL));
        ASSERT_EQ(result.second, utils::datatypes::ERROR::NONE);

        // when
        printingOrchestrator->removeCharacters(TEST_UUID, storage, pair(0, 0UL), pair(14UL, 0UL));
        result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

        // then
        ASSERT_FALSE(result.first.has_value());
    }

    TEST_F(FastSearchIntegTest, getNextMatchFromResults)
    {
        for (int i = 0; i < TEST_LOOP_COUNT; ++i)
        {
            printingOrchestrator->removeChangesRelatedToWindow(TEST_UUID);
            // before
            std::u32string searchedString{U'U', U'T', U'F', U'-', U'8'};
            const char *commandLineArguments[] = {"./feather", "file", "--relax=10", "--mode=text"};
            utils::ProgramOptionsParser::parseArguments(4UL, commandLineArguments);
            configuration->overrideConfiguration();

            std::shared_ptr<feather::utils::storage::AbstractStorage> storageFromFile = storageFactory->getStorage("./tst/ut/helpers/testfiles/Utf8TestFile", TEST_UUID_4);
            std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("getNextMatchFromResults.fea", storageFromFile);

            EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
                .WillRepeatedly(::testing::ReturnRef(storage));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getStorage())
                .WillRepeatedly(::testing::ReturnRef(storage));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getUUID(false))
                .WillRepeatedly(::testing::ReturnRef(TEST_UUID));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getCursorRealPosition(::testing::_))
                .WillRepeatedly(::testing::Return(pair(8, 0)));
            std::unique_ptr<FastSearch> fastSearch = std::make_unique<FastSearch>(TEST_UUID_4, std::u32string(searchedString), fileStorageFactory, inMemoryStorageFactory, printingOrchestrator, bufferFillerMock, pair(0, 0), breakPoints, currentBreakPoint, breakPointsAdded);

            // when
            auto nextMatch = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

            // then
            ASSERT_TRUE(nextMatch.first.has_value());
            ASSERT_EQ(nextMatch.first.value(), std::make_pair(pair(61UL, 0), 5UL));

            // when
            nextMatch = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

            // then
            ASSERT_TRUE(nextMatch.first.has_value());
            ASSERT_EQ(nextMatch.first.value(), std::make_pair(pair(293UL, 0), 5UL));

            // when
            nextMatch = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

            // then
            ASSERT_TRUE(nextMatch.first.has_value());
            ASSERT_EQ(nextMatch.first.value(), std::make_pair(pair(417UL, 0), 5UL));

            // when
            nextMatch = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

            // then
            ASSERT_TRUE(nextMatch.first.has_value());
            ASSERT_EQ(nextMatch.first.value(), std::make_pair(pair(61UL, 0), 5UL));
        }
    }

    TEST_F(FastSearchIntegTest, getNextMatchToResults)
    {
        for (int i = 0; i < TEST_LOOP_COUNT; ++i)
        {
            // before
            std::u32string searchedString{U'U', U'T', U'F', U'-', U'8'};
            const char *commandLineArguments[] = {"./feather", "file", "--relax=10", "--mode=text"};
            utils::ProgramOptionsParser::parseArguments(4UL, commandLineArguments);
            configuration->overrideConfiguration();

            std::shared_ptr<feather::utils::storage::AbstractStorage> storageFromFile = storageFactory->getStorage("./tst/ut/helpers/testfiles/Utf8TestFile", TEST_UUID_4);
            std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("getNextMatchToResults.fea", storageFromFile);

            EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
                .WillRepeatedly(::testing::ReturnRef(storage));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getStorage())
                .WillRepeatedly(::testing::ReturnRef(storage));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getUUID(false))
                .WillRepeatedly(::testing::ReturnRef(TEST_UUID));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getCursorRealPosition(::testing::_))
                .WillRepeatedly(::testing::Return(pair(8, 0)));

            std::unique_ptr<FastSearch> fastSearch = std::make_unique<FastSearch>(TEST_UUID_4, std::u32string(searchedString), fileStorageFactory, inMemoryStorageFactory, printingOrchestrator, bufferFillerMock, pair(500UL, 0), breakPoints, currentBreakPoint, breakPointsAdded);

            // when
            auto nextMatch = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

            // then
            ASSERT_TRUE(nextMatch.first.has_value());
            ASSERT_EQ(nextMatch.first.value(), std::make_pair(pair(61UL, 0), 5UL));

            // when
            nextMatch = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

            // then
            ASSERT_TRUE(nextMatch.first.has_value());
            ASSERT_EQ(nextMatch.first.value(), std::make_pair(pair(293UL, 0), 5UL));

            // when
            nextMatch = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

            // then
            ASSERT_TRUE(nextMatch.first.has_value());
            ASSERT_EQ(nextMatch.first.value(), std::make_pair(pair(417UL, 0), 5UL));

            // when
            nextMatch = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

            // then
            ASSERT_TRUE(nextMatch.first.has_value());
            ASSERT_EQ(nextMatch.first.value(), std::make_pair(pair(61UL, 0), 5UL));
        }
    }

    TEST_F(FastSearchIntegTest, getNextMatchBothFromAndToResults)
    {
        for (int i = 0; i < TEST_LOOP_COUNT; ++i)
        {
            // before
            std::u32string searchedString{U'U', U'T', U'F', U'-', U'8'};
            const char *commandLineArguments[] = {"./feather", "file", "--relax=10", "--mode=text"};
            utils::ProgramOptionsParser::parseArguments(4UL, commandLineArguments);
            configuration->overrideConfiguration();

            std::shared_ptr<feather::utils::storage::AbstractStorage> storageFromFile = storageFactory->getStorage("./tst/ut/helpers/testfiles/Utf8TestFile", TEST_UUID_4);
            std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("getNextMatchBothFromAndToResults.fea", storageFromFile);

            EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
                .WillRepeatedly(::testing::ReturnRef(storage));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getStorage())
                .WillRepeatedly(::testing::ReturnRef(storage));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getUUID(false))
                .WillRepeatedly(::testing::ReturnRef(TEST_UUID_4));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getCursorRealPosition(::testing::_))
                .WillRepeatedly(::testing::Return(pair(400, 0)));

            std::unique_ptr<FastSearch> fastSearch = std::make_unique<FastSearch>(TEST_UUID, std::u32string(searchedString), fileStorageFactory, inMemoryStorageFactory, printingOrchestrator, bufferFillerMock, pair(400UL, 0), breakPoints, currentBreakPoint, breakPointsAdded);

            // when
            auto nextMatch = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

            // then
            ASSERT_TRUE(nextMatch.first.has_value());
            ASSERT_EQ(nextMatch.first.value(), std::make_pair(pair(417UL, 0), 5UL));

            // when
            nextMatch = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

            // then
            ASSERT_TRUE(nextMatch.first.has_value());
            ASSERT_EQ(nextMatch.first.value(), std::make_pair(pair(61UL, 0), 5UL));

            // when
            nextMatch = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

            // then
            ASSERT_TRUE(nextMatch.first.has_value());
            ASSERT_EQ(nextMatch.first.value(), std::make_pair(pair(293UL, 0), 5UL));

            // when
            nextMatch = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

            // then
            ASSERT_TRUE(nextMatch.first.has_value());
            ASSERT_EQ(nextMatch.first.value(), std::make_pair(pair(417UL, 0), 5UL));
        }
    }

    TEST_F(FastSearchIntegTest, getNextMatchFromResultsWhenWraps)
    {
        for (int i = 0; i < TEST_LOOP_COUNT; ++i)
        {
            // before
            std::u32string searchedString{U'U', U'T', U'F', U'-', U'8'};
            const char *commandLineArguments[] = {"./feather", "file", "--relax=10", "--mode=text"};
            utils::ProgramOptionsParser::parseArguments(4UL, commandLineArguments);
            configuration->overrideConfiguration();

            std::shared_ptr<feather::utils::storage::AbstractStorage> storageFromFile = storageFactory->getStorage("./tst/ut/helpers/testfiles/Utf8TestFile", TEST_UUID_4);
            std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("getNextMatchFromResultsWhenWraps.fea", storageFromFile);

            EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
                .WillRepeatedly(::testing::ReturnRef(storage));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getStorage())
                .WillRepeatedly(::testing::ReturnRef(storage));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getUUID(false))
                .WillRepeatedly(::testing::ReturnRef(TEST_UUID_4));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getCursorRealPosition(::testing::_))
                .WillRepeatedly(::testing::Return(pair(14201UL, 0)));

            std::unique_ptr<FastSearch> fastSearch = std::make_unique<FastSearch>(TEST_UUID, std::u32string(searchedString), fileStorageFactory, inMemoryStorageFactory, printingOrchestrator, bufferFillerMock, pair(14201UL, 0), breakPoints, currentBreakPoint, breakPointsAdded);

            // when
            auto nextMatch = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

            // then
            ASSERT_TRUE(nextMatch.first.has_value());
            ASSERT_EQ(nextMatch.first.value(), std::make_pair(pair(61UL, 0UL), 5UL));

            // when
            nextMatch = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

            // then
            ASSERT_TRUE(nextMatch.first.has_value());
            ASSERT_EQ(nextMatch.first.value(), std::make_pair(pair(293UL, 0UL), 5UL));

            // when
            nextMatch = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

            // then
            ASSERT_TRUE(nextMatch.first.has_value());
            ASSERT_EQ(nextMatch.first.value(), std::make_pair(pair(417UL, 0UL), 5UL));

            // when
            nextMatch = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

            // then
            ASSERT_TRUE(nextMatch.first.has_value());
            ASSERT_EQ(nextMatch.first.value(), std::make_pair(pair(61UL, 0UL), 5UL));
        }
    }

    TEST_F(FastSearchIntegTest, noResultsIfAnyOfCharacterIsModified)
    {
        for (int i = 0; i < TEST_LOOP_COUNT; ++i)
        {
            printingOrchestrator->removeChangesRelatedToWindow(TEST_UUID_4);
            // before
            std::u32string searchedString{U'O', U'r', U'i', U'g', U'i'};
            const char *commandLineArguments[] = {"./feather", "file", "--relax=50", "--mode=text"};
            utils::ProgramOptionsParser::parseArguments(4UL, commandLineArguments);
            configuration->overrideConfiguration();

            std::shared_ptr<feather::utils::storage::AbstractStorage> storageFromFile = storageFactory->getStorage("./tst/ut/helpers/testfiles/Utf8TestFile", TEST_UUID_4);
            std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("noResultsIfAnyOfCharacterIsModified.fea", storageFromFile);

            EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
                .WillRepeatedly(::testing::ReturnRef(storage));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getStorage())
                .WillRepeatedly(::testing::ReturnRef(storage));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getUUID(false))
                .WillRepeatedly(::testing::ReturnRef(TEST_UUID_4));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getCursorRealPosition(::testing::_))
                .WillRepeatedly(::testing::Return(pair(20UL, 0)));

            std::unique_ptr<FastSearch> fastSearch = std::make_unique<FastSearch>(TEST_UUID_4, std::u32string(searchedString), fileStorageFactory, inMemoryStorageFactory, printingOrchestrator, bufferFillerMock, pair(20UL, 0), breakPoints, currentBreakPoint, breakPointsAdded);

            // when
            auto nextMatch = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

            // then
            ASSERT_TRUE(nextMatch.first.has_value());
            ASSERT_EQ(nextMatch.first.value(), std::make_pair(pair(0UL, 0UL), 5UL));

            // when
            printingOrchestrator->removeCharacters(TEST_UUID_4, storage, pair(0, 0), pair(1, 0));
            nextMatch = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

            // then
            ASSERT_FALSE(nextMatch.first.has_value());
            ASSERT_EQ(nextMatch.second, utils::datatypes::ERROR::NONE);
        }
    }

    TEST_F(FastSearchIntegTest, getNextMatchToResultsWhenWraps)
    {
        for (int i = 0; i < TEST_LOOP_COUNT; ++i)
        {
            printingOrchestrator->removeChangesRelatedToWindow(TEST_UUID_4);
            // before
            std::u32string searchedString{U'U', U'T', U'F', U'-', U'8'};
            const char *commandLineArguments[] = {"./feather", "file", "--relax=50", "--mode=text"};
            utils::ProgramOptionsParser::parseArguments(4UL, commandLineArguments);
            configuration->overrideConfiguration();

            std::shared_ptr<feather::utils::storage::AbstractStorage> fileStorage = storageFactory->getStorage("./tst/ut/helpers/testfiles/Utf8TestFile_2", TEST_UUID_4);
            std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("getNextMatchToResultsWhenWraps.fea", fileStorage);

            EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
                .WillRepeatedly(::testing::ReturnRef(storage));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getStorage())
                .WillRepeatedly(::testing::ReturnRef(storage));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getUUID(false))
                .WillRepeatedly(::testing::ReturnRef(TEST_UUID_4));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getCursorRealPosition(::testing::_))
                .WillRepeatedly(::testing::Return(pair(20UL, 0)));

            std::unique_ptr<FastSearch> fastSearch = std::make_unique<FastSearch>(TEST_UUID_4, std::u32string(searchedString), fileStorageFactory, inMemoryStorageFactory, printingOrchestrator, bufferFillerMock, pair(20UL, 0), breakPoints, currentBreakPoint, breakPointsAdded);

            // when
            auto nextMatch = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

            // then
            ASSERT_TRUE(nextMatch.first.has_value());
            ASSERT_EQ(nextMatch.first.value(), std::make_pair(pair(66UL, 0UL), 5UL));

            // when
            nextMatch = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

            // then
            ASSERT_TRUE(nextMatch.first.has_value());
            ASSERT_EQ(nextMatch.first.value(), std::make_pair(pair(298UL, 0UL), 5UL));

            // when
            nextMatch = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

            // then
            ASSERT_TRUE(nextMatch.first.has_value());
            ASSERT_EQ(nextMatch.first.value(), std::make_pair(pair(422UL, 0UL), 5UL));

            // when
            nextMatch = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

            // then
            ASSERT_TRUE(nextMatch.first.has_value());
            ASSERT_EQ(nextMatch.first.value(), std::make_pair(pair(14206UL, 0UL), 5UL));

            // when
            nextMatch = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

            // then
            ASSERT_TRUE(nextMatch.first.has_value());
            ASSERT_EQ(nextMatch.first.value(), std::make_pair(pair(14213UL, 0UL), 5UL));

            // when
            nextMatch = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

            // then
            ASSERT_TRUE(nextMatch.first.has_value());
            ASSERT_EQ(nextMatch.first.value(), std::make_pair(pair(0UL, 0UL), 5UL));
        }
    }

    TEST_F(FastSearchIntegTest, getNextMatchToResultsWhenWrapsWithSecondarySearchResults)
    {
        for (int i = 0; i < TEST_LOOP_COUNT; ++i)
        {
            printingOrchestrator->removeChangesRelatedToWindow(TEST_UUID_4);
            // before
            std::u32string searchedString{U'U', U'T', U'F', U'-', U'8'};
            const char *commandLineArguments[] = {"./feather", "file", "--relax=50", "--mode=text"};
            utils::ProgramOptionsParser::parseArguments(4UL, commandLineArguments);
            configuration->overrideConfiguration();

            std::shared_ptr<feather::utils::storage::AbstractStorage> storageFromFile = storageFactory->getStorage("./tst/ut/helpers/testfiles/Utf8TestFile_2", TEST_UUID_4);
            std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("getNextMatchToResultsWhenWrapsWithSecondarySearchResults.fea", storageFromFile);

            EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
                .WillRepeatedly(::testing::ReturnRef(storage));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getStorage())
                .WillRepeatedly(::testing::ReturnRef(storage));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getUUID(false))
                .WillRepeatedly(::testing::ReturnRef(TEST_UUID_4));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getCursorRealPosition(::testing::_))
                .WillRepeatedly(::testing::Return(pair(20UL, 0)));
            std::unique_ptr<FastSearch> fastSearch = std::make_unique<FastSearch>(TEST_UUID_4, std::u32string(searchedString), fileStorageFactory, inMemoryStorageFactory, printingOrchestrator, bufferFillerMock, pair(20UL, 0), breakPoints, currentBreakPoint, breakPointsAdded);

            // when
            printingOrchestrator->addChanges(TEST_UUID_4, storage, pair(0, 0), {'U', 'T', 'F', '-', '8'}, 5UL);
            auto nextMatch = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

            // then
            ASSERT_TRUE(nextMatch.first.has_value());
            ASSERT_EQ(nextMatch.first.value(), std::make_pair(pair(66UL, 0), 5UL));

            // when
            nextMatch = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

            // then
            ASSERT_TRUE(nextMatch.first.has_value());
            ASSERT_EQ(nextMatch.first.value(), std::make_pair(pair(298UL, 0), 5UL));

            // when
            nextMatch = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

            // then
            ASSERT_TRUE(nextMatch.first.has_value());
            ASSERT_EQ(nextMatch.first.value(), std::make_pair(pair(422UL, 0), 5UL));

            // when
            nextMatch = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

            // then
            ASSERT_TRUE(nextMatch.first.has_value());
            ASSERT_EQ(nextMatch.first.value(), std::make_pair(pair(14206UL, 0), 5UL));

            // when
            nextMatch = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

            // then
            ASSERT_TRUE(nextMatch.first.has_value());
            ASSERT_EQ(nextMatch.first.value(), std::make_pair(pair(14213UL, 0), 5UL));

            // when
            nextMatch = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::DOWN);

            // then
            ASSERT_TRUE(nextMatch.first.has_value());
            ASSERT_EQ(nextMatch.first.value(), std::make_pair(pair(0, 0), 5UL));
        }
    }

    TEST_F(FastSearchIntegTest, getPrevMatchFromResults)
    {
        for (int i = 0; i < TEST_LOOP_COUNT; ++i)
        {
            printingOrchestrator->removeChangesRelatedToWindow(TEST_UUID_4);
            // before
            std::u32string searchedString{U'U', U'T', U'F', U'-', U'8'};
            const char *commandLineArguments[] = {"./feather", "file", "--relax=10", "--mode=text"};
            utils::ProgramOptionsParser::parseArguments(4UL, commandLineArguments);
            configuration->overrideConfiguration();

            std::shared_ptr<feather::utils::storage::AbstractStorage> storageFromFile = storageFactory->getStorage("./tst/ut/helpers/testfiles/Utf8TestFile", TEST_UUID_4);
            std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("getPrevMatchFromResults.fea", storageFromFile);

            EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
                .WillRepeatedly(::testing::ReturnRef(storage));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getStorage())
                .WillRepeatedly(::testing::ReturnRef(storage));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getUUID(false))
                .WillRepeatedly(::testing::ReturnRef(TEST_UUID_4));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getCursorRealPosition(::testing::_))
                .WillRepeatedly(::testing::Return(pair(0, 0)));

            std::unique_ptr<FastSearch> fastSearch = std::make_unique<FastSearch>(TEST_UUID_4, std::u32string(searchedString), fileStorageFactory, inMemoryStorageFactory, printingOrchestrator, bufferFillerMock, pair(0, 0), breakPoints, currentBreakPoint, breakPointsAdded);

            // when
            auto nextMatch = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::UP);

            // then
            ASSERT_TRUE(nextMatch.first.has_value());
            ASSERT_EQ(nextMatch.first.value(), std::make_pair(pair(417UL, 0), 5UL));

            // when
            nextMatch = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::UP);

            // then
            ASSERT_TRUE(nextMatch.first.has_value());
            ASSERT_EQ(nextMatch.first.value(), std::make_pair(pair(293UL, 0), 5UL));

            // when
            nextMatch = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::UP);

            // then
            ASSERT_TRUE(nextMatch.first.has_value());
            ASSERT_EQ(nextMatch.first.value(), std::make_pair(pair(61UL, 0), 5UL));

            // when
            nextMatch = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, utils::Direction::UP);

            // then
            ASSERT_TRUE(nextMatch.first.has_value());
            ASSERT_EQ(nextMatch.first.value(), std::make_pair(pair(417UL, 0), 5UL));
        }
    }

    TEST_F(FastSearchIntegTest, findNewLineHexSearch)
    {
        // before
        utils::Direction searchDirection = utils::Direction::DOWN;
        std::u32string searchedString{0x10};
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4UL, commandLineArguments);
        configuration->overrideConfiguration();

        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("findNewLineHexSearch.fea", {(char)0x10, (char)0x4A, (char)0x46, (char)0x49, (char)0x46, (char)0xC3, (char)0x9F, (char)0x00, (char)0x01, (char)0x01, (char)0x0A});
        EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
            .WillRepeatedly(::testing::ReturnRef(storage));
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getStorage())
            .WillRepeatedly(::testing::ReturnRef(storage));
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getUUID(false))
            .WillRepeatedly(::testing::ReturnRef(TEST_UUID_4));
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getCursorRealPosition(::testing::_))
            .WillRepeatedly(::testing::Return(pair(0, 0)));

        std::unique_ptr<FastSearch> fastSearch = std::make_unique<FastSearch>(TEST_UUID, std::u32string(searchedString), fileStorageFactory, inMemoryStorageFactory, printingOrchestrator, bufferFillerMock, pair(0, 0), breakPoints, currentBreakPoint, breakPointsAdded);

        // when
        printingOrchestrator->addChanges(TEST_UUID, storage, pair(2, 0), {0x10}, 1UL);
        auto result = fastSearch->getNextMatch(windowsManagerMock, printingVisitorMock, halfFrameUpVisitorMock, bottomBarWindowMock, currentFeatherMode, searchDirection);

        // then
        ASSERT_TRUE(result.first.has_value());
        ASSERT_EQ(result.first.value(), std::make_pair(std::pair(0UL, 0UL), 1UL));
        ASSERT_EQ(result.second, utils::datatypes::ERROR::NONE);
    }
} // namespace feather::utils::algorithm
