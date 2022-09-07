#include "mocks/BufferFillerMock.hpp"
#include "mocks/NCursesMainWindowDecoratorMock.hpp"
#include "mocks/PrintingOrchestratorMock.hpp"
#include "mocks/StorageMock.hpp"
#include "action/MoveToNextWordAction.hpp"
#include "helpers/TestBase.hpp"
#include "utils/BufferFillerInterface.hpp"
#include "utils/storage/InMemoryStorageFactory.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace ::testing::mocks;

namespace feather::action
{
class MoveToNextWordActionTest : public ::testing::Test, public feather::test::TestBase
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
        inMemoryStorageFactory = std::make_shared<feather::utils::storage::InMemoryStorageFactory>();
        bufferFillerMock = std::make_unique<BufferFillerMock>();
        printingOrchestratorMock = std::make_shared<PrintingOrchestratorMock>();
        nCursesMainWindowDecoratorMock = std::make_unique<NCursesMainWindowDecoratorMock>();
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getBufferFiller())
            .WillRepeatedly(::testing::ReturnRef(bufferFillerMock));
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getUUID(false))
            .WillRepeatedly(::testing::ReturnRef(TEST_UUID));
        storageMock = std::make_shared<StorageMock>();
    }
};
/*
TEST_F(MoveToNextWordActionTest, moveToNextWord)
{
    //before
    std::unique_ptr<MoveToNextWordAction> action = std::make_unique<MoveToNextWordAction>(MoveToNextWordActionInput(nCursesMainWindowDecoratorMock, printingOrchestratorMock));
    EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getCursorRealPositionMock(printingOrchestratorMock.get()))
        .WillOnce(::testing::Return(std::make_pair<size_t, size_t>(1300, 0)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPos(::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .WillOnce(::testing::Return(true))
        .WillOnce(::testing::Return(true))
        .WillOnce(::testing::Return(true))
        .WillRepeatedly(::testing::Return(false));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNextVirtualPosition(TEST_UUID, storageMock, std::make_pair<size_t, size_t>(1300, 0)))
        .WillOnce(::testing::Return(std::make_pair(1300, 1)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNextVirtualPosition(TEST_UUID, storageMock, std::make_pair<size_t, size_t>(1300, 1)))
        .WillOnce(::testing::Return(std::make_pair(1300, 2)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNextVirtualPosition(TEST_UUID, storageMock, std::make_pair<size_t, size_t>(1300, 2)))
        .WillOnce(::testing::Return(std::make_pair(1300, 3)));
    EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
        .WillRepeatedly(::testing::ReturnRef(storageMock));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getLastValidPosition(TEST_UUID, storageMock))
        .WillOnce(::testing::Return(std::make_pair<size_t, size_t>(1349, 0)));

    //when
    auto result = action->execute();

    //then
    auto properCursorPos = std::pair<size_t, size_t>(1300ULL, 3ULL);
    ASSERT_EQ(result.getResult(), properCursorPos);
}

TEST_F(MoveToNextWordActionTest, returnInitialCursorPosition)
{
    //before
    std::unique_ptr<MoveToNextWordAction> action = std::make_unique<MoveToNextWordAction>(MoveToNextWordActionInput(nCursesMainWindowDecoratorMock, printingOrchestratorMock));
    EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getCursorRealPositionMock(printingOrchestratorMock.get()))
        .WillOnce(::testing::Return(std::make_pair<size_t, size_t>(13448, 0)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), isCharAtPos(::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .WillRepeatedly(::testing::Return(true));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNextVirtualPosition(TEST_UUID, storageMock, std::make_pair<size_t, size_t>(13448, 0)))
        .WillOnce(::testing::Return(std::make_pair(13449, 0)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNextVirtualPosition(TEST_UUID, storageMock, std::make_pair<size_t, size_t>(13449, 0)))
        .WillOnce(::testing::Return(std::make_pair(13449, 0)));
    EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
        .WillRepeatedly(::testing::ReturnRef(storageMock));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getLastValidPosition(TEST_UUID, storageMock))
        .WillOnce(::testing::Return(std::make_pair<size_t, size_t>(1349, 0)));

    //when
    auto result = action->execute();

    //then
    auto properCursorPos = std::pair<size_t, size_t>(13448ULL, 0ULL);
    ASSERT_EQ(result.getResult(), properCursorPos);
}
*/
} // namespace feather::action
