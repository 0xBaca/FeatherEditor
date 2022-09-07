#include "action/MoveToNextWordAction.hpp"
#include "helpers/TestBase.hpp"
#include "mocks/NCursesMainWindowDecoratorMock.hpp"
#include "mocks/NCursesWindowMock.hpp"
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
    class MoveToNextWordActionIntegTest : public ::testing::Test, public ::feather::test::TestBase
    {
    protected:
        std::shared_ptr<WindowsManager> windowsManagerMock;
        std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator;
        std::shared_ptr<printer::PrintingStorageInterface> filePrintingStorage;
        std::shared_ptr<MainWindowInterface> nCursesMainWindowDecoratorMock;
        std::shared_ptr<WindowImplInterface> nCursesWindowMock;

    public:
        void SetUp() override
        {
            windowsManagerMock = std::make_shared<WindowsManagerMock>();
            filePrintingStorage = std::make_shared<printer::FilePrintingStorage>(inMemoryStorageFactory);
            printingOrchestrator = std::make_shared<printer::PrintingOrchestrator>(inMemoryStorageFactory, filePrintingStorage, nullptr, changesStack, filesystemMock);
            nCursesMainWindowDecoratorMock = std::make_shared<NCursesMainWindowDecoratorMock>();
            nCursesWindowMock = std::make_shared<NCursesWindowMock>();
        }

        void TearDown() override
        {
            printingOrchestrator->removeChangesRelatedToWindow(TEST_UUID);
        }
    };

    TEST_F(MoveToNextWordActionIntegTest, moveWhenEmptyFile)
    {
        // before
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();

        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createEmptyStorage("copyLineWhenEmptyFile.fea");

        // when
        std::unique_ptr<action::MoveToNextWordAction> action = std::make_unique<action::MoveToNextWordAction>(action::MoveToNextWordActionInput(TEST_UUID, pair(0, 0), storage, printingOrchestrator, windowsManagerMock));

        // then
        ASSERT_EQ(action->execute().getResult(), pair(0, 0));
    }

    TEST_F(MoveToNextWordActionIntegTest, moveAllCharactersBelongToWord)
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

        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("moveAllCharactersBelongToWord.fea", TWENTY_BYTE_UTF8_CHARACTER);

        // when
        std::unique_ptr<action::MoveToNextWordAction> action = std::make_unique<action::MoveToNextWordAction>(action::MoveToNextWordActionInput(TEST_UUID, pair(0, 0), storage, printingOrchestrator, windowsManagerMock));

        // then
        ASSERT_EQ(action->execute().getResult(), pair(16UL, 0));
    }

    TEST_F(MoveToNextWordActionIntegTest, moveWhenAtPosNotBelongingToWord)
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

        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("moveAllCharactersBelongToWord.fea", TWENTY_BYTE_UTF8_CHARACTER);

        // when
        printingOrchestrator->addChanges(TEST_UUID, storage, pair(0UL, 0UL), std::vector<char>{'\n'}, 1UL);
        std::unique_ptr<action::MoveToNextWordAction> action = std::make_unique<action::MoveToNextWordAction>(action::MoveToNextWordActionInput(TEST_UUID, pair(0, 0), storage, printingOrchestrator, windowsManagerMock));

        // then
        ASSERT_EQ(action->execute().getResult(), pair(0, 1UL));
    }
}