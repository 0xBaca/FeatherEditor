#include "action/JumpToBracketAction.hpp"
#include "helpers/TestBase.hpp"
#include "mocks/BufferFillerMock.hpp"
#include "mocks/NCursesMainWindowDecoratorMock.hpp"
#include "mocks/NCursesWindowMock.hpp"
#include "mocks/PrintingOrchestratorMock.hpp"
#include "mocks/WindowsManagerMock.hpp"
#include "printer/FilePrintingStorage.hpp"
#include "printer/PrintingOrchestrator.hpp"
#include "utils/ProgramOptionsParser.hpp"
#include "utils/datatypes/Strings.hpp"
#include "utils/helpers/Conversion.hpp"
#include "utils/storage/AbstractStorage.hpp"
#include "utils/storage/FileStorageFactory.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace ::testing::mocks;

namespace feather::action
{
    class JumpToBracketActionIntegTest : public ::testing::Test, public ::feather::test::TestBase
    {
    protected:
        std::unique_ptr<utils::BufferFillerInterface> bufferFillerMock;
        std::shared_ptr<WindowsManager> windowsManagerMock;
        std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator;
        std::shared_ptr<PrintingStorageInterface> filePrintingStorage;
        std::shared_ptr<MainWindowInterface> nCursesMainWindowDecoratorMock;
        std::shared_ptr<WindowImplInterface> nCursesWindowMock;

    public:
        void SetUp() override
        {
            bufferFillerMock = std::unique_ptr<BufferFillerMock>(new BufferFillerMock());
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

    TEST_F(JumpToBracketActionIntegTest, returnIfEmptyFile)
    {
        //before
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createEmptyStorage("removCharacterWhenNewFile");
        std::unique_ptr<action::JumpToBracketAction> action = std::make_unique<action::JumpToBracketAction>(action::JumpToBracketActionInput(TEST_UUID, pair(2, 0), storage, printingOrchestrator, windowsManagerMock, U'{'));

        //when
        auto result = action->execute();

        //then
        ASSERT_EQ(result.newCursorPosition, std::nullopt);
    }

    TEST_F(JumpToBracketActionIntegTest, returnIfCursorNotAtBracket)
    {
        //before
        std::vector<char> text = FIVE_BYTE_UTF8_CHARACTER;
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("removCharacterWhenNewFile", text);
        std::unique_ptr<action::JumpToBracketAction> action = std::make_unique<action::JumpToBracketAction>(action::JumpToBracketActionInput(TEST_UUID, pair(2, 0), storage, printingOrchestrator, windowsManagerMock, U'{'));

        //when
        auto result = action->execute();

        //then
        ASSERT_EQ(result.newCursorPosition, std::nullopt);
    }

    TEST_F(JumpToBracketActionIntegTest, returnIfAtOpenBracketLookingForAnotherOpenBracket)
    {
        //before
        std::vector<char> text = FIVE_BYTE_UTF8_CHARACTER;
        text[2] = '{';
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("removCharacterWhenNewFile", text);
        std::unique_ptr<action::JumpToBracketAction> action = std::make_unique<action::JumpToBracketAction>(action::JumpToBracketActionInput(TEST_UUID, pair(2, 0), storage, printingOrchestrator, windowsManagerMock, U'<'));

        //when
        auto result = action->execute();

        //then
        ASSERT_EQ(result.newCursorPosition, std::nullopt);
    }

    TEST_F(JumpToBracketActionIntegTest, returnIfAtClosingBracketLookingForAnotherClosingBracket)
    {
        //before
        std::vector<char> text = FIVE_BYTE_UTF8_CHARACTER;
        text[2] = ')';
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("removCharacterWhenNewFile", text);
        std::unique_ptr<action::JumpToBracketAction> action = std::make_unique<action::JumpToBracketAction>(action::JumpToBracketActionInput(TEST_UUID, pair(2, 0), storage, printingOrchestrator, windowsManagerMock,  U'}'));

        //when
        auto result = action->execute();

        //then
        ASSERT_EQ(result.newCursorPosition, std::nullopt);
    }

    TEST_F(JumpToBracketActionIntegTest, atOpeningBracketAndNoCorrespondingClosingBrackets)
    {
        //before
        std::vector<char> text = FIVE_BYTE_UTF8_CHARACTER;
        text.push_back(U'{');
        text.push_back(U'<');
        text.push_back(U'>');
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("removCharacterWhenNewFile", text);
        std::unique_ptr<action::JumpToBracketAction> action = std::make_unique<action::JumpToBracketAction>(action::JumpToBracketActionInput(TEST_UUID, pair(2, 0), storage, printingOrchestrator, windowsManagerMock, U'('));

        //when
        auto result = action->execute();

        //then
        ASSERT_EQ(result.newCursorPosition, std::nullopt);
    }

    TEST_F(JumpToBracketActionIntegTest, jumpToClosingBracket_NoClosingBracket)
    {
        //before
        std::vector<char> text = {'{'};
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("removCharacterWhenNewFile", text);
        std::unique_ptr<action::JumpToBracketAction> action = std::make_unique<action::JumpToBracketAction>(action::JumpToBracketActionInput(TEST_UUID, pair(0, 0), storage, printingOrchestrator, windowsManagerMock, U'('));

        //when
        auto result = action->execute();

        //then
        ASSERT_EQ(result.newCursorPosition, std::nullopt);
    }

    TEST_F(JumpToBracketActionIntegTest, jumpToClosingBracket_ClosingBracketsDontMatch)
    {
        //before
        std::vector<char> text = {'{', '<', '>'};
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("removCharacterWhenNewFile", text);
        std::unique_ptr<action::JumpToBracketAction> action = std::make_unique<action::JumpToBracketAction>(action::JumpToBracketActionInput(TEST_UUID, pair(0, 0), storage, printingOrchestrator, windowsManagerMock, U'('));

        //when
        auto result = action->execute();

        //then
        ASSERT_EQ(result.newCursorPosition, std::nullopt);
    }

    TEST_F(JumpToBracketActionIntegTest, jumpToClosingBracket_DifferentClosingBracket)
    {
        //before
        std::vector<char> text = {'{', '<', '>', '}', '.'};
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("removCharacterWhenNewFile", text);
        std::unique_ptr<action::JumpToBracketAction> action = std::make_unique<action::JumpToBracketAction>(action::JumpToBracketActionInput(TEST_UUID, pair(0, 0), storage, printingOrchestrator, windowsManagerMock, U')'));

        //when
        auto result = action->execute();

        //then
        ASSERT_EQ(result.newCursorPosition, std::nullopt);
    }

    TEST_F(JumpToBracketActionIntegTest, jumpToClosingBracket)
    {
        //before
        std::vector<char> text = {'{', '<', '>', '}', '.'};
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        EXPECT_CALL(*dynamic_cast<testing::mocks::WindowsManagerMock *>(windowsManagerMock.get()), getMainWindow(TEST_UUID))
            .WillRepeatedly(::testing::Return(nCursesMainWindowDecoratorMock));
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getLibraryWindowHandler())
            .WillRepeatedly(::testing::Return(nCursesWindowMock.get()));
        EXPECT_CALL(*dynamic_cast<NCursesWindowMock *>(nCursesWindowMock.get()), getCharacter())
            .WillRepeatedly(::testing::Return(0));
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("removCharacterWhenNewFile", text);
        std::unique_ptr<action::JumpToBracketAction> action = std::make_unique<action::JumpToBracketAction>(action::JumpToBracketActionInput(TEST_UUID, pair(0, 0), storage, printingOrchestrator, windowsManagerMock, U'}'));

        //when
        auto result = action->execute();

        //then
        ASSERT_TRUE(result.newCursorPosition.has_value());
        ASSERT_EQ(result.newCursorPosition.value(), pair(3, 0));
    }

    TEST_F(JumpToBracketActionIntegTest, atClosingBracketAndNoCorrespondingOpenBrackets)
    {
        //before
        std::vector<char> text = FIVE_BYTE_UTF8_CHARACTER;
        text.push_back(U'<');
        text.push_back(U'>');
        text.push_back(U'}');
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
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("removCharacterWhenNewFile", text);
        std::unique_ptr<action::JumpToBracketAction> action = std::make_unique<action::JumpToBracketAction>(action::JumpToBracketActionInput(TEST_UUID, pair(7, 0), storage, printingOrchestrator, windowsManagerMock, U'{'));

        //when
        auto result = action->execute();

        //then
        ASSERT_EQ(result.newCursorPosition, std::nullopt);
    }

    TEST_F(JumpToBracketActionIntegTest, jumpToOpeningBracket_NoOpeningBracket)
    {
        //before
        std::vector<char> text = {'}'};
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("removCharacterWhenNewFile", text);
        std::unique_ptr<action::JumpToBracketAction> action = std::make_unique<action::JumpToBracketAction>(action::JumpToBracketActionInput(TEST_UUID, pair(0, 0), storage, printingOrchestrator, windowsManagerMock, U'('));

        //when
        auto result = action->execute();

        //then
        ASSERT_EQ(result.newCursorPosition, std::nullopt);
    }

    TEST_F(JumpToBracketActionIntegTest, jumpToOpeningBracket_OpeningBracketsDontMatch)
    {
        //before
        std::vector<char> text = {'(', '}', '>'};
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
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("removCharacterWhenNewFile", text);
        std::unique_ptr<action::JumpToBracketAction> action = std::make_unique<action::JumpToBracketAction>(action::JumpToBracketActionInput(TEST_UUID, pair(2, 0), storage, printingOrchestrator, windowsManagerMock, U'<'));

        //when
        auto result = action->execute();

        //then
        ASSERT_EQ(result.newCursorPosition, std::nullopt);
    }

    TEST_F(JumpToBracketActionIntegTest, jumpToOpeningBracket_DifferentOpeningBracket)
    {
        //before
        std::vector<char> text = {'{', '<', '>', '}', '.'};
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("removCharacterWhenNewFile", text);
        std::unique_ptr<action::JumpToBracketAction> action = std::make_unique<action::JumpToBracketAction>(action::JumpToBracketActionInput(TEST_UUID, pair(3, 0), storage, printingOrchestrator, windowsManagerMock, U'('));

        //when
        auto result = action->execute();

        //then
        ASSERT_EQ(result.newCursorPosition, std::nullopt);
    }

    TEST_F(JumpToBracketActionIntegTest, jumpToOpeningBracket)
    {
        //before
        std::vector<char> text = {'{', '<', '>', '}', '.'};
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        EXPECT_CALL(*dynamic_cast<testing::mocks::WindowsManagerMock *>(windowsManagerMock.get()), getMainWindow(TEST_UUID))
            .WillRepeatedly(::testing::Return(nCursesMainWindowDecoratorMock));
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getLibraryWindowHandler())
            .WillRepeatedly(::testing::Return(nCursesWindowMock.get()));
        EXPECT_CALL(*dynamic_cast<NCursesWindowMock *>(nCursesWindowMock.get()), getCharacter())
            .WillRepeatedly(::testing::Return(0));
        EXPECT_CALL(*dynamic_cast<testing::mocks::WindowsManagerMock *>(windowsManagerMock.get()), refreshAllWindows(::testing::_, ::testing::_));
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("removCharacterWhenNewFile", text);
        std::unique_ptr<action::JumpToBracketAction> action = std::make_unique<action::JumpToBracketAction>(action::JumpToBracketActionInput(TEST_UUID, pair(2, 0), storage, printingOrchestrator, windowsManagerMock, U'<'));

        //when
        auto result = action->execute();

        //then
        ASSERT_TRUE(result.newCursorPosition.has_value());
        ASSERT_EQ(result.newCursorPosition.value(), pair(1, 0));

        //when
        //action = std::make_unique<action::JumpToBracketAction>(action::JumpToBracketActionInput(TEST_UUID, pair(3, 0), storage, printingOrchestrator, U'{'));
        //result = action->execute();

        //then
        //ASSERT_TRUE(result.newCursorPosition.has_value());
        //ASSERT_EQ(result.newCursorPosition.value(), pair(0, 0));
    }
} // namespace feather::action