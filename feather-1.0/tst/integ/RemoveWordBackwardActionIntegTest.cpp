#include "action/RemoveWordBackwardAction.hpp"
#include "helpers/TestBase.hpp"
#include "mocks/NCursesMainWindowDecoratorMock.hpp"
#include "mocks/NCursesWindowMock.hpp"
#include "mocks/WindowsManagerMock.hpp"
#include "printer/FilePrintingStorage.hpp"
#include "printer/PrintingOrchestrator.hpp"
#include "utils/ProgramOptionsParser.hpp"
#include "utils/storage/AbstractStorage.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace ::testing::mocks;

namespace feather::utils::algorithm
{
    class RemoveWordBackwardActionIntegTest : public ::testing::Test, public ::feather::test::TestBase
    {
    protected:
        std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator;
        std::shared_ptr<printer::PrintingStorageInterface> filePrintingStorage;
        std::shared_ptr<WindowsManager> windowsManagerMock;
        std::shared_ptr<MainWindowInterface> nCursesMainWindowDecoratorMock;
        std::shared_ptr<WindowImplInterface> nCursesWindowMock;

    public:
        void SetUp() override
        {
            filePrintingStorage = std::make_shared<printer::FilePrintingStorage>(inMemoryStorageFactory);
            printingOrchestrator = std::make_shared<printer::PrintingOrchestrator>(inMemoryStorageFactory, filePrintingStorage, nullptr, changesStack, filesystemMock);
            windowsManagerMock = std::make_shared<WindowsManagerMock>();
            nCursesMainWindowDecoratorMock = std::make_shared<NCursesMainWindowDecoratorMock>();
            nCursesWindowMock = std::make_shared<NCursesWindowMock>();
        }

        void TearDown() override
        {
            printingOrchestrator->removeChangesRelatedToWindow(TEST_UUID);
        }
    };

    TEST_F(RemoveWordBackwardActionIntegTest, removeCharacterWhenNewFile)
    {
        // before
        std::vector<char> text = FIVE_BYTE_UTF8_CHARACTER;
        text[1UL] = '\n';
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createEmptyStorage("removCharacterWhenNewFile");

        // when
        std::unique_ptr<action::RemoveWordBackwardAction> action = std::make_unique<action::RemoveWordBackwardAction>(action::RemoveWordBackwardActionInput(TEST_UUID, windowsManagerMock, pair(0, 0), storage, printingOrchestrator));

        // then
        ASSERT_EQ(action->execute().getResult(), pair(0, 0));
    }

    TEST_F(RemoveWordBackwardActionIntegTest, beforeCharacterNotBelongingToWord)
    {
        // expect
        EXPECT_CALL(*dynamic_cast<testing::mocks::WindowsManagerMock *>(windowsManagerMock.get()), getMainWindow(TEST_UUID))
            .WillRepeatedly(::testing::Return(nCursesMainWindowDecoratorMock));
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getLibraryWindowHandler())
            .WillRepeatedly(::testing::Return(nCursesWindowMock.get()));
        EXPECT_CALL(*dynamic_cast<NCursesWindowMock *>(nCursesWindowMock.get()), getCharacter())
            .WillRepeatedly(::testing::Return(0));
        EXPECT_CALL(*dynamic_cast<testing::mocks::WindowsManagerMock *>(windowsManagerMock.get()), refreshAllWindows(::testing::_, ::testing::_));

        // before
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("beforeCharacterNotBelongingToWord", FIVE_BYTE_UTF8_CHARACTER);

        // when
        printingOrchestrator->addChanges(TEST_UUID, storage, pair(1UL, 0), std::vector<char>{'='}, 1UL);
        std::unique_ptr<action::RemoveWordBackwardAction> action = std::make_unique<action::RemoveWordBackwardAction>(action::RemoveWordBackwardActionInput(TEST_UUID, windowsManagerMock, pair(1UL, 1UL), storage, printingOrchestrator));

        // then
        ASSERT_EQ(action->execute().getResult(), pair(1UL, 1UL));
    }

    TEST_F(RemoveWordBackwardActionIntegTest, notBelongToWordAtLastPos)
    {
        // expect
        EXPECT_CALL(*dynamic_cast<testing::mocks::WindowsManagerMock *>(windowsManagerMock.get()), getMainWindow(TEST_UUID))
            .WillRepeatedly(::testing::Return(nCursesMainWindowDecoratorMock));
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getLibraryWindowHandler())
            .WillRepeatedly(::testing::Return(nCursesWindowMock.get()));
        EXPECT_CALL(*dynamic_cast<NCursesWindowMock *>(nCursesWindowMock.get()), getCharacter())
            .WillRepeatedly(::testing::Return(0));
        EXPECT_CALL(*dynamic_cast<testing::mocks::WindowsManagerMock *>(windowsManagerMock.get()), refreshAllWindows(::testing::_, ::testing::_));

        // before
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("notBelongToWordAtLastPos", std::vector<char>{'t', 'e', 's', 't', ')'});

        // when
        printingOrchestrator->addChanges(TEST_UUID, storage, pair(5UL, 0), std::vector<char>{'\n'}, 1UL);
        std::unique_ptr<action::RemoveWordBackwardAction> action = std::make_unique<action::RemoveWordBackwardAction>(action::RemoveWordBackwardActionInput(TEST_UUID, windowsManagerMock, pair(5UL, 0UL), storage, printingOrchestrator));

        // then
        ASSERT_EQ(action->execute().getResult(), pair(5UL, 0));
    }

    TEST_F(RemoveWordBackwardActionIntegTest, atBeginingOfFile)
    {
        // before
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("atBeginingOfFile", std::vector<char>{'t', 'e', 's', 't', ')'});

        // when
        std::unique_ptr<action::RemoveWordBackwardAction> action = std::make_unique<action::RemoveWordBackwardAction>(action::RemoveWordBackwardActionInput(TEST_UUID, windowsManagerMock, pair(0, 0), storage, printingOrchestrator));

        // then
        ASSERT_EQ(action->execute().getResult(), pair(0, 0));
    }
}