#include "action/PasteAction.hpp"
#include "helpers/TestBase.hpp"
#include "mocks/NCursesMainWindowDecoratorMock.hpp"
#include "mocks/NCursesWindowMock.hpp"
#include "mocks/PrintingVisitorMock.hpp"
#include "mocks/WindowsManagerMock.hpp"
#include "printer/FilePrintingStorage.hpp"
#include "printer/PrintingOrchestrator.hpp"
#include "utils/ProgramOptionsParser.hpp"
#include "utils/storage/AbstractStorage.hpp"
#include "utils/storage/FileStorageFactory.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace ::testing::mocks;

namespace feather::utils::algorithm
{
    class PasteActionIntegTest : public ::testing::Test, public ::feather::test::TestBase
    {
    protected:
        std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator;
        std::shared_ptr<PrintingStorageInterface> filePrintingStorage;
        std::unique_ptr<printer::PrintingVisitorInterface> printingVisitorMock;
        std::shared_ptr<feather::windows::WindowsManager> windowsManagerMock;
        std::unordered_map<utils::datatypes::Uuid, std::list<pair>, utils::datatypes::UuidHasher> breakPoints;
        std::unordered_map<utils::datatypes::Uuid, std::set<pair>, utils::datatypes::UuidHasher> breakPointsAdded;
        std::unordered_map<utils::datatypes::Uuid, std::map<pair, pair>, utils::datatypes::UuidHasher> textToCopyPositionsToSkip;
        std::list<pair>::iterator currBreakPoint;
        std::shared_ptr<MainWindowInterface> nCursesMainWindowDecoratorMock;
        std::shared_ptr<WindowImplInterface> nCursesWindowMock;

    public:
        void SetUp() override
        {
            filePrintingStorage = std::make_shared<FilePrintingStorage>(inMemoryStorageFactory);
            printingOrchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, filePrintingStorage, nullptr, changesStack, filesystemMock);
            printingVisitorMock = std::unique_ptr<PrintingVisitorMock>(new PrintingVisitorMock());
            windowsManagerMock = std::make_shared<WindowsManagerMock>();
            currBreakPoint = breakPoints[TEST_UUID].end();
            nCursesMainWindowDecoratorMock = std::make_shared<NCursesMainWindowDecoratorMock>();
            nCursesWindowMock = std::make_shared<NCursesWindowMock>();
        }

        void TearDown() override
        {
            printingOrchestrator->removeChangesRelatedToWindow(TEST_UUID);
        }
    };
/*
    TEST_F(PasteActionIntegTest, copyAllNewChanges)
    {
        // expect
        EXPECT_CALL(*dynamic_cast<testing::mocks::WindowsManagerMock *>(windowsManagerMock.get()), getMainWindow(TEST_UUID))
            .WillRepeatedly(::testing::Return(nCursesMainWindowDecoratorMock));
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getLibraryWindowHandler())
            .WillRepeatedly(::testing::Return(nCursesWindowMock.get()));
        EXPECT_CALL(*dynamic_cast<NCursesWindowMock *>(nCursesWindowMock.get()), getCharacter())
            .WillRepeatedly(::testing::Return(0));
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getCursorRealPosition(::testing::_))
            .WillOnce(::testing::Return(pair(0, 0)));
        EXPECT_CALL(*dynamic_cast<testing::mocks::WindowsManagerMock *>(windowsManagerMock.get()), refreshAllWindows(::testing::_, ::testing::_))
            .Times(2);

        // before
        std::vector<char> sumCharacter{(char)0xE2, (char)0x88, (char)0x91};
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createEmptyStorage("copyAllNewChanges");
        printingOrchestrator->addChanges(TEST_UUID, storage, pair(0UL, 0UL), sumCharacter, 3UL);
        printingOrchestrator->addChanges(TEST_UUID, storage, pair(0UL, 0UL), sumCharacter, 3UL);
        printingOrchestrator->addChanges(TEST_UUID, storage, pair(0UL, 0UL), sumCharacter, 3UL);

        std::unique_ptr<action::PasteAction> action = std::make_unique<action::PasteAction>(action::PasteActionInput(
            TEST_UUID,
            std::make_pair(std::make_pair(TEST_UUID, "copyAllNewChanges"), std::make_pair(pair(0, 0), pair(0, 9UL))),
            inMemoryStorageFactory,
            pair(0, 3),
            storage,
            printingOrchestrator,
            textToCopyPositionsToSkip,
            breakPoints[TEST_UUID],
            currBreakPoint,
            std::nullopt,
            breakPointsAdded[TEST_UUID],
            windowsManagerMock,
            printingVisitorMock,
            false));

        // when
        auto actionResult = action->execute();

        // then
        auto chunk = printingOrchestrator->getContinousCharacters(pair(0, 0), 18UL, storage, TEST_UUID);
        ASSERT_THAT(chunk.first, ::testing::ContainerEq(std::vector<char>{(char)0xE2, (char)0x88, (char)0x91, (char)0xE2, (char)0x88, (char)0x91, (char)0xE2, (char)0x88, (char)0x91, (char)0xE2, (char)0x88, (char)0x91, (char)0xE2, (char)0x88, (char)0x91, (char)0xE2, (char)0x88, (char)0x91}));
        // ASSERT_THAT(actionResult.positionsToSkipUpdated, ::testing::ContainerEq(std::map<pair, pair>{std::make_pair(pair(0, 6), pair(0, 15))}));
    }

    TEST_F(PasteActionIntegTest, copyAllNewChangesPasteTwice)
    {
        // expect
        EXPECT_CALL(*dynamic_cast<testing::mocks::WindowsManagerMock *>(windowsManagerMock.get()), getMainWindow(TEST_UUID))
            .WillRepeatedly(::testing::Return(nCursesMainWindowDecoratorMock));
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getLibraryWindowHandler())
            .WillRepeatedly(::testing::Return(nCursesWindowMock.get()));
        EXPECT_CALL(*dynamic_cast<NCursesWindowMock *>(nCursesWindowMock.get()), getCharacter())
            .WillRepeatedly(::testing::Return(0));
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getCursorRealPosition(::testing::_))
            .WillOnce(::testing::Return(pair(0, 0)));
        EXPECT_CALL(*dynamic_cast<testing::mocks::WindowsManagerMock *>(windowsManagerMock.get()), refreshAllWindows(::testing::_, ::testing::_))
            .Times(2);

        // before
        std::vector<char> sumCharacter{(char)0xE2, (char)0x88, (char)0x91};
        const char *commandLineArguments[] = {"./feather", "file", "--relax=6", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createEmptyStorage("copyAllNewChanges");
        printingOrchestrator->addChanges(TEST_UUID, storage, pair(0UL, 0UL), sumCharacter, 3UL);
        printingOrchestrator->addChanges(TEST_UUID, storage, pair(0UL, 0UL), sumCharacter, 3UL);
        printingOrchestrator->addChanges(TEST_UUID, storage, pair(0UL, 0UL), sumCharacter, 3UL);

        std::unique_ptr<action::PasteAction> action = std::make_unique<action::PasteAction>(action::PasteActionInput(
            TEST_UUID,
            std::make_pair(std::make_pair(TEST_UUID, "copyAllNewChanges"), std::make_pair(pair(0, 0), pair(0, 9UL))),
            inMemoryStorageFactory,
            pair(0, 3),
            storage,
            printingOrchestrator,
            textToCopyPositionsToSkip,
            breakPoints[TEST_UUID],
            currBreakPoint,
            std::nullopt,
            breakPointsAdded[TEST_UUID],
            windowsManagerMock,
            printingVisitorMock,
            false));

        // when
        auto actionResult = action->execute();

        // then
        auto chunk = printingOrchestrator->getContinousCharacters(pair(0, 0), 18UL, storage, TEST_UUID);
        ASSERT_THAT(chunk.first, ::testing::ContainerEq(std::vector<char>{(char)0xE2, (char)0x88, (char)0x91, (char)0xE2, (char)0x88, (char)0x91, (char)0xE2, (char)0x88, (char)0x91, (char)0xE2, (char)0x88, (char)0x91, (char)0xE2, (char)0x88, (char)0x91, (char)0xE2, (char)0x88, (char)0x91}));
        // ASSERT_THAT(actionResult.positionsToSkipUpdated, ::testing::ContainerEq(std::map<pair, pair>{std::make_pair(pair(0, 6), pair(0, 15))}));

        // when
        action = std::make_unique<action::PasteAction>(action::PasteActionInput(
            TEST_UUID,
            std::make_pair(std::make_pair(TEST_UUID, "copyAllNewChanges"), std::make_pair(pair(0, 0), pair(0, 9UL))),
            inMemoryStorageFactory,
            pair(0, 9),
            storage,
            printingOrchestrator,
            textToCopyPositionsToSkip,
            breakPoints[TEST_UUID],
            currBreakPoint,
            std::nullopt,
            breakPointsAdded[TEST_UUID],
            windowsManagerMock,
            printingVisitorMock,
            false));

        // then
        chunk = printingOrchestrator->getContinousCharacters(pair(0, 0), 18UL, storage, TEST_UUID);
        // ASSERT_THAT(actionResult.positionsToSkipUpdated, ::testing::ContainerEq(std::map<pair, pair>{std::make_pair(pair(0, 6), pair(0, 24))}));
    }
    */
} // namespace feather::utils::algorith