#include "action/RemoveBetweenWhiteCharactersAction.hpp"
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
    class RemoveBetweenWhiteCharactersActionIntegTest : public ::testing::Test, public ::feather::test::TestBase
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
            printingOrchestrator->removeChangesRelatedToWindow(TEST_UUID_2);
            printingOrchestrator->removeChangesRelatedToWindow(TEST_UUID_3);
        }
    };

    TEST_F(RemoveBetweenWhiteCharactersActionIntegTest, emptyFile)
    {
        // before
        std::vector<char> text = FIVE_BYTE_UTF8_CHARACTER;
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createEmptyStorage("emptyFile.fea");

        // when
        std::unique_ptr<action::RemoveBetweenWhiteCharactersAction> action = std::make_unique<action::RemoveBetweenWhiteCharactersAction>(action::RemoveBetweenWhiteCharactersActionInput(TEST_UUID, windowsManagerMock, pair(0, 0), storage, printingOrchestrator));

        // then
        ASSERT_EQ(action->execute().getResult(), pair(0, 0));
    }

    TEST_F(RemoveBetweenWhiteCharactersActionIntegTest, emptyFileAfterRemovingAll)
    {
        // before
        std::vector<char> text = FIVE_BYTE_UTF8_CHARACTER;
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("emptyFileAfterRemovingAll.fea", text);

        // when
        printingOrchestrator->removeCharacters(TEST_UUID, storage, {{pair(0, 0), pair(5, 0)}}, true);
        std::unique_ptr<action::RemoveBetweenWhiteCharactersAction> action = std::make_unique<action::RemoveBetweenWhiteCharactersAction>(action::RemoveBetweenWhiteCharactersActionInput(TEST_UUID, windowsManagerMock, pair(5UL, 0), storage, printingOrchestrator));

        // then
        ASSERT_EQ(action->execute().getResult(), pair(5UL, 0UL));
    }

    TEST_F(RemoveBetweenWhiteCharactersActionIntegTest, oneSingleWhiteChar)
    {
        // before
        std::vector<char> text{'\n'};
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("oneSingleWhiteChar.fea", text);

        // when
        std::unique_ptr<action::RemoveBetweenWhiteCharactersAction> action = std::make_unique<action::RemoveBetweenWhiteCharactersAction>(action::RemoveBetweenWhiteCharactersActionInput(TEST_UUID, windowsManagerMock, pair(0, 0), storage, printingOrchestrator));

        // then
        ASSERT_EQ(action->execute().getResult(), pair(0, 0));
        ASSERT_EQ(printingOrchestrator->getDeletionsPositions(TEST_UUID), (std::map<pair, pair>{}));

        // when
        text = {' '};
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage_1 = createStorage("oneSingleWhiteChar_1.fea", text);
        action.reset(new action::RemoveBetweenWhiteCharactersAction(action::RemoveBetweenWhiteCharactersActionInput(TEST_UUID_2, windowsManagerMock, pair(0, 0), storage_1, printingOrchestrator)));

        // then
        ASSERT_EQ(action->execute().getResult(), pair(0, 0));
        ASSERT_EQ(printingOrchestrator->getDeletionsPositions(TEST_UUID_2), (std::map<pair, pair>{}));

        // when
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage_2 = createEmptyStorage("oneSingleWhiteChar_2.fea");
        action.reset(new action::RemoveBetweenWhiteCharactersAction(action::RemoveBetweenWhiteCharactersActionInput(TEST_UUID_3, windowsManagerMock, pair(0, 0), storage_1, printingOrchestrator)));
        printingOrchestrator->addChanges(TEST_UUID, storage, pair(0, 0), std::vector<char>{'\t'}, 1UL);

        // then
        ASSERT_EQ(action->execute().getResult(), pair(0, 0));
        // ASSERT_EQ(printingOrchestrator->getDeletionsPositions(TEST_UUID_3), (std::map<pair, pair>{std::make_pair(pair(0, 0), pair(1UL, 0))}));
        ASSERT_EQ(printingOrchestrator->getDeletionsPositions(TEST_UUID_3), (std::map<pair, pair>{}));
    }

    TEST_F(RemoveBetweenWhiteCharactersActionIntegTest, betweenWhiteCharacters)
    {
        // expect
        EXPECT_CALL(*dynamic_cast<testing::mocks::WindowsManagerMock *>(windowsManagerMock.get()), getMainWindow(::testing::_))
            .WillRepeatedly(::testing::Return(nCursesMainWindowDecoratorMock));
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getLibraryWindowHandler())
            .WillRepeatedly(::testing::Return(nCursesWindowMock.get()));
        EXPECT_CALL(*dynamic_cast<NCursesWindowMock *>(nCursesWindowMock.get()), getCharacter())
            .WillRepeatedly(::testing::Return(0));

        // before
        std::vector<char> text{'a', '\n', '\n', 'b', '\n'};
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();

        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("betweenWhiteCharacters.fea", text);

        // when
        std::unique_ptr<action::RemoveBetweenWhiteCharactersAction> action = std::make_unique<action::RemoveBetweenWhiteCharactersAction>(action::RemoveBetweenWhiteCharactersActionInput(TEST_UUID, windowsManagerMock, pair(3UL, 0), storage, printingOrchestrator));

        // then
        ASSERT_EQ(action->execute().getResult(), pair(4UL, 0UL));
        ASSERT_EQ(printingOrchestrator->getDeletionsPositions(TEST_UUID), (std::map<pair, pair>{std::make_pair(pair(3UL, 0), pair(4UL, 0))}));

        // when
        text = {'a', '\n', '\n', 'b'};
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage_1 = createStorage("betweenWhiteCharacters_1.fea", text);
        action.reset(new action::RemoveBetweenWhiteCharactersAction(action::RemoveBetweenWhiteCharactersActionInput(TEST_UUID_2, windowsManagerMock, pair(3UL, 0), storage_1, printingOrchestrator)));

        // then
        ASSERT_EQ(action->execute().getResult(), pair(2UL, 0UL));
        ASSERT_EQ(printingOrchestrator->getDeletionsPositions(TEST_UUID_2), (std::map<pair, pair>{std::make_pair(pair(3UL, 0), pair(4UL, 0))}));
    }

    TEST_F(RemoveBetweenWhiteCharactersActionIntegTest, firstWordWithoutNewLine)
    {
        // expect
        EXPECT_CALL(*dynamic_cast<testing::mocks::WindowsManagerMock *>(windowsManagerMock.get()), getMainWindow(::testing::_))
            .WillRepeatedly(::testing::Return(nCursesMainWindowDecoratorMock));
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getLibraryWindowHandler())
            .WillRepeatedly(::testing::Return(nCursesWindowMock.get()));
        EXPECT_CALL(*dynamic_cast<NCursesWindowMock *>(nCursesWindowMock.get()), getCharacter())
            .WillRepeatedly(::testing::Return(0));
        EXPECT_CALL(*dynamic_cast<testing::mocks::WindowsManagerMock *>(windowsManagerMock.get()), refreshAllWindows(::testing::_, ::testing::_));

        // before
        std::vector<char> text = TEN_BYTE_UTF8_CHARACTER;
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();

        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("firstWordWithoutNewLine.fea", text);

        // when
        std::unique_ptr<action::RemoveBetweenWhiteCharactersAction> action = std::make_unique<action::RemoveBetweenWhiteCharactersAction>(action::RemoveBetweenWhiteCharactersActionInput(TEST_UUID, windowsManagerMock, pair(6UL, 0), storage, printingOrchestrator));

        // then
        ASSERT_EQ(action->execute().getResult(), pair(10UL, 0UL));
        ASSERT_EQ(printingOrchestrator->getDeletionsPositions(TEST_UUID), (std::map<pair, pair>{std::make_pair(pair(0, 0), pair(10UL, 0))}));
    }
}