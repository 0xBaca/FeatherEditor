#include "mocks/BufferFillerMock.hpp"
#include "mocks/NCursesMainWindowDecoratorMock.hpp"
#include "mocks/NCursesWindowMock.hpp"
#include "mocks/NCursesWrapperMock.hpp"
#include "mocks/PrintingOrchestratorMock.hpp"
#include "mocks/StorageMock.hpp"
#include "mocks/WindowsManagerMock.hpp"
#include "action/RemoveLineAction.hpp"
#include "utils/BufferFillerInterface.hpp"
#include "utils/storage/FileStorageFactory.hpp"
#include "utils/storage/InMemoryStorageFactory.hpp"
#include "windows/WindowsManager.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

extern std::unique_ptr<const feather::config::Configuration> configuration;

using namespace ::testing::mocks;

namespace feather::action
{
class RemoveLineActionTest : public ::testing::Test
{
protected:
    static constexpr const char *testUUID = "220097a9-67fc-4110-b92e-c60227359c68";
    std::string testFile;
    utils::datatypes::Uuid TEST_UUID;
    std::shared_ptr<feather::utils::storage::AbstractStorageFactory> storageFactory;
    std::shared_ptr<feather::utils::storage::AbstractStorageFactory> inMemoryStorageFactory;
    std::unique_ptr<BufferFillerInterface> bufferFillerMock;
    std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestratorMock;
    std::shared_ptr<windows::WindowsManager> windowsManagerMock;
    std::unique_ptr<MainWindowInterface> nCursesMainWindowDecoratorMock;
    std::shared_ptr<feather::utils::storage::AbstractStorage> storageMock;

public:
    void SetUp() override
    {
        testFile = "test";
        storageMock = std::make_shared<StorageMock>();
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
TEST_F(RemoveLineActionTest, removeLineWhenAtNewLine)
{
    //before
    configuration->forceBinaryMode(testFile);
    std::unique_ptr<RemoveLineAction> action = std::make_unique<RemoveLineAction>(RemoveLineActionInput(nCursesMainWindowDecoratorMock, printingOrchestratorMock));
    EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getCursorRealPositionMock(::testing::_))
        .WillOnce(::testing::Return(pair(0, 0)));
    EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
        .WillRepeatedly(::testing::ReturnRef(storageMock));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPos(pair(0, 0), ::testing::_, ::testing::_, ::testing::_))
        .WillOnce(::testing::Return(true));
    EXPECT_CALL(*dynamic_cast<StorageMock *>(storageMock.get()), getSize())
        .WillOnce(::testing::Return(0));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNumberOfDeletedBytesAtDummyDeletions(TEST_UUID))
        .WillOnce(::testing::Return(0UL));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getLastValidPosition(TEST_UUID, storageMock))
        .WillOnce(::testing::Return(pair(0, 0)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNextVirtualPosition(TEST_UUID, storageMock, pair(0, 0)))
        .WillOnce(::testing::Return(pair(1, 0)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getPreviousVirtualPosition(TEST_UUID, storageMock, pair(1, 0)))
        .WillOnce(::testing::Return(pair(0, 0)));
    //EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), removeCharacter(::testing::_, std::optional<pair>(pair(1, 0))));

    //when
    auto result = action->execute();

    //then
    auto properCursorPos = pair(0ULL, 0ULL);
    ASSERT_EQ(result.getResult(), properCursorPos);
}

TEST_F(RemoveLineActionTest, lastLineWithoutNewLineCharacterAtTheEnd)
{
    //before
    configuration->forceBinaryMode(testFile);
    std::unique_ptr<RemoveLineAction> action = std::make_unique<RemoveLineAction>(RemoveLineActionInput(nCursesMainWindowDecoratorMock, printingOrchestratorMock));
    EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getCursorRealPositionMock(::testing::_))
        .WillOnce(::testing::Return(pair(1, 0)));
    EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
        .WillRepeatedly(::testing::ReturnRef(storageMock));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPos(pair(1, 0), ::testing::_, ::testing::_, ::testing::_))
        .WillOnce(::testing::Return(false))
        .WillOnce(::testing::Return(false));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPos(pair(0, 0), ::testing::_, ::testing::_, ::testing::_))
        .WillOnce(::testing::Return(false));
    EXPECT_CALL(*dynamic_cast<StorageMock *>(storageMock.get()), getSize())
        .WillOnce(::testing::Return(2));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNumberOfDeletedBytesAtDummyDeletions(TEST_UUID))
        .WillOnce(::testing::Return(0UL));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getLastValidPosition(TEST_UUID, storageMock, 2ULL))
        .WillOnce(::testing::Return(pair(1, 0)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNextVirtualPosition(TEST_UUID, storageMock, pair(0, 0)))
        .WillOnce(::testing::Return(pair(1, 0)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNextVirtualPosition(TEST_UUID, storageMock, pair(1, 0)))
        .WillOnce(::testing::Return(pair(2, 0)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getPreviousVirtualPosition(TEST_UUID, storageMock, pair(2, 0)))
        .WillOnce(::testing::Return(pair(1, 0)))
        .WillOnce(::testing::Return(pair(1, 0)))
        .WillOnce(::testing::Return(pair(0, 0)))
        .WillOnce(::testing::Return(pair(0, 0)))
        .WillOnce(::testing::Return(pair(0, 0)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getPreviousVirtualPosition(TEST_UUID, storageMock, pair(1, 0)))
        .WillOnce(::testing::Return(pair(0, 0)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), removeCharacter(::testing::_, pair(2, 0)))
        .Times(2);

    //when
    auto result = action->execute();

    //then
    auto properCursorPos = pair(0ULL, 0ULL);
    ASSERT_EQ(result.getResult(), properCursorPos);
}

TEST_F(RemoveLineActionTest, betweenTwoNewLines)
{
    //before
    configuration->forceTextMode(testFile);
    std::unique_ptr<RemoveLineAction> action = std::make_unique<RemoveLineAction>(RemoveLineActionInput(nCursesMainWindowDecoratorMock, printingOrchestratorMock));
    EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getCursorRealPositionMock(::testing::_))
        .WillOnce(::testing::Return(pair(2, 0)));
    EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
        .WillRepeatedly(::testing::ReturnRef(storageMock));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPos(pair(2, 0), ::testing::_, ::testing::_, ::testing::_))
        .WillOnce(::testing::Return(false))
        .WillOnce(::testing::Return(false))
        .WillOnce(::testing::Return(false));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPos(pair(3, 0), ::testing::_, ::testing::_, ::testing::_))
        .WillOnce(::testing::Return(true));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPos(pair(1, 0), ::testing::_, ::testing::_, ::testing::_))
        .WillOnce(::testing::Return(true))
        .WillOnce(::testing::Return(true));
    EXPECT_CALL(*dynamic_cast<StorageMock *>(storageMock.get()), getSize())
        .WillOnce(::testing::Return(5));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNumberOfDeletedBytesAtDummyDeletions(TEST_UUID))
        .WillOnce(::testing::Return(0UL));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getLastValidPosition(TEST_UUID, storageMock, 5ULL))
        .WillOnce(::testing::Return(pair(4, 0)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNextVirtualPosition(TEST_UUID, storageMock, pair(2, 0)))
        .WillOnce(::testing::Return(pair(3, 0)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNextVirtualPosition(TEST_UUID, storageMock, pair(3, 0)))
        .WillOnce(::testing::Return(pair(4, 0)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getPreviousVirtualPosition(TEST_UUID, storageMock, pair(2, 0)))
        .WillOnce(::testing::Return(pair(1, 0)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getPreviousVirtualPosition(TEST_UUID, storageMock, pair(4, 0)))
        .WillOnce(::testing::Return(pair(3, 0)))
        .WillOnce(::testing::Return(pair(3, 0)))
        .WillOnce(::testing::Return(pair(2, 0)))
        .WillOnce(::testing::Return(pair(2, 0)))
        .WillOnce(::testing::Return(pair(1, 0)))
        .WillOnce(::testing::Return(pair(1, 0)))
        .WillOnce(::testing::Return(pair(0, 0)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), removeCharacter(::testing::_, pair(4, 0)))
        .Times(2);

    //when
    auto result = action->execute();

    //then
    auto properCursorPos = pair(0ULL, 0ULL);
    ASSERT_EQ(result.getResult(), properCursorPos);
}
*/
} // namespace feather::action
