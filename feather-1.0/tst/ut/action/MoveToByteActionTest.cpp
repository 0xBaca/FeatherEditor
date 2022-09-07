#include "mocks/BufferFillerMock.hpp"
#include "mocks/NCursesMainWindowDecoratorMock.hpp"
#include "mocks/PrintingOrchestratorMock.hpp"
#include "mocks/StorageMock.hpp"
#include "action/MoveToByteAction.hpp"
#include "helpers/TestBase.hpp"
#include "utils/BufferFillerInterface.hpp"
#include "utils/storage/InMemoryStorageFactory.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace ::testing::mocks;

namespace feather::action
{
class MoveToByteActionTest : public ::testing::Test, public feather::test::TestBase
{
protected:
    std::shared_ptr<feather::utils::storage::AbstractStorageFactory> inMemoryStorageFactory;
    std::unique_ptr<BufferFillerInterface> bufferFillerMock;
    std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestratorMock;
    std::unique_ptr<MainWindowInterface> nCursesMainWindowDecoratorMock;
    std::shared_ptr<feather::utils::storage::AbstractStorage> storageMock;

public:
    void SetUp() override
    {
        storageMock = std::make_shared<StorageMock>();
        inMemoryStorageFactory = std::make_shared<feather::utils::storage::InMemoryStorageFactory>();
        bufferFillerMock = std::make_unique<BufferFillerMock>();
        printingOrchestratorMock = std::make_shared<PrintingOrchestratorMock>();
        nCursesMainWindowDecoratorMock = std::make_unique<NCursesMainWindowDecoratorMock>();
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getBufferFiller())
            .WillRepeatedly(::testing::ReturnRef(bufferFillerMock));
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getUUID(false))
            .WillRepeatedly(::testing::ReturnRef(TEST_UUID));
    }
};
/*
TEST_F(MoveToByteActionTest, properlyMoveToByteOnEmptyFileWithoutNewLine_textMode)
{
    //before
    configuration->forceTextMode(testFile);
    utils::windows::FramePositions framePositions = utils::windows::FramePositions{0, 0, 0, 0};
    std::unique_ptr<MoveToByteAction> action = std::make_unique<MoveToByteAction>(MoveToByteActionInput(10, nCursesMainWindowDecoratorMock, printingOrchestratorMock));
    //EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), convertRealPositionToByteOffset(pair(0, 0), TEST_UUID))
    //    .WillOnce(::testing::Return(0));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getLastValidPosition(TEST_UUID, storageMock))
        .WillOnce(::testing::Return(pair(0, 0)));
    EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
        .WillRepeatedly(::testing::ReturnRef(storageMock));
    EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getFramePositions())
        .WillRepeatedly(::testing::ReturnRef(framePositions));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNumberOfDeletedBytesAtDummyDeletions(TEST_UUID))
        .WillOnce(::testing::Return(0));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPos(std::make_pair<size_t, size_t>(0, 0), ::testing::_, TEST_UUID, storageMock))
        .WillOnce(::testing::Return(false));

    //when
    auto result = action->execute();

    //then
    ASSERT_THAT(result.getResult(), ::testing::Eq(pair(0, 0)));
}

TEST_F(MoveToByteActionTest, properlyMoveToByteOnEmptyFileWithNewLine_textMode)
{
    //before
    configuration->forceTextMode(testFile);
    utils::windows::FramePositions framePositions = utils::windows::FramePositions{0, 0, 0, 1};
    std::unique_ptr<MoveToByteAction> action = std::make_unique<MoveToByteAction>(MoveToByteActionInput(1, nCursesMainWindowDecoratorMock, printingOrchestratorMock));
    //EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), convertRealPositionToByteOffset(pair(0, 1), TEST_UUID))
    //    .WillOnce(::testing::Return(1));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getLastValidPosition(TEST_UUID, storageMock))
        .WillOnce(::testing::Return(pair(0, 1)));
    EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
        .WillRepeatedly(::testing::ReturnRef(storageMock));
    EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getFramePositions())
        .WillRepeatedly(::testing::ReturnRef(framePositions));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNumberOfDeletedBytesAtDummyDeletions(TEST_UUID))
        .WillOnce(::testing::Return(0));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPos(std::make_pair<size_t, size_t>(0, 1), ::testing::_, TEST_UUID, storageMock))
        .WillOnce(::testing::Return(true));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getPreviousVirtualPosition(TEST_UUID, storageMock, std::make_pair<size_t, size_t>(0, 1)))
        .WillOnce(::testing::Return(pair(0, 0)));

    //when
    auto result = action->execute();

    //then
    ASSERT_THAT(result.getResult(), ::testing::Eq(pair(0, 0)));
}

TEST_F(MoveToByteActionTest, properlyMoveToByteOnEmptyFileWithNewLine_binaryMode)
{
    //before
    configuration->forceBinaryMode(testFile);
    utils::windows::FramePositions framePositions = utils::windows::FramePositions{0, 0, 0, 1};
    std::unique_ptr<MoveToByteAction> action = std::make_unique<MoveToByteAction>(MoveToByteActionInput(1, nCursesMainWindowDecoratorMock, printingOrchestratorMock));
    //EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), convertRealPositionToByteOffset(pair(0, 1), TEST_UUID))
    //    .WillOnce(::testing::Return(1));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getLastValidPosition(TEST_UUID, storageMock))
        .WillOnce(::testing::Return(pair(0, 1)));
    EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
        .WillRepeatedly(::testing::ReturnRef(storageMock));
    EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getFramePositions())
        .WillRepeatedly(::testing::ReturnRef(framePositions));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNumberOfDeletedBytesAtDummyDeletions(TEST_UUID))
        .WillOnce(::testing::Return(0));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPos(std::make_pair<size_t, size_t>(0, 1), ::testing::_, TEST_UUID, storageMock))
        .WillOnce(::testing::Return(true));

    //when
    auto result = action->execute();

    //then
    ASSERT_THAT(result.getResult(), ::testing::Eq(pair(0, 1)));
}

TEST_F(MoveToByteActionTest, properlyMoveToNewChangesByteWhenByteIsNewLine_textMode)
{
    //before
    configuration->forceTextMode(testFile);
    utils::windows::FramePositions framePositions = utils::windows::FramePositions{0, 0, 0, 4};
    std::unique_ptr<MoveToByteAction> action = std::make_unique<MoveToByteAction>(MoveToByteActionInput(8UL, nCursesMainWindowDecoratorMock, printingOrchestratorMock));
    //EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), convertRealPositionToByteOffset(pair(0UL, 4UL), TEST_UUID))
    //    .WillOnce(::testing::Return(10));
    EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
        .WillRepeatedly(::testing::ReturnRef(storageMock));
    EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getFramePositions())
        .WillRepeatedly(::testing::ReturnRef(framePositions));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getLastValidPosition(TEST_UUID, storageMock))
        .WillOnce(::testing::Return(std::make_pair<size_t, size_t>(0, 4)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getPreviousVirtualPosition(TEST_UUID, storageMock, pair(0, 8UL)))
        .WillOnce(::testing::Return(std::make_pair<size_t, size_t>(0, 4)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNumberOfChangesBytesAtPos(0, TEST_UUID))
        .WillOnce(::testing::Return(11));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNumberOfDeletedBytesAtDummyDeletions(TEST_UUID))
        .WillOnce(::testing::Return(0));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getDiffToNextChange(0ULL, TEST_UUID))
        .WillOnce(::testing::Return(0));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosWithChanges(0ULL, TEST_UUID))
        .WillOnce(::testing::Return(true));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPos(std::make_pair<size_t, size_t>(0, 8), ::testing::_, TEST_UUID, storageMock))
        .WillOnce(::testing::Return(true));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPos(std::make_pair<size_t, size_t>(0, 4), ::testing::_, TEST_UUID, storageMock))
        .WillOnce(::testing::Return(true))
        .WillOnce(::testing::Return(true));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNewChanges(TEST_UUID, 0UL, 0UL, configuration->getMemoryBytesRelaxed()))
        .WillOnce(::testing::Return(std::vector<char>{(char)0xF0, (char)0x9F, (char)0x98, (char)0x9A, (char)0xF0, (char)0x9F, (char)0x98, (char)0x9A, '\n', 'c', '\n'}));

    //when
    auto result = action->execute();

    //then
    ASSERT_THAT(result.getResult(), ::testing::Eq(pair(0, 8UL)));
}

TEST_F(MoveToByteActionTest, properlyMoveToNewChangesByteWhenByteIsNewLine_binaryMode)
{
    //before
    configuration->forceBinaryMode(testFile);
    utils::windows::FramePositions framePositions = utils::windows::FramePositions{0, 0, 0, 4};
    std::unique_ptr<MoveToByteAction> action = std::make_unique<MoveToByteAction>(MoveToByteActionInput(2UL, nCursesMainWindowDecoratorMock, printingOrchestratorMock));
    //EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), convertRealPositionToByteOffset(pair(0UL, 4UL), TEST_UUID))
    //    .WillOnce(::testing::Return(10));
    EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
        .WillRepeatedly(::testing::ReturnRef(storageMock));
    EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getFramePositions())
        .WillRepeatedly(::testing::ReturnRef(framePositions));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getLastValidPosition(TEST_UUID, storageMock))
        .WillOnce(::testing::Return(std::make_pair<size_t, size_t>(0, 4)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNumberOfChangesBytesAtPos(0, TEST_UUID))
        .WillOnce(::testing::Return(11));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNumberOfDeletedBytesAtDummyDeletions(TEST_UUID))
        .WillOnce(::testing::Return(0));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getDiffToNextChange(0ULL, TEST_UUID))
        .WillOnce(::testing::Return(0));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosWithChanges(0ULL, TEST_UUID))
        .WillOnce(::testing::Return(true));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPos(std::make_pair<size_t, size_t>(0, 4), ::testing::_, TEST_UUID, storageMock))
        .WillOnce(::testing::Return(true));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNewChanges(TEST_UUID, 0UL, 0UL, configuration->getMemoryBytesRelaxed()))
        .WillOnce(::testing::Return(std::vector<char>{(char)0x9A, (char)0x9, '\n', 'c', '\n'}));

    //when
    auto result = action->execute();

    //then
    ASSERT_THAT(result.getResult(), ::testing::Eq(pair(0, 2)));
}

TEST_F(MoveToByteActionTest, properlyMoveToNewChangesByteWhenInTheMiddleOfCharacter)
{
    //before
    configuration->forceTextMode(testFile);
    utils::windows::FramePositions framePositions = utils::windows::FramePositions{0, 0, 0, 4};
    std::unique_ptr<MoveToByteAction> action = std::make_unique<MoveToByteAction>(MoveToByteActionInput(5UL, nCursesMainWindowDecoratorMock, printingOrchestratorMock));
    //EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), convertRealPositionToByteOffset(pair(0UL, 4UL), TEST_UUID))
    //    .WillOnce(::testing::Return(10));
    EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
        .WillRepeatedly(::testing::ReturnRef(storageMock));
    EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getFramePositions())
        .WillRepeatedly(::testing::ReturnRef(framePositions));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getLastValidPosition(TEST_UUID, storageMock))
        .WillOnce(::testing::Return(std::make_pair<size_t, size_t>(0, 4)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNumberOfChangesBytesAtPos(0, TEST_UUID))
        .WillOnce(::testing::Return(11));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNumberOfDeletedBytesAtDummyDeletions(TEST_UUID))
        .WillOnce(::testing::Return(0));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getDiffToNextChange(0ULL, TEST_UUID))
        .WillOnce(::testing::Return(0));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPosWithChanges(0ULL, TEST_UUID))
        .WillOnce(::testing::Return(true));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPos(std::make_pair<size_t, size_t>(0, 4), ::testing::_, TEST_UUID, storageMock))
        .WillOnce(::testing::Return(false))
        .WillOnce(::testing::Return(false));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNewChanges(TEST_UUID, 0UL, 0UL, configuration->getMemoryBytesRelaxed()))
        .WillOnce(::testing::Return(std::vector<char>{(char)0xF0, (char)0x9F, (char)0x98, (char)0x9A, (char)0xF0, (char)0x9F, (char)0x98, (char)0x9A, '\n', 'c', '\n'}));

    //when
    auto result = action->execute();

    //then
    ASSERT_THAT(result.getResult(), ::testing::Eq(pair(0, 4UL)));
}
*/
} // namespace feather::action
