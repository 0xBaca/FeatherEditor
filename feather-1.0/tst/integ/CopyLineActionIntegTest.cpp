#include "action/CopyLineAction.hpp"
#include "helpers/TestBase.hpp"
#include "mocks/NCursesMainWindowDecoratorMock.hpp"
#include "mocks/NCursesWindowMock.hpp"
#include "mocks/PrintingOrchestratorMock.hpp"
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
    class CopyLineActionIntegTest : public ::testing::Test, public ::feather::test::TestBase
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
            inMemoryStorageFactory->removeAllStorages();
        }
    };

    TEST_F(CopyLineActionIntegTest, copyLineWhenEmptyFile)
    {
        // before
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();

        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createEmptyStorage("copyLineWhenEmptyFile.fea");

        // when
        std::unique_ptr<action::CopyLineAction> action = std::make_unique<action::CopyLineAction>(action::CopyLineActionInput(TEST_UUID, pair(0, 0), storage, printingOrchestrator, windowsManagerMock));

        // then
        ASSERT_EQ(action->execute().getResult(), std::nullopt);
    }

    TEST_F(CopyLineActionIntegTest, copyNewLineOnly)
    {
        // before
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();

        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("copyLineWhenEmptyFile.fea", std::vector<char>{'\n'});

        // when
        std::unique_ptr<action::CopyLineAction> action = std::make_unique<action::CopyLineAction>(action::CopyLineActionInput(TEST_UUID, pair(0, 0), storage, printingOrchestrator, windowsManagerMock));

        // then
        ASSERT_NE(action->execute().getResult(), std::nullopt);
        ASSERT_EQ(action->execute().getResult().value(), std::make_pair(pair(0, 0), pair(1, 0)));
    }

    TEST_F(CopyLineActionIntegTest, getLine)
    {
        // before
        auto data = TWENTY_BYTE_UTF8_CHARACTER;
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();

        EXPECT_CALL(*dynamic_cast<testing::mocks::WindowsManagerMock *>(windowsManagerMock.get()), getMainWindow(TEST_UUID))
            .WillRepeatedly(::testing::Return(nCursesMainWindowDecoratorMock));
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getLibraryWindowHandler())
            .WillRepeatedly(::testing::Return(nCursesWindowMock.get()));
        EXPECT_CALL(*dynamic_cast<NCursesWindowMock *>(nCursesWindowMock.get()), getCharacter())
            .WillRepeatedly(::testing::Return(0));
        EXPECT_CALL(*dynamic_cast<testing::mocks::WindowsManagerMock *>(windowsManagerMock.get()), refreshAllWindows(::testing::_, ::testing::_))
            .Times(2);

        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("getLine.fea", data);

        // when
        printingOrchestrator->addChanges(TEST_UUID, storage, pair(6UL, 0UL), std::vector<char>{'\n'}, 1UL);
        std::unique_ptr<action::CopyLineAction> action = std::make_unique<action::CopyLineAction>(action::CopyLineActionInput(TEST_UUID, pair(2UL, 0), storage, printingOrchestrator, windowsManagerMock));

        // then
        ASSERT_NE(action->execute().getResult(), std::nullopt);
        ASSERT_EQ(action->execute().getResult().value(), std::make_pair(pair(0, 0), pair(6UL, 1UL)));

        // when
        action = std::make_unique<action::CopyLineAction>(action::CopyLineActionInput(TEST_UUID, pair(16UL, 0), storage, printingOrchestrator, windowsManagerMock));

        // then
        ASSERT_EQ(action->execute().getResult().value(), std::make_pair(pair(6UL, 1UL), pair(16UL, 0)));
    }
}