#include "helpers/TestBase.hpp"
#include "mocks/FilesystemMock.hpp"
#include "mocks/BufferFillerMock.hpp"
#include "mocks/NCursesMainWindowDecoratorMock.hpp"
#include "mocks/NCursesWindowMock.hpp"
#include "mocks/PrintingVisitorMock.hpp"
#include "mocks/WindowsManagerMock.hpp"
#include "printer/FilePrintingStorage.hpp"
#include "printer/PrintingOrchestrator.hpp"
#include "utils/ProgramOptionsParser.hpp"
#include "utils/helpers/Lambda.hpp"
#include "utils/storage/AbstractStorage.hpp"
#include "utils/storage/FileStorageFactory.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace ::testing::mocks;

namespace feather::utils::algorithm
{
    class LambdaIntegTest : public ::testing::Test, public ::feather::test::TestBase
    {
    protected:
        std::unique_ptr<printer::PrintingVisitorInterface> printingVisitorMock;
        std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator;
        std::shared_ptr<PrintingStorageInterface> filePrintingStorage;
        std::shared_ptr<WindowsManager> windowsManagerMock;
        std::shared_ptr<MainWindowInterface> nCursesMainWindowDecoratorMock;
        std::shared_ptr<WindowImplInterface> nCursesWindowMock;
        std::unique_ptr<utils::BufferFillerInterface> bufferFillerMock;

    public:
        void SetUp() override
        {
            printingVisitorMock = std::unique_ptr<PrintingVisitorMock>(new PrintingVisitorMock());
            filePrintingStorage = std::make_shared<FilePrintingStorage>(inMemoryStorageFactory);
            printingOrchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, filePrintingStorage, nullptr, changesStack, filesystemMock);
            windowsManagerMock = std::make_shared<WindowsManagerMock>();
            nCursesMainWindowDecoratorMock = std::make_shared<NCursesMainWindowDecoratorMock>();
            nCursesWindowMock = std::make_shared<NCursesWindowMock>();
            bufferFillerMock = std::make_unique<BufferFillerMock>();
        }

        void TearDown() override
        {
            printingOrchestrator->removeChangesRelatedToWindow(TEST_UUID);
        }
    };

    TEST_F(LambdaIntegTest, findNext)
    {
        // expect
        EXPECT_CALL(*dynamic_cast<testing::mocks::WindowsManagerMock *>(windowsManagerMock.get()), getMainWindow(TEST_UUID))
            .WillRepeatedly(::testing::Return(nCursesMainWindowDecoratorMock));
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getLibraryWindowHandler())
            .WillRepeatedly(::testing::Return(nCursesWindowMock.get()));
        EXPECT_CALL(*dynamic_cast<NCursesWindowMock *>(nCursesWindowMock.get()), getCharacter())
            .WillRepeatedly(::testing::Return(0));

        // before
        utils::windows::FramePositions framePositions = utils::windows::FramePositions();
        std::vector<char> text = FIVE_BYTE_UTF8_CHARACTER;
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        configuration->forceTextMode(testFile);
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("setCursorInMiddleOfUtf8Character.fea", text);

        // when
        // then
        ASSERT_EQ(pair(0, 0), utils::helpers::Lambda::findNext(
                                  pair(0, 0), [](char32_t c)
                                  { return c == U't'; },
                                  printingOrchestrator, TEST_UUID, storage, windowsManagerMock, false, false)
                                  .first);
        ASSERT_EQ(pair(1, 0), utils::helpers::Lambda::findNext(
                                  pair(0, 0), [](char32_t c)
                                  { return c == 0x9BC5; },
                                  printingOrchestrator, TEST_UUID, storage, windowsManagerMock, false, false)
                                  .first);
        ASSERT_EQ(pair(3, 0), utils::helpers::Lambda::findNext(
                                  pair(3, 0), [](char32_t c)
                                  { return c == 0x9BC5; },
                                  printingOrchestrator, TEST_UUID, storage, windowsManagerMock, false, false)
                                  .first);
        // Nothing found
        ASSERT_EQ(pair(5, 0), utils::helpers::Lambda::findNext(
                                  pair(3, 0), [](char32_t c)
                                  { return c == 0x9B9B; },
                                  printingOrchestrator, TEST_UUID, storage, windowsManagerMock, false, false)
                                  .first);

        // when
        printingOrchestrator->addChanges(TEST_UUID, storage, pair(0, 0), TWO_BYTE_UTF8_CHARACTER, 2UL);

        // then
        ASSERT_EQ(pair(0, 0), utils::helpers::Lambda::findNext(
                                  pair(0, 0), [](char32_t c)
                                  { return c == 0x9BC5; },
                                  printingOrchestrator, TEST_UUID, storage, windowsManagerMock, false, false)
                                  .first);

        // when
        printingOrchestrator->removeCharacters(TEST_UUID, storage, pair(1, 0), pair(3, 0));

        // then
        ASSERT_EQ(pair(3, 0), utils::helpers::Lambda::findNext(
                                  pair(0, 2), [](char32_t c)
                                  { return c == 0x9BC5; },
                                  printingOrchestrator, TEST_UUID, storage, windowsManagerMock, false, false)
                                  .first);
    }

    TEST_F(LambdaIntegTest, findPrevious)
    {
        // expect
        EXPECT_CALL(*dynamic_cast<testing::mocks::WindowsManagerMock *>(windowsManagerMock.get()), getMainWindow(TEST_UUID))
            .WillRepeatedly(::testing::Return(nCursesMainWindowDecoratorMock));
        EXPECT_CALL(*dynamic_cast<testing::mocks::WindowsManagerMock *>(windowsManagerMock.get()), refreshAllWindows(::testing::_, ::testing::_))
            .Times(2);
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getLibraryWindowHandler())
            .WillRepeatedly(::testing::Return(nCursesWindowMock.get()));
        EXPECT_CALL(*dynamic_cast<NCursesWindowMock *>(nCursesWindowMock.get()), getCharacter())
            .WillRepeatedly(::testing::Return(0));

        // before
        utils::windows::FramePositions framePositions = utils::windows::FramePositions();
        std::vector<char> text = TEN_BYTE_UTF8_CHARACTER;
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        configuration->forceTextMode(testFile);
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("findPrevious.fea", text);

        // when
        // then
        ASSERT_EQ(pair(0, 0), utils::helpers::Lambda::findPrevious(
                                  pair(10, 0), [](char32_t c)
                                  { return c == U't'; },
                                  printingOrchestrator, TEST_UUID, storage, windowsManagerMock, false)
                                  .first);
        ASSERT_EQ(pair(1, 0), utils::helpers::Lambda::findPrevious(
                                  pair(10, 0), [](char32_t c)
                                  { return c == U'e'; },
                                  printingOrchestrator, TEST_UUID, storage, windowsManagerMock, false)
                                  .first);
        ASSERT_EQ(pair(6, 0), utils::helpers::Lambda::findPrevious(
                                  pair(10, 0), [](char32_t c)
                                  { return c == 0x9A989FF0; },
                                  printingOrchestrator, TEST_UUID, storage, windowsManagerMock, false)
                                  .first);
        // Nothing found
        ASSERT_EQ(std::nullopt, utils::helpers::Lambda::findPrevious(
                                    pair(10, 0), [](char32_t c)
                                    { return c == 0x9B9B; },
                                    printingOrchestrator, TEST_UUID, storage, windowsManagerMock, false)
                                    .second);
        // Nothing found
        ASSERT_EQ(std::nullopt, utils::helpers::Lambda::findPrevious(
                                    pair(0, 0), [](char32_t c)
                                    { return c == 0x9B9B; },
                                    printingOrchestrator, TEST_UUID, storage, windowsManagerMock, false)
                                    .second);
    }

    TEST_F(LambdaIntegTest, alignToProperPosition)
    {
        // before
        utils::windows::FramePositions framePositions = utils::windows::FramePositions();
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        configuration->forceTextMode(testFile);
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("alignToProperPosition.fea", TWENTY_BYTE_UTF8_CHARACTER);
        setMode(utils::FEATHER_MODE::READ_MODE);

        // when
        // then
        ASSERT_EQ(pair(0, 0), utils::helpers::Lambda::alignToProperPosition(pair(0, 0), storage, printingOrchestrator, TEST_UUID));
        ASSERT_EQ(pair(16UL, 0), utils::helpers::Lambda::alignToProperPosition(pair(20UL, 0), storage, printingOrchestrator, TEST_UUID));

        // when
        printingOrchestrator->removeCharacters(TEST_UUID, storage, pair(6UL, 0), pair(10UL, 0));

        // then
        ASSERT_EQ(pair(2UL, 0), utils::helpers::Lambda::alignToProperPosition(pair(5, 0), storage, printingOrchestrator, TEST_UUID));
    }

    TEST_F(LambdaIntegTest, mergeInterval)
    {
        // before
        auto emptyInterval = std::map<pair, pair>();
        auto oneElementInterval = std::map<pair, pair>{std::make_pair(pair(0, 0), pair(0, 5))};
        auto twoElementsIntervalNotJoin = std::map<pair, pair>{std::make_pair(pair(0, 0), pair(0, 5)), std::make_pair(pair(0, 7), pair(0, 15))};
        auto twoElementsIntervalJoin = std::map<pair, pair>{std::make_pair(pair(0, 0), pair(0, 5)), std::make_pair(pair(0, 6), pair(0, 15))};
        auto threeElementsIntervalJoin = std::map<pair, pair>{std::make_pair(pair(0, 0), pair(0, 5)), std::make_pair(pair(0, 6), pair(0, 15)), std::make_pair(pair(0, 17), pair(0, 20))};

        // when
        auto emptyIntervalReslt = utils::helpers::Lambda::mergeInterval(emptyInterval, TEST_UUID, printingOrchestrator, 1);
        auto oneElementIntervalResult = utils::helpers::Lambda::mergeInterval(oneElementInterval, TEST_UUID, printingOrchestrator, 1);
        auto twoElementsIntervalNotJoinResult = utils::helpers::Lambda::mergeInterval(twoElementsIntervalNotJoin, TEST_UUID, printingOrchestrator, 1);
        auto twoElementsIntervalJoinResult = utils::helpers::Lambda::mergeInterval(twoElementsIntervalJoin, TEST_UUID, printingOrchestrator, 1);
        auto threeElementsIntervalJoinResult = utils::helpers::Lambda::mergeInterval(threeElementsIntervalJoin, TEST_UUID, printingOrchestrator, 1);

        // then
        ASSERT_THAT(emptyIntervalReslt, ::testing::ElementsAre());
        ASSERT_THAT(oneElementIntervalResult, ::testing::ElementsAre(std::make_pair(pair(0, 0), pair(0, 5))));
        ASSERT_THAT(twoElementsIntervalNotJoinResult, ::testing::ElementsAre(std::make_pair(pair(0, 0), pair(0, 5)), std::make_pair(pair(0, 7), pair(0, 15))));
        ASSERT_THAT(twoElementsIntervalJoinResult, ::testing::ElementsAre(std::make_pair(pair(0, 0), pair(0, 15))));
        ASSERT_THAT(threeElementsIntervalJoinResult, ::testing::ElementsAre(std::make_pair(pair(0, 0), pair(0, 15)), std::make_pair(pair(0, 17), pair(0, 20))));
    }

    TEST_F(LambdaIntegTest, findNextNothingFound)
    {
        // expect
        EXPECT_CALL(*dynamic_cast<testing::mocks::WindowsManagerMock *>(windowsManagerMock.get()), getMainWindow(TEST_UUID))
            .WillRepeatedly(::testing::Return(nCursesMainWindowDecoratorMock));
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getLibraryWindowHandler())
            .WillRepeatedly(::testing::Return(nCursesWindowMock.get()));
        EXPECT_CALL(*dynamic_cast<NCursesWindowMock *>(nCursesWindowMock.get()), getCharacter())
            .WillRepeatedly(::testing::Return(0));

        // before
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        configuration->forceTextMode(testFile);
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("alignToProperPosition.fea", TWENTY_BYTE_UTF8_CHARACTER);

        // when
        auto findOpeningBracket = utils::helpers::Lambda::findNext(pair(0, 0), utils::helpers::Lambda::isOpenBracket, printingOrchestrator, TEST_UUID, storage, windowsManagerMock, false, false);

        // then
        ASSERT_THAT(findOpeningBracket.first, pair(20, 0));
        ASSERT_THAT(findOpeningBracket.second, std::nullopt);
    }

    TEST_F(LambdaIntegTest, getFolderName)
    {
        // expect
        EXPECT_CALL(*dynamic_cast<testing::mocks::WindowsManagerMock *>(windowsManagerMock.get()), getMainWindow(TEST_UUID))
            .WillRepeatedly(::testing::Return(nCursesMainWindowDecoratorMock));
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getLibraryWindowHandler())
            .WillRepeatedly(::testing::Return(nCursesWindowMock.get()));
        EXPECT_CALL(*dynamic_cast<NCursesWindowMock *>(nCursesWindowMock.get()), getCharacter())
            .WillRepeatedly(::testing::Return(0));

        // before
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        configuration->forceTextMode(testFile);
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("alignToProperPosition.fea", TWENTY_BYTE_UTF8_CHARACTER);

        // when
        auto findOpeningBracket = utils::helpers::Lambda::findNext(pair(0, 0), utils::helpers::Lambda::isOpenBracket, printingOrchestrator, TEST_UUID, storage, windowsManagerMock, false, false);

        // then
        ASSERT_THAT(findOpeningBracket.first, pair(20, 0));
        ASSERT_THAT(findOpeningBracket.second, std::nullopt);
    }

    TEST_F(LambdaIntegTest, isCharAtPos)
    {
        // before
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        auto text = FIVE_BYTE_UTF8_CHARACTER;
        text.push_back(U'>');
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        configuration->forceTextMode(testFile);
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("alignToProperPosition.fea", text);

        // when
        auto result = utils::helpers::Lambda::isCharAtPos(pair(0, 0), utils::helpers::Lambda::isOpenBracket, TEST_UUID, storage, printingOrchestrator);

        // then
        ASSERT_FALSE(result.first);
        ASSERT_EQ(result.second.value(), U't');

        // when
        result = utils::helpers::Lambda::isCharAtPos(pair(5, 0), utils::helpers::Lambda::isBracket, TEST_UUID, storage, printingOrchestrator);

        // then
        ASSERT_TRUE(result.first);
        ASSERT_EQ(result.second.value(), U'>');
    }

    TEST_F(LambdaIntegTest, convertStrigifiedHexToHex)
    {
        // before
        std::u32string expectedResult;
        expectedResult.push_back(0xAB);
        expectedResult.push_back(0xCD);
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        auto text = FIVE_BYTE_UTF8_CHARACTER;
        text.push_back(U'>');
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        configuration->forceTextMode(testFile);

        // when
        auto result = utils::helpers::Lambda::convertStrigifiedHexToHex(U"0xABCD");

        // then
        ASSERT_EQ(result, expectedResult);
    }

    TEST_F(LambdaIntegTest, getCursorCoordinatesFromRealPositionWhenCursorPosIsInvalidAndInLastColumn)
    {
        utils::ScreenBuffer buffer(2);
        std::u32string firstLine{'8', '0', 0x9BD2};
        std::u32string secondLine{'8', '0', 0x9BD2};
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("getCursorCoordinatesFromRealPositionWhenCursorPosIsInvalidAndInLastColumn.fea", {(char)0x80, (char)0xD2, (char)0x9B, (char)0x18});
        utils::windows::FramePositions currentFramePosition(pair{0, 0}, pair{11, 0});

        // expect
        EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
            .WillOnce(::testing::ReturnRef(storage));
        EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getFramePositions())
            .WillOnce(::testing::ReturnRef(currentFramePosition));
        EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getScreenBuffer())
            .WillRepeatedly(::testing::ReturnRef(buffer));

        // before
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4UL, commandLineArguments);
        configuration->overrideConfiguration();
        buffer.fillBufferLine(firstLine);
        buffer.fillBufferLine(secondLine);

        // when
        auto result = utils::helpers::Lambda::getCursorCoordinatesFromRealPosition(pair(3UL, 0), printingOrchestrator, bufferFillerMock, TEST_UUID, 10UL, false, false);

        // then
        ASSERT_EQ(result, pair(0, 2UL));
    }

    TEST_F(LambdaIntegTest, getCursorCoordinatesFromRealPosition)
    {
        // before
        utils::windows::FramePositions currentFramePosition(pair{0, 0}, pair{11, 0});
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4UL, commandLineArguments);
        configuration->overrideConfiguration();
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("getCursorCoordinatesFromRealPosition.fea", {(char)0x10, (char)0x4A, (char)0x46, (char)0x49, (char)0x46, (char)0xC3, (char)0x9F, (char)0x00, (char)0x01, (char)0x01, (char)0x0A});
        utils::ScreenBuffer buffer(2);
        std::u32string firstLine;
        firstLine.push_back(0x10);
        firstLine.push_back(0x4A);
        firstLine.push_back(0x10);
        std::u32string secondLine;
        secondLine.push_back(0x46);
        secondLine.push_back(0x49);
        secondLine.push_back(0x46);
        secondLine.push_back(0xC39F);
        secondLine.push_back(0x00);
        secondLine.push_back(0x01);
        secondLine.push_back(0x01);
        secondLine.push_back(0x0A);
        buffer.fillBufferLine(firstLine);
        buffer.fillBufferLine(secondLine);
        EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
            .WillOnce(::testing::ReturnRef(storage));

        EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getFramePositions())
            .WillOnce(::testing::ReturnRef(currentFramePosition));

        EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getScreenBuffer())
            .WillRepeatedly(::testing::ReturnRef(buffer));
        printingOrchestrator->addChanges(TEST_UUID, storage, pair(2, 0), {0x10}, 1UL);

        // when
        auto result = utils::helpers::Lambda::getCursorCoordinatesFromRealPosition(pair(2, 0), printingOrchestrator, bufferFillerMock, TEST_UUID, 10UL, false, false);

        // then
        ASSERT_EQ(result, pair(0, 3UL));
    }

    TEST_F(LambdaIntegTest, getCursorCoordinatesFromRealPositionWhenCursorPosIsInvalid)
    {
        // before
        utils::windows::FramePositions currentFramePosition(pair{0, 0}, pair{11, 0});
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4UL, commandLineArguments);
        configuration->overrideConfiguration();
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("getCursorCoordinatesFromRealPositionWhenCursorPosIsInvalid.fea", {(char)0x80, (char)0xD2, (char)0x9B, (char)0x18});
        utils::ScreenBuffer buffer(2);
        std::u32string firstLine{'8', '0', 0x9BD2, '1', '8'};
        buffer.fillBufferLine(firstLine);
        EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
            .WillOnce(::testing::ReturnRef(storage));

        EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getFramePositions())
            .WillOnce(::testing::ReturnRef(currentFramePosition));

        EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getScreenBuffer())
            .WillRepeatedly(::testing::ReturnRef(buffer));

        // when
        auto result = utils::helpers::Lambda::getCursorCoordinatesFromRealPosition(pair(3UL, 0), printingOrchestrator, bufferFillerMock, TEST_UUID, 10UL, false, false);

        // then
        ASSERT_EQ(result, pair(0, 2UL));
    }

    TEST_F(LambdaIntegTest, getCursorCoordinatesFromRealPositionWithUtf8)
    {
        // before
        utils::windows::FramePositions currentFramePosition(pair{920952788, 0}, pair{920962595, 0});
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4UL, commandLineArguments);
        configuration->overrideConfiguration();
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("getCursorCoordinatesFromRealPositionWithUtf8", {(char)0x80, (char)0x00});
        utils::ScreenBuffer screenBuffer(10UL);
        std::vector<std::u32string> buffer = {{0x5f, 0x43, 0x54, 0x59, 0x50, 0x45, 0x0, 0x4c, 0x41, 0x4e, 0x47, 0x0, 0x2f, 0x75, 0x73, 0x72, 0x2f, 0x73, 0x68, 0x61, 0x72, 0x65, 0x2f, 0x6b, 0x65, 0x79, 0x6d, 0x61, 0x70, 0x73, 0x2f, 0x0, 0x2f, 0x75, 0x73, 0x72, 0x2f, 0x73, 0x68, 0x61, 0x72, 0x65, 0x2f, 0x6b, 0x62, 0x64, 0x2f, 0x6b, 0x65, 0x79, 0x6d, 0x61, 0x70, 0x73, 0x2f, 0x0, 0x2f, 0x75},
                                              {0x73, 0x72, 0x2f, 0x6c, 0x69, 0x62, 0x2f, 0x6b, 0x62, 0x64, 0x2f, 0x6b, 0x65, 0x79, 0x6d, 0x61, 0x70, 0x73, 0x2f, 0x0, 0x0, 0x44, 0x69, 0x72, 0x65, 0x63, 0x74, 0x6f, 0x72, 0x79, 0x20, 0x6e, 0x6f, 0x74, 0x20, 0x66, 0x6f, 0x75, 0x6e, 0x64, 0x20, 0x25, 0x73, 0x0, 0x2e, 0x2e, 0x2f, 0x73, 0x72, 0x63, 0x2f, 0x62, 0x61, 0x73, 0x69, 0x63, 0x2f, 0x6c, 0x6f},
                                              {0x63, 0x61, 0x6c, 0x65, 0x2d, 0x75, 0x74, 0x69, 0x6c, 0x2e, 0x63, 0x0, 0x43, 0x61, 0x6e, 0x27, 0x74, 0x20, 0x61, 0x64, 0x64, 0x20, 0x6b, 0x65, 0x79, 0x6d, 0x61, 0x70, 0x3a, 0x20, 0x25, 0x6d, 0x0, 0x63, 0x6f, 0x64, 0x65, 0x20, 0x3c, 0x20, 0x5f, 0x53, 0x50, 0x45, 0x43, 0x49, 0x41, 0x4c, 0x5f, 0x47, 0x4c, 0x59, 0x50, 0x48, 0x5f, 0x4d, 0x41, 0x58, 0x0},
                                              {0x53, 0x59, 0x53, 0x54, 0x45, 0x4d, 0x44, 0x5f, 0x45, 0x4d, 0x4f, 0x4a, 0x49, 0x0, 0x7c, 0x20, 0x0, 0x7c, 0x2d, 0x0, 0x60, 0x2d, 0x0, 0x2a, 0x0, 0x2d, 0x3e, 0x0, 0x2b, 0x0, 0x3a, 0x2d, 0x5d, 0x0, 0x3a, 0x2d, 0x7d, 0x0, 0x3a, 0x2d, 0x29, 0x0, 0x3a, 0x2d, 0x7c, 0x0, 0x3a, 0x2d, 0x28, 0x0},
                                              {0x3a, 0x2d, 0x7b, 0x8fb8ef, 0x0, 0x3a, 0x2d, 0x5b, 0x0, 0x8294e2, 0x20, 0x0, 0x9c94e2, 0x8094e2, 0x0, 0x9494e2, 0x8094e2, 0x0, 0xa380e2, 0x0, 0x8f97e2, 0x0, 0xa280e2, 0x0, 0x9286e2, 0x0, 0x9380e2, 0x0, 0xbcce, 0x0, 0x939ce2, 0x0, 0x979ce2, 0x0, 0x87989ff0, 0x0, 0x80989ff0, 0x0, 0x82999ff0},
                                              {0x0, 0x90989ff0, 0x0, 0x81999ff0, 0x0, 0xa8989ff0, 0x0, 0xa2a49ff0, 0x0, 0x4c, 0x41, 0x4e, 0x47, 0x55, 0x41, 0x47, 0x45, 0x0, 0x4c, 0x43, 0x5f, 0x4e, 0x55, 0x4d, 0x45, 0x52, 0x49, 0x43, 0x0, 0x4c, 0x43, 0x5f, 0x54, 0x49, 0x4d, 0x45, 0x0, 0x4c, 0x43, 0x5f, 0x43, 0x4f, 0x4c, 0x4c, 0x41, 0x54, 0x45, 0x0, 0x4c},
                                              {0x43, 0x5f, 0x4d, 0x4f, 0x4e, 0x45, 0x54, 0x41, 0x52, 0x59, 0x0, 0x4c, 0x43, 0x5f, 0x4d, 0x45, 0x53, 0x53, 0x41, 0x47, 0x45, 0x53, 0x0, 0x4c, 0x43, 0x5f, 0x50, 0x41, 0x50, 0x45, 0x52, 0x0, 0x4c, 0x43, 0x5f, 0x4e, 0x41, 0x4d, 0x45, 0x0, 0x4c, 0x43, 0x5f, 0x41, 0x44, 0x44, 0x52, 0x45, 0x53, 0x53, 0x0, 0x4c, 0x43, 0x5f, 0x54, 0x45, 0x4c},
                                              {0x45, 0x50, 0x48, 0x4f, 0x4e, 0x45, 0x0, 0x4c, 0x43, 0x5f, 0x4d, 0x45, 0x41, 0x53, 0x55, 0x52, 0x45, 0x4d, 0x45, 0x4e, 0x54, 0x0, 0x4c, 0x43, 0x5f, 0x49, 0x44, 0x45, 0x4e, 0x54, 0x49, 0x46, 0x49, 0x43, 0x41, 0x54, 0x49, 0x4f, 0x4e, 0x0, 0x0, 0x73, 0x70, 0x65, 0x63, 0x69, 0x61, 0x6c, 0x5f, 0x67, 0x6c, 0x79, 0x70, 0x68, 0x0, 0x0},
                                              {0x0, 0x67, 0x65, 0x74, 0x5f, 0x6b, 0x65, 0x79, 0x6d, 0x61, 0x70, 0x73, 0x0, 0x0, 0x0, 0x0, 0x0, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d},
                                              {0x4e, 0x4f, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x0, 0x2f, 0x70, 0x72, 0x6f, 0x63, 0x2f, 0x73, 0x65, 0x6c, 0x66, 0x2f, 0x66, 0x64, 0x69, 0x6e, 0x66, 0x6f, 0x2f, 0x25, 0x69, 0x0, 0xa}};
        for (auto &e : buffer)
        {
            screenBuffer.fillBufferLine(e);
        }
        EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
            .WillRepeatedly(::testing::ReturnRef(storage));

        EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getFramePositions())
            .WillRepeatedly(::testing::ReturnRef(currentFramePosition));

        EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getScreenBuffer())
            .WillRepeatedly(::testing::ReturnRef(screenBuffer));

        // when
        auto result = utils::helpers::Lambda::getCursorCoordinatesFromRealPosition(pair(920953085, 0), printingOrchestrator, bufferFillerMock, TEST_UUID, 62UL, false, false);

        // then
        ASSERT_EQ(result, pair(4UL, 60UL));

        // when
        result = utils::helpers::Lambda::getCursorCoordinatesFromRealPosition(pair(920953088, 0), printingOrchestrator, bufferFillerMock, TEST_UUID, 62UL, false, false);

        // then
        ASSERT_EQ(result, pair(4UL, 60UL));
    }

    TEST_F(LambdaIntegTest, getScreenBufferPositionWithUtf8)
    {
        // before
        utils::windows::FramePositions currentFramePosition(pair{920952788, 0}, pair{920962595, 0});
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4UL, commandLineArguments);
        configuration->overrideConfiguration();
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("getCursorCoordinatesFromRealPositionWithUtf8", {(char)0x80, (char)0x00});
        utils::ScreenBuffer screenBuffer(10UL);
        std::vector<std::u32string> buffer = {{0x5f, 0x43, 0x54, 0x59, 0x50, 0x45, 0x0, 0x4c, 0x41, 0x4e, 0x47, 0x0, 0x2f, 0x75, 0x73, 0x72, 0x2f, 0x73, 0x68, 0x61, 0x72, 0x65, 0x2f, 0x6b, 0x65, 0x79, 0x6d, 0x61, 0x70, 0x73, 0x2f, 0x0, 0x2f, 0x75, 0x73, 0x72, 0x2f, 0x73, 0x68, 0x61, 0x72, 0x65, 0x2f, 0x6b, 0x62, 0x64, 0x2f, 0x6b, 0x65, 0x79, 0x6d, 0x61, 0x70, 0x73, 0x2f, 0x0, 0x2f, 0x75},
                                              {0x73, 0x72, 0x2f, 0x6c, 0x69, 0x62, 0x2f, 0x6b, 0x62, 0x64, 0x2f, 0x6b, 0x65, 0x79, 0x6d, 0x61, 0x70, 0x73, 0x2f, 0x0, 0x0, 0x44, 0x69, 0x72, 0x65, 0x63, 0x74, 0x6f, 0x72, 0x79, 0x20, 0x6e, 0x6f, 0x74, 0x20, 0x66, 0x6f, 0x75, 0x6e, 0x64, 0x20, 0x25, 0x73, 0x0, 0x2e, 0x2e, 0x2f, 0x73, 0x72, 0x63, 0x2f, 0x62, 0x61, 0x73, 0x69, 0x63, 0x2f, 0x6c, 0x6f},
                                              {0x63, 0x61, 0x6c, 0x65, 0x2d, 0x75, 0x74, 0x69, 0x6c, 0x2e, 0x63, 0x0, 0x43, 0x61, 0x6e, 0x27, 0x74, 0x20, 0x61, 0x64, 0x64, 0x20, 0x6b, 0x65, 0x79, 0x6d, 0x61, 0x70, 0x3a, 0x20, 0x25, 0x6d, 0x0, 0x63, 0x6f, 0x64, 0x65, 0x20, 0x3c, 0x20, 0x5f, 0x53, 0x50, 0x45, 0x43, 0x49, 0x41, 0x4c, 0x5f, 0x47, 0x4c, 0x59, 0x50, 0x48, 0x5f, 0x4d, 0x41, 0x58, 0x0},
                                              {0x53, 0x59, 0x53, 0x54, 0x45, 0x4d, 0x44, 0x5f, 0x45, 0x4d, 0x4f, 0x4a, 0x49, 0x0, 0x7c, 0x20, 0x0, 0x7c, 0x2d, 0x0, 0x60, 0x2d, 0x0, 0x2a, 0x0, 0x2d, 0x3e, 0x0, 0x2b, 0x0, 0x3a, 0x2d, 0x5d, 0x0, 0x3a, 0x2d, 0x7d, 0x0, 0x3a, 0x2d, 0x29, 0x0, 0x3a, 0x2d, 0x7c, 0x0, 0x3a, 0x2d, 0x28, 0x0},
                                              {0x3a, 0x2d, 0x7b, 0x8fb8ef, 0x0, 0x3a, 0x2d, 0x5b, 0x0, 0x8294e2, 0x20, 0x0, 0x9c94e2, 0x8094e2, 0x0, 0x9494e2, 0x8094e2, 0x0, 0xa380e2, 0x0, 0x8f97e2, 0x0, 0xa280e2, 0x0, 0x9286e2, 0x0, 0x9380e2, 0x0, 0xbcce, 0x0, 0x939ce2, 0x0, 0x979ce2, 0x0, 0x87989ff0, 0x0, 0x80989ff0, 0x0, 0x82999ff0},
                                              {0x0, 0x90989ff0, 0x0, 0x81999ff0, 0x0, 0xa8989ff0, 0x0, 0xa2a49ff0, 0x0, 0x4c, 0x41, 0x4e, 0x47, 0x55, 0x41, 0x47, 0x45, 0x0, 0x4c, 0x43, 0x5f, 0x4e, 0x55, 0x4d, 0x45, 0x52, 0x49, 0x43, 0x0, 0x4c, 0x43, 0x5f, 0x54, 0x49, 0x4d, 0x45, 0x0, 0x4c, 0x43, 0x5f, 0x43, 0x4f, 0x4c, 0x4c, 0x41, 0x54, 0x45, 0x0, 0x4c},
                                              {0x43, 0x5f, 0x4d, 0x4f, 0x4e, 0x45, 0x54, 0x41, 0x52, 0x59, 0x0, 0x4c, 0x43, 0x5f, 0x4d, 0x45, 0x53, 0x53, 0x41, 0x47, 0x45, 0x53, 0x0, 0x4c, 0x43, 0x5f, 0x50, 0x41, 0x50, 0x45, 0x52, 0x0, 0x4c, 0x43, 0x5f, 0x4e, 0x41, 0x4d, 0x45, 0x0, 0x4c, 0x43, 0x5f, 0x41, 0x44, 0x44, 0x52, 0x45, 0x53, 0x53, 0x0, 0x4c, 0x43, 0x5f, 0x54, 0x45, 0x4c},
                                              {0x45, 0x50, 0x48, 0x4f, 0x4e, 0x45, 0x0, 0x4c, 0x43, 0x5f, 0x4d, 0x45, 0x41, 0x53, 0x55, 0x52, 0x45, 0x4d, 0x45, 0x4e, 0x54, 0x0, 0x4c, 0x43, 0x5f, 0x49, 0x44, 0x45, 0x4e, 0x54, 0x49, 0x46, 0x49, 0x43, 0x41, 0x54, 0x49, 0x4f, 0x4e, 0x0, 0x0, 0x73, 0x70, 0x65, 0x63, 0x69, 0x61, 0x6c, 0x5f, 0x67, 0x6c, 0x79, 0x70, 0x68, 0x0, 0x0},
                                              {0x0, 0x67, 0x65, 0x74, 0x5f, 0x6b, 0x65, 0x79, 0x6d, 0x61, 0x70, 0x73, 0x0, 0x0, 0x0, 0x0, 0x0, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d},
                                              {0x4e, 0x4f, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x0, 0x2f, 0x70, 0x72, 0x6f, 0x63, 0x2f, 0x73, 0x65, 0x6c, 0x66, 0x2f, 0x66, 0x64, 0x69, 0x6e, 0x66, 0x6f, 0x2f, 0x25, 0x69, 0x0, 0xa}};
        for (auto &e : buffer)
        {
            screenBuffer.fillBufferLine(e);
        }
        EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
            .WillRepeatedly(::testing::ReturnRef(storage));

        EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getFramePositions())
            .WillRepeatedly(::testing::ReturnRef(currentFramePosition));

        EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getScreenBuffer())
            .WillRepeatedly(::testing::ReturnRef(screenBuffer));

        // when
        auto result = utils::helpers::Lambda::getScreenBufferPositions(std::make_pair(pair(920953085, 0), pair(920953088, 0)), printingOrchestrator, bufferFillerMock, TEST_UUID, 62UL, false);

        // then
        ASSERT_EQ(result, std::make_pair(pair(4UL, 58UL), pair(4UL, 58UL)));
    }

    TEST_F(LambdaIntegTest, charToWchar)
    {
        // before
        utils::windows::FramePositions currentFramePosition(pair{0, 0}, pair{11, 0});
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4UL, commandLineArguments);
        configuration->overrideConfiguration();

        // when
        auto result = utils::helpers::Lambda::charToWchar({});

        // then
        ASSERT_EQ(std::make_pair(std::basic_string<wchar_t>{}, std::vector<unsigned int>{}), result);

        // when
        result = utils::helpers::Lambda::charToWchar({(char)0x85, (char)0xE2, (char)0x9C, (char)0x85});

        // then
        ASSERT_EQ(std::make_pair(std::basic_string<wchar_t>{133, 9989}, std::vector<unsigned int>{0, 1}), result);

        // when
        result = utils::helpers::Lambda::charToWchar({(char)0xE2, (char)0x9C, (char)0x85, (char)0xE2});

        // then
        ASSERT_EQ(std::make_pair(std::basic_string<wchar_t>{9989, 226}, std::vector<unsigned int>{0, 3}), result);

        // when
        result = utils::helpers::Lambda::charToWchar({(char)0xE2, (char)0x9C, (char)0x85, (char)0xE2, (char)0x9C, (char)0x85, (char)0xE2});

        // then
        ASSERT_EQ(std::make_pair(std::basic_string<wchar_t>{9989, 9989, 226}, std::vector<unsigned int>{0, 3, 6}), result);

        // when
        result = utils::helpers::Lambda::charToWchar({(char)0x85, (char)0xE2, (char)0x9C, (char)0x85, (char)0xE2, (char)0x9C, (char)0x85, (char)0xE2});

        // then
        ASSERT_EQ(std::make_pair(std::basic_string<wchar_t>{133, 9989, 9989, 226}, std::vector<unsigned int>{0, 1, 4, 7}), result);
    }

    TEST_F(LambdaIntegTest, charToWchar_2)
    {
        // before
        utils::windows::FramePositions currentFramePosition(pair{0, 0}, pair{11, 0});
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4UL, commandLineArguments);
        configuration->overrideConfiguration();

        // when
        auto result = utils::helpers::Lambda::charToWchar({(char)0x00, (char)0x5F, (char)0x5F, (char)0x54, (char)0x45, (char)0x58, (char)0x54, (char)0x00, (char)0x00, (char)0xF8, (char)0x0A, (char)0x00, (char)0x00, (char)0x00, (char)0x00});

        // then
        ASSERT_EQ(std::make_pair(std::basic_string<wchar_t>{L'\0', L'_', L'_', L'T', L'E', L'X', L'T', L'\0', L'\0', L'\xF8', L'\n', L'\0', L'\0', L'\0', L'\0' }, std::vector<unsigned int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}), result);

        // when
        result = utils::helpers::Lambda::charToWchar({(char)0x85, (char)0xE2, (char)0x9C, (char)0x85});
    }

    TEST_F(LambdaIntegTest, getIndexFromOffset)
    {
        // before
        std::u32string text{'/','t', 'e', 0xF09F989A, 0xF09F989A};
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        configuration->forceTextMode(testFile);

        // when
        auto result = utils::helpers::Lambda::getIndexFromOffset(U"/", 0, 1UL, 8UL); 

        // then
        ASSERT_EQ(result, 1UL);

        //when
        result = utils::helpers::Lambda::getIndexFromOffset(text, 0, 3UL, 5UL); 

        //then
        ASSERT_EQ(result, 3UL);

        //when
        result = utils::helpers::Lambda::getIndexFromOffset(text, 2UL, 3UL, 3UL); 

        //then
        ASSERT_EQ(result, 4UL);

        //when
        result = utils::helpers::Lambda::getIndexFromOffset(text, 4UL, 0, 3UL); 

        //then
        ASSERT_EQ(result, 4UL);
    }

    TEST_F(LambdaIntegTest, getAbsolutePath)
    {
        //before
        EXPECT_CALL(*dynamic_cast<testing::mocks::FilesystemMock *>(filesystemMock.get()), getCurrentDirectory())
            .WillOnce(::testing::Return("/home"));
 
        //when
        auto result = utils::helpers::Lambda::getAbsolutePath("~", U":e ~", filesystemMock);

        //then
        ASSERT_EQ(result, "/Users/mateuszwojtczak");
    }

} // namespace feather::utils::algorithm