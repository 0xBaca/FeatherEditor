#include "action/SaveChangesAction.hpp"
#include "helpers/TestBase.hpp"
#include "mocks/BufferFillerMock.hpp"
#include "mocks/FilesystemMock.hpp"
#include "mocks/PrintingOrchestratorMock.hpp"
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
    class SaveChangesActionIntegTest : public ::testing::Test, public ::feather::test::TestBase
    {
    protected:
        std::unique_ptr<utils::BufferFillerInterface> bufferFillerMock;
        std::unique_ptr<printer::PrintingVisitorInterface> printingVisitorMock;
        std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator;
        std::shared_ptr<PrintingStorageInterface> filePrintingStorage;
        std::shared_ptr<WindowsManager> windowsManagerMock;

    public:
        void SetUp() override
        {
            bufferFillerMock = std::make_unique<BufferFillerMock>();
            printingVisitorMock = std::unique_ptr<PrintingVisitorMock>(new PrintingVisitorMock());
            filePrintingStorage = std::make_shared<FilePrintingStorage>(inMemoryStorageFactory);
            printingOrchestrator = std::make_shared<PrintingOrchestrator>(inMemoryStorageFactory, filePrintingStorage, nullptr, changesStack, filesystemMock);
            windowsManagerMock = std::make_shared<WindowsManagerMock>();
        }

        void TearDown() override
        {
            printingOrchestrator->removeChangesRelatedToWindow(TEST_UUID);
            inMemoryStorageFactory->removeAllStorages();
        }
    };

    TEST_F(SaveChangesActionIntegTest, noChangesSave)
    {
        // before
        utils::windows::FramePositions framePositions = utils::windows::FramePositions();
        std::vector<char> text = FIVE_BYTE_UTF8_CHARACTER;
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("savedFile", text);
        EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getFramePositions())
            .WillOnce(::testing::ReturnRef(framePositions));
        EXPECT_CALL(*dynamic_cast<FilesystemMock *>(filesystemMock.get()), isFileExist(::testing::_))
            .WillOnce(::testing::Return(true));
        EXPECT_CALL(*dynamic_cast<WindowsManagerMock *>(windowsManagerMock.get()), refreshProgresWindow(::testing::_, ::testing::_, ::testing::_))
            .Times(2UL);

        // when
        std::unique_ptr<action::SaveChangesAction> action = std::make_unique<action::SaveChangesAction>(action::SaveChangesActionInput(printingOrchestrator, inMemoryStorageFactory, TEST_UUID, bufferFillerMock, storage, windowsManagerMock, "savedFileOther", printingVisitorMock, changesStack, U""));
        action->execute(filesystemMock);

        // then
        std::shared_ptr<feather::utils::storage::AbstractStorage> savedStorage = inMemoryStorageFactory->getStorage("savedFile", TEST_UUID);
        ASSERT_EQ(savedStorage->getSize(), 5UL);
        std::vector<char> dataHolder(5UL, 0);
        savedStorage->readChunk(dataHolder, 5UL);
        ASSERT_EQ(dataHolder, FIVE_BYTE_UTF8_CHARACTER);
    }

    TEST_F(SaveChangesActionIntegTest, saveWithChanges)
    {
        // before
        utils::windows::FramePositions framePositions = utils::windows::FramePositions();
        std::vector<char> text = FIVE_BYTE_UTF8_CHARACTER;
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("setCursorInMiddleOfUtf8Character.fea", text);
        EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getFramePositions())
            .WillOnce(::testing::ReturnRef(framePositions));
        EXPECT_CALL(*dynamic_cast<FilesystemMock *>(filesystemMock.get()), isFileExist(::testing::_))
            .WillOnce(::testing::Return(true));
        EXPECT_CALL(*dynamic_cast<WindowsManagerMock *>(windowsManagerMock.get()), refreshProgresWindow(::testing::_, ::testing::_, ::testing::_))
            .Times(1UL);

        // when
        //('t', '\n'), 0xC5, 0x9B, (0xC5, 0x9B), '\n' -- In brackets are deletions
        printingOrchestrator->addChanges(TEST_UUID, storage, pair(1UL, 0UL), std::vector<char>{'\n'}, 1UL);
        printingOrchestrator->removeCharacters(TEST_UUID, storage, pair(0, 0), pair(1, 1));
        printingOrchestrator->addChanges(TEST_UUID, storage, pair(5UL, 0UL), std::vector<char>{'\n'}, 1UL);
        printingOrchestrator->removeCharacters(TEST_UUID, storage, pair(3, 0), pair(5, 0));
        std::unique_ptr<action::SaveChangesAction> action = std::make_unique<action::SaveChangesAction>(action::SaveChangesActionInput(printingOrchestrator, inMemoryStorageFactory, TEST_UUID, bufferFillerMock, storage, windowsManagerMock, "saveWithChanges", printingVisitorMock, changesStack, U""));
        action->execute(filesystemMock);

        // then
        std::shared_ptr<feather::utils::storage::AbstractStorage> savedStorage = inMemoryStorageFactory->getStorage("saveWithChanges", TEST_UUID);
        ASSERT_EQ(savedStorage->getSize(), 3UL);
        std::vector<char> dataHolder(3UL, 0);
        savedStorage->readChunk(dataHolder, 3UL);
        ASSERT_EQ(dataHolder, (std::vector{(char)0xC5, (char)0x9B, '\n'}));
    }

    TEST_F(SaveChangesActionIntegTest, saveWithChangesAndDeletionsUntilFileEnd)
    {
        // before
        utils::windows::FramePositions framePositions = utils::windows::FramePositions();
        std::vector<char> text = FIVE_BYTE_UTF8_CHARACTER;
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createStorage("setCursorInMiddleOfUtf8Character.fea", text);
        EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getFramePositions())
            .WillOnce(::testing::ReturnRef(framePositions));
        EXPECT_CALL(*dynamic_cast<FilesystemMock *>(filesystemMock.get()), isFileExist(::testing::_))
            .WillOnce(::testing::Return(true));
        EXPECT_CALL(*dynamic_cast<WindowsManagerMock *>(windowsManagerMock.get()), refreshProgresWindow(::testing::_, ::testing::_, ::testing::_))
            .Times(1UL);

        // when
        //('t', '\n'), 0xC5, 0x9B, (0xC5, 0x9B, '\n') -- In brackets are deletions
        printingOrchestrator->addChanges(TEST_UUID, storage, pair(1UL, 0UL), std::vector<char>{'\n'}, 1UL);
        printingOrchestrator->removeCharacters(TEST_UUID, storage, pair(0, 0), pair(1, 1));
        printingOrchestrator->addChanges(TEST_UUID, storage, pair(5UL, 0UL), std::vector<char>{'\n'}, 1UL);
        printingOrchestrator->removeCharacters(TEST_UUID, storage, pair(3, 0), pair(6, 0));
        std::unique_ptr<action::SaveChangesAction> action = std::make_unique<action::SaveChangesAction>(action::SaveChangesActionInput(printingOrchestrator, inMemoryStorageFactory, TEST_UUID, bufferFillerMock, storage, windowsManagerMock, "saveWithChanges", printingVisitorMock, changesStack, U""));
        action->execute(filesystemMock);

        // then
        std::shared_ptr<feather::utils::storage::AbstractStorage> savedStorage = inMemoryStorageFactory->getStorage("saveWithChanges", TEST_UUID);
        ASSERT_EQ(savedStorage->getSize(), 2UL);
        std::vector<char> dataHolder(2UL, 0);
        savedStorage->readChunk(dataHolder, 2UL);
        ASSERT_EQ(dataHolder, (std::vector{(char)0xC5, (char)0x9B}));
    }
} // namespace feather::utils::algorithm