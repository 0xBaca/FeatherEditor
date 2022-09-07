#include "mocks/BufferFillerMock.hpp"
#include "mocks/NCursesMainWindowDecoratorMock.hpp"
#include "mocks/NCursesWindowMock.hpp"
#include "mocks/NCursesWrapperMock.hpp"
#include "mocks/PrintingOrchestratorMock.hpp"
#include "mocks/StorageMock.hpp"
#include "mocks/WindowsManagerMock.hpp"
#include "action/RemoveCharacterUnderCursorAction.hpp"
#include "utils/BufferFillerInterface.hpp"
#include "utils/storage/FileStorageFactory.hpp"
#include "utils/storage/InMemoryStorageFactory.hpp"
#include "windows/WindowsManager.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace ::testing::mocks;

extern std::unique_ptr<const feather::config::Configuration> configuration;

namespace feather::action
{
class RemoveCharacterUnderCursorActionTest : public ::testing::Test
{
protected:
    std::string testFile;
    static constexpr const char *testUUID = "220097a9-67fc-4110-b92e-c60227359c68";
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
        testFile = "testFile";
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
        storageMock = std::make_shared<StorageMock>();
    }
};
/*
TEST_F(RemoveCharacterUnderCursorActionTest, dontRemoveWhenAtNewLine_TextMode)
{
    //before
    configuration->forceTextMode(testFile);
    std::unique_ptr<RemoveCharacterUnderCursorAction> action = std::make_unique<RemoveCharacterUnderCursorAction>(RemoveCharacterUnderCursorActionInput(nCursesMainWindowDecoratorMock, printingOrchestratorMock));
    EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getCursorRealPositionMock(printingOrchestratorMock.get()))
        .WillOnce(::testing::Return(std::make_pair<size_t, size_t>(0, 0)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPos(::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .WillOnce(::testing::Return(true));
    EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
        .WillRepeatedly(::testing::ReturnRef(storageMock));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getLastValidPosition(TEST_UUID, storageMock))
        .WillOnce(::testing::Return(pair(1, 0)));

    //when
    auto result = action->execute();

    //then
    ASSERT_EQ(result.getResult(), pair(0, 0));
}

TEST_F(RemoveCharacterUnderCursorActionTest, removeWhenAtNewLine_BinaryMode)
{
    //before
    configuration->forceBinaryMode(testFile);
    std::unique_ptr<RemoveCharacterUnderCursorAction> action = std::make_unique<RemoveCharacterUnderCursorAction>(RemoveCharacterUnderCursorActionInput(nCursesMainWindowDecoratorMock, printingOrchestratorMock));
    EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getCursorRealPositionMock(printingOrchestratorMock.get()))
        .WillOnce(::testing::Return(pair(0, 0)));
    EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
        .WillRepeatedly(::testing::ReturnRef(storageMock));
    EXPECT_CALL(*dynamic_cast<StorageMock *>(storageMock.get()), getSize())
        .WillOnce(::testing::Return(2));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getLastValidPosition(TEST_UUID, storageMock))
        .WillOnce(::testing::Return(pair(1, 0)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNextVirtualPosition(TEST_UUID, storageMock, std::make_pair<size_t, size_t>(0, 0)))
        .WillOnce(::testing::Return(std::make_pair(1, 0)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), removeCharacter(::testing::_, storageMock, std::optional<pair>(pair(1, 0))));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNumberOfChangesAtPos(0UL, TEST_UUID))
        .WillOnce(::testing::Return(0UL))
        .WillOnce(::testing::Return(0UL));

    //when
    auto result = action->execute();

    //then
    ASSERT_EQ(result.getResult(), pair(1, 0));
}

TEST_F(RemoveCharacterUnderCursorActionTest, removeCharacterWhenAtFileBeginingAndNextCharacterIsNotNewLine_TextMode)
{
    //before
    configuration->forceTextMode(testFile);
    std::unique_ptr<RemoveCharacterUnderCursorAction> action = std::make_unique<RemoveCharacterUnderCursorAction>(RemoveCharacterUnderCursorActionInput(nCursesMainWindowDecoratorMock, printingOrchestratorMock));
    EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getCursorRealPositionMock(printingOrchestratorMock.get()))
        .WillOnce(::testing::Return(std::make_pair<size_t, size_t>(0, 0)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPos(::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .WillOnce(::testing::Return(false))
        .WillOnce(::testing::Return(false));
    EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
        .WillRepeatedly(::testing::ReturnRef(storageMock));
    EXPECT_CALL(*dynamic_cast<StorageMock *>(storageMock.get()), getSize())
        .WillOnce(::testing::Return(2));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getLastValidPosition(TEST_UUID, storageMock))
        .WillOnce(::testing::Return(pair(1, 0)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), removeCharacter(::testing::_, storageMock, std::optional<pair>(pair(1, 0))));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNextVirtualPosition(TEST_UUID, storageMock, pair(0, 0)))
        .WillOnce(::testing::Return(std::make_pair(1, 0)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNumberOfChangesAtPos(0UL, TEST_UUID))
        .WillOnce(::testing::Return(0UL))
        .WillOnce(::testing::Return(0UL));

    //when
    auto result = action->execute();

    //then
    ASSERT_EQ(result.getResult(), pair(1, 0));
}

TEST_F(RemoveCharacterUnderCursorActionTest, removeCharacterWhenAtFileBeginingAndNextCharacterIsNewLine_TextMode)
{
    //before
    configuration->forceTextMode(testFile);
    std::unique_ptr<RemoveCharacterUnderCursorAction> action = std::make_unique<RemoveCharacterUnderCursorAction>(RemoveCharacterUnderCursorActionInput(nCursesMainWindowDecoratorMock, printingOrchestratorMock));
    EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getCursorRealPositionMock(printingOrchestratorMock.get()))
        .WillOnce(::testing::Return(std::make_pair<size_t, size_t>(0, 0)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPos(::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .WillOnce(::testing::Return(false))
        .WillOnce(::testing::Return(true));
    EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
        .WillRepeatedly(::testing::ReturnRef(storageMock));
    EXPECT_CALL(*dynamic_cast<StorageMock *>(storageMock.get()), getSize())
        .WillOnce(::testing::Return(2));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getLastValidPosition(TEST_UUID, storageMock))
        .WillOnce(::testing::Return(pair(1, 0)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtFirstPosition(pair(1, 0), ::testing::_))
        .WillOnce(::testing::Return(true));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), removeCharacter(::testing::_, storageMock, std::optional<pair>(pair(1, 0))));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNextVirtualPosition(TEST_UUID, storageMock, pair(0, 0)))
        .WillOnce(::testing::Return(std::make_pair(1, 0)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNumberOfChangesAtPos(0UL, TEST_UUID))
        .WillOnce(::testing::Return(0UL))
        .WillOnce(::testing::Return(0UL));

    //when
    auto result = action->execute();

    //then
    ASSERT_EQ(result.getResult(), pair(1, 0));
}

TEST_F(RemoveCharacterUnderCursorActionTest, removeCharacterWhenAtFileEndWithNewLine_textMode)
{
    //before
    configuration->forceTextMode(testFile);
    std::unique_ptr<RemoveCharacterUnderCursorAction> action = std::make_unique<RemoveCharacterUnderCursorAction>(RemoveCharacterUnderCursorActionInput(nCursesMainWindowDecoratorMock, printingOrchestratorMock));
    EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getCursorRealPositionMock(printingOrchestratorMock.get()))
        .WillOnce(::testing::Return(std::make_pair<size_t, size_t>(1, 0)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPos(::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .WillOnce(::testing::Return(false))
        .WillOnce(::testing::Return(true))
        .WillOnce(::testing::Return(true));
    EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
        .WillRepeatedly(::testing::ReturnRef(storageMock));
    EXPECT_CALL(*dynamic_cast<StorageMock *>(storageMock.get()), getSize())
        .WillOnce(::testing::Return(3));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getLastValidPosition(TEST_UUID, storageMock))
        .WillOnce(::testing::Return(pair(2, 0)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtFirstPosition(pair(2, 0), ::testing::_))
        .WillOnce(::testing::Return(false));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), removeCharacter(::testing::_, storageMock, std::optional<pair>(pair(2, 0))));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getPreviousVirtualPosition(TEST_UUID, storageMock, std::make_pair<size_t, size_t>(1, 0)))
        .WillOnce(::testing::Return(std::make_pair(0, 0)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNextVirtualPosition(TEST_UUID, storageMock, pair(1, 0)))
        .WillOnce(::testing::Return(std::make_pair(2, 0)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNumberOfChangesAtPos(1UL, TEST_UUID))
        .WillOnce(::testing::Return(0UL))
        .WillOnce(::testing::Return(0UL));

    //when
    auto result = action->execute();

    //then
    ASSERT_EQ(result.getResult(), pair(2, 0));
}

TEST_F(RemoveCharacterUnderCursorActionTest, removeCharacterWhenAtFileEndWithNewLine_binaryMode)
{
    //before
    configuration->forceBinaryMode(testFile);
    std::unique_ptr<RemoveCharacterUnderCursorAction> action = std::make_unique<RemoveCharacterUnderCursorAction>(RemoveCharacterUnderCursorActionInput(nCursesMainWindowDecoratorMock, printingOrchestratorMock));
    EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getCursorRealPositionMock(printingOrchestratorMock.get()))
        .WillOnce(::testing::Return(std::make_pair<size_t, size_t>(1, 0)));
    EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
        .WillRepeatedly(::testing::ReturnRef(storageMock));
    EXPECT_CALL(*dynamic_cast<StorageMock *>(storageMock.get()), getSize())
        .WillOnce(::testing::Return(3));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getLastValidPosition(TEST_UUID, storageMock))
        .WillOnce(::testing::Return(pair(2, 0)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), removeCharacter(::testing::_, storageMock, std::optional<pair>(pair(2, 0))));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNextVirtualPosition(TEST_UUID, storageMock, pair(1, 0)))
        .WillOnce(::testing::Return(std::make_pair(2, 0)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNumberOfChangesAtPos(1UL, TEST_UUID))
        .WillOnce(::testing::Return(0UL))
        .WillOnce(::testing::Return(0UL));

    //when
    auto result = action->execute();

    //then
    ASSERT_EQ(result.getResult(), pair(2, 0));
}

TEST_F(RemoveCharacterUnderCursorActionTest, removeCharacterWhenAtFileEndWithoutNewLine_textMode)
{
    //before
    configuration->forceTextMode(testFile);
    std::unique_ptr<RemoveCharacterUnderCursorAction> action = std::make_unique<RemoveCharacterUnderCursorAction>(RemoveCharacterUnderCursorActionInput(nCursesMainWindowDecoratorMock, printingOrchestratorMock));
    EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getCursorRealPositionMock(printingOrchestratorMock.get()))
        .WillOnce(::testing::Return(std::make_pair<size_t, size_t>(2, 0)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPos(::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .WillOnce(::testing::Return(false));
    EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
        .WillRepeatedly(::testing::ReturnRef(storageMock));
    EXPECT_CALL(*dynamic_cast<StorageMock *>(storageMock.get()), getSize())
        .WillOnce(::testing::Return(3));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getLastValidPosition(TEST_UUID, storageMock))
        .WillOnce(::testing::Return(pair(2, 0)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtFirstPosition(pair(3, 0), ::testing::_))
        .WillOnce(::testing::Return(false));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), removeCharacter(::testing::_, storageMock, std::optional<pair>(pair(3, 0))));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getPreviousVirtualPosition(TEST_UUID, storageMock, pair(2, 0)))
        .WillOnce(::testing::Return(std::make_pair(1, 0)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNextVirtualPosition(TEST_UUID, storageMock, pair(2, 0)))
        .WillOnce(::testing::Return(pair(3, 0)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNumberOfChangesAtPos(2UL, TEST_UUID))
        .WillOnce(::testing::Return(0UL))
        .WillOnce(::testing::Return(0UL));

    //when
    auto result = action->execute();

    //then
    ASSERT_EQ(result.getResult(), pair(1, 0));
}

TEST_F(RemoveCharacterUnderCursorActionTest, removeCharacterWhenAtPosWithNewChanges_textMode)
{
    //before
    configuration->forceTextMode(testFile);
    std::unique_ptr<RemoveCharacterUnderCursorAction> action = std::make_unique<RemoveCharacterUnderCursorAction>(RemoveCharacterUnderCursorActionInput(nCursesMainWindowDecoratorMock, printingOrchestratorMock));
    EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getCursorRealPositionMock(printingOrchestratorMock.get()))
        .WillOnce(::testing::Return(std::make_pair<size_t, size_t>(2, 2)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPos(::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .WillOnce(::testing::Return(false))
        .WillOnce(::testing::Return(false));
    EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
        .WillRepeatedly(::testing::ReturnRef(storageMock));
    EXPECT_CALL(*dynamic_cast<StorageMock *>(storageMock.get()), getSize())
        .WillOnce(::testing::Return(5));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getLastValidPosition(TEST_UUID, storageMock))
        .WillOnce(::testing::Return(pair(2, 0)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), removeCharacter(::testing::_, storageMock, std::optional<pair>(pair(3, 2))));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNextVirtualPosition(TEST_UUID, storageMock, pair(2, 2)))
        .WillOnce(::testing::Return(pair(3, 0)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNumberOfChangesAtPos(2UL, TEST_UUID))
        .WillOnce(::testing::Return(2UL))
        .WillOnce(::testing::Return(2UL));

    //when
    auto result = action->execute();

    //then
    ASSERT_EQ(result.getResult(), pair(3, 2));
}
*/
} // namespace feather::action
