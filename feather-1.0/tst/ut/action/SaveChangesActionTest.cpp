#include "action/SaveChangesAction.hpp"
#include "mocks/BufferFillerMock.hpp"
#include "mocks/FilesystemMock.hpp"
#include "mocks/NCursesMainWindowDecoratorMock.hpp"
#include "mocks/NCursesWindowMock.hpp"
#include "mocks/NCursesWrapperMock.hpp"
#include "mocks/PrintingOrchestratorMock.hpp"
#include "mocks/WindowsManagerMock.hpp"
#include "utils/BufferFillerInterface.hpp"
#include "utils/storage/FileStorageFactory.hpp"
#include "utils/storage/InMemoryStorageFactory.hpp"
#include "windows/WindowsManager.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace ::testing::mocks;

namespace feather::action
{
class SaveChangesActionTest : public ::testing::Test
{
protected:
    static constexpr const char *testUUID = "220097a9-67fc-4110-b92e-c60227359c68";
    utils::datatypes::Uuid TEST_UUID;
    std::shared_ptr<feather::utils::storage::AbstractStorageFactory> storageFactory;
    std::shared_ptr<feather::utils::storage::AbstractStorageFactory> inMemoryStorageFactory;
    std::unique_ptr<BufferFillerInterface> bufferFillerMock;
    std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestratorMock;
    std::shared_ptr<windows::WindowsManager> windowsManagerMock;
    std::unique_ptr<MainWindowInterface> nCursesMainWindowDecoratorMock;

public:
    void SetUp() override
    {
        storageFactory = std::make_shared<feather::utils::storage::FileStorageFactory>();
        inMemoryStorageFactory = std::make_shared<feather::utils::storage::InMemoryStorageFactory>();
        bufferFillerMock = std::make_unique<BufferFillerMock>();
        printingOrchestratorMock = std::make_shared<PrintingOrchestratorMock>();
        windowsManagerMock = std::make_shared<WindowsManagerMock>();
        nCursesMainWindowDecoratorMock = std::make_unique<NCursesMainWindowDecoratorMock>();
        TEST_UUID = utils::datatypes::Uuid(testUUID);
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getBufferFiller())
            .WillRepeatedly(::testing::ReturnRef(bufferFillerMock));
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getUUID(false))
            .WillRepeatedly(::testing::ReturnRef(TEST_UUID));
    }
};

/*
    5 bytes at file begining are removed and pos 5 has 5 changes. Frame starts at pos 5 : 5.
    5 Chnages are moved and 6 added so file size should increase by 1 byte
*/
/*
TEST_F(SaveChangesActionTest, saveChangesActionTestReplaceFile)
{
    //before
    std::shared_ptr<feather::utils::storage::AbstractStorage> data = storageFactory->getStorage("./tst/ut/helpers/testfiles/Utf8TestFile", TEST_UUID);
    std::shared_ptr<feather::utils::storage::AbstractStorage> storage = inMemoryStorageFactory->getStorage("./tst/ut/helpers/testfiles/Utf8TestFile", TEST_UUID);
    storage->copy(data);
    storage->setCursor(0);
    utils::windows::FramePositions framePositions{5,5,20,0};
    std::unique_ptr<utils::windows::FramePositions> arbitraryPos = std::make_unique<utils::windows::FramePositions>(5,5,20,0);
    EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
        .WillRepeatedly(::testing::ReturnRef(storage));
    //EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), convertRealPositionToByteOffset(std::pair<size_t, size_t>(5, 5), TEST_UUID))
    //    .WillOnce(::testing::Return(5));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isAnyChangesMade(TEST_UUID))
        .WillOnce(::testing::Return(true));
    EXPECT_CALL(*dynamic_cast<BufferFillerMock*>(bufferFillerMock.get()), getFramePositions())
        .WillRepeatedly(::testing::ReturnRef(framePositions));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNumberOfDeletedBytesAtDummyDeletions(TEST_UUID))
        .WillOnce(::testing::Return(5));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNumberOfDeletedBytesAtPos(::testing::_, TEST_UUID))
        .WillRepeatedly(::testing::Return(0));
    ON_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosWithChanges(::testing::_, TEST_UUID))
        .WillByDefault(::testing::Return(false));
    ON_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosWithChanges(5, TEST_UUID))
        .WillByDefault(::testing::Return(true));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosWithChanges(::testing::_, TEST_UUID))
        .Times(::testing::AnyNumber());
    ON_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getDiffToNextChange(testing::_, TEST_UUID))
        .WillByDefault(::testing::Return(1));
    ON_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getDiffToNextChange(5, TEST_UUID))
        .WillByDefault(::testing::Return(0));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getDiffToNextChange(::testing::_, TEST_UUID))
        .Times(::testing::AnyNumber());
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock*>(printingOrchestratorMock.get()), isCharAtPosWithDeletions(::testing::_, TEST_UUID))
        .WillRepeatedly(::testing::Return(false));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNewChanges(::testing::_, 5, ::testing::_, ::testing::_))
        .WillOnce(::testing::Return(std::vector<char>{U'A', U'B', U'C', U'D', U'E', U'F'}))
        .WillOnce(::testing::Return(std::vector<char>{}));

    //when
    auto result = std::make_unique<SaveChangesAction>(SaveChangesActionInput(printingOrchestratorMock, inMemoryStorageFactory, nCursesMainWindowDecoratorMock, windowsManagerMock, storage->getName()))->execute(arbitraryPos, std::make_unique<testing::mocks::FilesystemMock>());

    //then
    //auto replacedStorage = dynamic_cast<BufferFillerMock*>(bufferFillerMock.get())->storage;
    //ASSERT_EQ(replacedStorage->getSize(), 13459U);
}

TEST_F(SaveChangesActionTest, saveChangesActionNewFileReplaceFile)
{
    //before
    std::shared_ptr<feather::utils::storage::AbstractStorage> data = storageFactory->getStorage("./tst/ut/helpers/testfiles/emptyFile", TEST_UUID);
    std::shared_ptr<feather::utils::storage::AbstractStorage> storage = inMemoryStorageFactory->getStorage("./tst/ut/helpers/testfiles/emptyFile", TEST_UUID);
    storage->copy(data);
    utils::windows::FramePositions framePositions{0,0,0,0};
    std::unique_ptr<utils::windows::FramePositions> arbitraryPos = std::make_unique<utils::windows::FramePositions>(0,0,0,0);
    //EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), convertRealPositionToByteOffset(std::pair<size_t, size_t>(0, 0), TEST_UUID))
    //    .WillOnce(::testing::Return(0));
    EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
        .WillOnce(::testing::ReturnRef(storage));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isAnyChangesMade(TEST_UUID))
        .WillOnce(::testing::Return(true));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNumberOfDummyDeletions(TEST_UUID))
        .WillRepeatedly(::testing::Return(0));
    EXPECT_CALL(*dynamic_cast<BufferFillerMock*>(bufferFillerMock.get()), getFramePositions())
        .WillRepeatedly(::testing::ReturnRef(framePositions));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNumberOfDeletedBytesAtDummyDeletions(TEST_UUID))
        .WillOnce(::testing::Return(0));
         
    ON_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosWithChanges(::testing::_, TEST_UUID))
        .WillByDefault(::testing::Return(false));
    ON_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosWithChanges(0, TEST_UUID))
        .WillByDefault(::testing::Return(true));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosWithChanges(::testing::_, TEST_UUID))
        .Times(::testing::AnyNumber());
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock*>(printingOrchestratorMock.get()), isCharAtPosWithDeletions(::testing::_, TEST_UUID))
        .WillRepeatedly(::testing::Return(false));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNewChanges(::testing::_, 0, ::testing::_, ::testing::_))
        .WillOnce(::testing::Return(std::vector<char>{U'A', U'B', U'C', U'D', U'E', U'F', U'\n'}))
        .WillOnce(::testing::Return(std::vector<char>{}));

    //when
    auto result = std::make_unique<SaveChangesAction>(SaveChangesActionInput(printingOrchestratorMock, inMemoryStorageFactory, nCursesMainWindowDecoratorMock, windowsManagerMock, storage->getName()))->execute(arbitraryPos, std::make_unique<testing::mocks::FilesystemMock>());

    //then
    //auto replacedStorage = dynamic_cast<BufferFillerMock*>(bufferFillerMock.get())->storage;
    //ASSERT_EQ(replacedStorage->getSize(), 7U);
}
*/
}
