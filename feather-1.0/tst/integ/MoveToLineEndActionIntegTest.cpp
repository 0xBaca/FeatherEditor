#include "action/MoveToLineEndAction.hpp"
#include "helpers/TestBase.hpp"
#include "mocks/NCursesMainWindowDecoratorMock.hpp"
#include "mocks/NCursesWindowMock.hpp"
#include "mocks/PrintingOrchestratorMock.hpp"
#include "mocks/WindowsManagerMock.hpp"
#include "printer/FilePrintingStorage.hpp"
#include "printer/PrintingOrchestrator.hpp"
#include "utils/ProgramOptionsParser.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace ::testing::mocks;

namespace feather::utils::algorithm
{
    class MoveToLineEndActionIntegTest : public ::testing::Test, public ::feather::test::TestBase
    {
    protected:
        std::shared_ptr<WindowsManager> windowsManagerMock;
        std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator;
        std::shared_ptr<PrintingStorageInterface> filePrintingStorage;
        std::shared_ptr<MainWindowInterface> nCursesMainWindowDecoratorMock;
        std::shared_ptr<WindowImplInterface> nCursesWindowMock;

    public:
        void SetUp() override
        {
            windowsManagerMock = std::make_shared<WindowsManagerMock>();
            filePrintingStorage = std::make_shared<FilePrintingStorage>(inMemoryStorageFactory);
            printingOrchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, filePrintingStorage, nullptr, changesStack, filesystemMock);
            nCursesMainWindowDecoratorMock = std::make_shared<NCursesMainWindowDecoratorMock>();
            nCursesWindowMock = std::make_shared<NCursesWindowMock>();
        }

        void TearDown() override
        {
            printingOrchestrator->removeChangesRelatedToWindow(TEST_UUID);
        }
    };

    TEST_F(MoveToLineEndActionIntegTest, moveWhenEmptyFile)
    {
        // before
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();

        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createEmptyStorage("moveWhenFileEmpty");

        // when
        std::unique_ptr<action::MoveToLineEndAction> action = std::make_unique<action::MoveToLineEndAction>(action::MoveToLineEndActionInput(TEST_UUID, pair(0, 0), storage, printingOrchestrator, windowsManagerMock));

        // then
        ASSERT_EQ(action->execute().getResult(), pair(0, 0));
    }

    TEST_F(MoveToLineEndActionIntegTest, noNewLines)
    {
        // before
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        EXPECT_CALL(*dynamic_cast<testing::mocks::WindowsManagerMock *>(windowsManagerMock.get()), getMainWindow(TEST_UUID))
            .WillRepeatedly(::testing::Return(nCursesMainWindowDecoratorMock));
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getLibraryWindowHandler())
            .WillRepeatedly(::testing::Return(nCursesWindowMock.get()));
        EXPECT_CALL(*dynamic_cast<NCursesWindowMock*>(nCursesWindowMock.get()), getCharacter())
            .WillRepeatedly(::testing::Return(' '));
 
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("noNewLines", TWENTY_BYTE_UTF8_CHARACTER);

        // when
        std::unique_ptr<action::MoveToLineEndAction> action = std::make_unique<action::MoveToLineEndAction>(action::MoveToLineEndActionInput(TEST_UUID, pair(0, 0), storage, printingOrchestrator, windowsManagerMock));

        // then
        ASSERT_EQ(action->execute().getResult(), pair(16UL, 0));
    }

    TEST_F(MoveToLineEndActionIntegTest, moveToNewLine)
    {
        // before
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();

        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("moveAllCharactersBelongToWord.fea", TWENTY_BYTE_UTF8_CHARACTER);
        EXPECT_CALL(*dynamic_cast<testing::mocks::WindowsManagerMock *>(windowsManagerMock.get()), getMainWindow(TEST_UUID))
            .WillRepeatedly(::testing::Return(nCursesMainWindowDecoratorMock));
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getLibraryWindowHandler())
            .WillRepeatedly(::testing::Return(nCursesWindowMock.get()));
        EXPECT_CALL(*dynamic_cast<NCursesWindowMock*>(nCursesWindowMock.get()), getCharacter())
            .WillRepeatedly(::testing::Return(' '));
 
        // when
        printingOrchestrator->addChanges(TEST_UUID, storage, pair(2UL, 0UL), std::vector<char>{'\n'}, 1UL);
        std::unique_ptr<action::MoveToLineEndAction> action = std::make_unique<action::MoveToLineEndAction>(action::MoveToLineEndActionInput(TEST_UUID, pair(0, 0), storage, printingOrchestrator, windowsManagerMock));

        // then
        ASSERT_EQ(action->execute().getResult(), pair(1, 0UL));
    }

    TEST_F(MoveToLineEndActionIntegTest, alreadyAtNewLine)
    {
        // before
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();

        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("moveAllCharactersBelongToWord.fea", TWENTY_BYTE_UTF8_CHARACTER);

        // when
        printingOrchestrator->addChanges(TEST_UUID, storage, pair(2UL, 0UL), std::vector<char>{'\n'}, 1UL);
        std::unique_ptr<action::MoveToLineEndAction> action = std::make_unique<action::MoveToLineEndAction>(action::MoveToLineEndActionInput(TEST_UUID, pair(2UL, 0), storage, printingOrchestrator, windowsManagerMock));

        // then
        ASSERT_EQ(action->execute().getResult(), pair(2UL, 0UL));
    }
} // namespace feather::utils::algorithm