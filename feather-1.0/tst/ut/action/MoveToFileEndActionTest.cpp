#include "mocks/BufferFillerMock.hpp"
#include "mocks/NCursesMainWindowDecoratorMock.hpp"
#include "mocks/PrintingOrchestratorMock.hpp"
#include "mocks/StorageMock.hpp"
#include "action/MoveToFileEndAction.hpp"
#include "helpers/TestBase.hpp"
#include "utils/BufferFillerInterface.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace ::testing::mocks;

namespace feather::action
{
    class MoveToFileEndActionTest : public ::testing::Test, public feather::test::TestBase
    {
    protected:
        std::unique_ptr<BufferFillerInterface> bufferFillerMock;
        std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestratorMock;
        std::shared_ptr<MainWindowInterface> nCursesMainWindowDecoratorMock;
        std::shared_ptr<feather::utils::storage::AbstractStorage> storageMock;

    public:
        void SetUp() override
        {
            storageMock = std::make_shared<StorageMock>();
            bufferFillerMock = std::make_unique<BufferFillerMock>();
            printingOrchestratorMock = std::make_shared<PrintingOrchestratorMock>();
            nCursesMainWindowDecoratorMock = std::make_shared<NCursesMainWindowDecoratorMock>();
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getStorage())
                .WillRepeatedly(::testing::ReturnRef(storageMock));
            EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getUUID(false))
                .WillRepeatedly(::testing::ReturnRef(TEST_UUID));
        }
    };

    TEST_F(MoveToFileEndActionTest, properlyMoveToFileEndOnEmptyFile)
    {
        // before
        configuration->forceTextMode(testFile);
        utils::windows::FramePositions framePositions = utils::windows::FramePositions{pair{0, 0}, pair{0, 0}};
        std::unique_ptr<MoveToFileEndAction> action = std::make_unique<MoveToFileEndAction>(MoveToFileEndActionInput(nCursesMainWindowDecoratorMock, printingOrchestratorMock));
        EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
            .WillRepeatedly(::testing::ReturnRef(storageMock));
        EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getFramePositions())
            .WillRepeatedly(::testing::ReturnRef(framePositions));
        EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getLastValidPosition(TEST_UUID, storageMock))
            .WillOnce(::testing::Return(pair(0, 0)));
        EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNextVirtualPosition(TEST_UUID, storageMock, pair(0, 0), false))
            .WillOnce(::testing::Return(pair(0, 0)));
        EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getPreviousVirtualPosition(TEST_UUID, storageMock, pair(0, 0)))
            .WillOnce(::testing::Return(pair(0, 0)));
        EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getContinousCharacters(pair(0, 0), 4UL, storageMock, TEST_UUID, false, std::map<pair, pair>{}))
            .WillOnce(::testing::Return(std::make_pair(std::vector<char>(), pair(0, 0))));

        // when
        auto result = action->execute();

        // then
        ASSERT_THAT(result.getResult(), ::testing::Eq(pair(0, 0)));
    }

    TEST_F(MoveToFileEndActionTest, properlyMoveToFileEndWithNewLine_textMode)
    {
        // before
        configuration->forceTextMode(testFile);
        utils::windows::FramePositions framePositions = utils::windows::FramePositions{pair{0, 0}, pair{0, 16}};
        std::unique_ptr<MoveToFileEndAction> action = std::make_unique<MoveToFileEndAction>(MoveToFileEndActionInput(nCursesMainWindowDecoratorMock, printingOrchestratorMock));
        EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getLastValidPosition(TEST_UUID, storageMock))
            .WillOnce(::testing::Return(std::make_pair<size_t, size_t>(0, 16)));
        EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
            .WillRepeatedly(::testing::ReturnRef(storageMock));
        EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getFramePositions())
            .WillRepeatedly(::testing::ReturnRef(framePositions));
        EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNextVirtualPosition(TEST_UUID, storageMock, pair(0, 16UL), false))
            .WillOnce(::testing::Return(pair(0, 17UL)));
        EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getPreviousVirtualPosition(TEST_UUID, storageMock, pair(0, 16UL)))
            .WillOnce(::testing::Return(pair(0, 15UL)));
        EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getContinousCharacters(pair(0, 16UL), MAX_UTF8_CHAR_BYTES, storageMock, TEST_UUID, false, std::map<pair, pair>{}))
            .WillOnce(::testing::Return(std::make_pair(std::vector<char>{'\n'}, pair(0, 17UL))));

        // when
        auto result = action->execute();

        // then
        ASSERT_THAT(result.getResult(), ::testing::Eq(pair(0, 15)));
    }

    TEST_F(MoveToFileEndActionTest, properlyMoveToFileEndWithNewLineTwice_textMode)
    {
        // before
        configuration->forceTextMode(testFile);
        utils::windows::FramePositions framePositions = utils::windows::FramePositions{pair{0, 0}, pair{0, 16}};
        std::unique_ptr<MoveToFileEndAction> action = std::make_unique<MoveToFileEndAction>(MoveToFileEndActionInput(nCursesMainWindowDecoratorMock, printingOrchestratorMock));
        EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getLastValidPosition(TEST_UUID, storageMock))
            .WillOnce(::testing::Return(std::make_pair<size_t, size_t>(0, 16)));
        EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
            .WillRepeatedly(::testing::ReturnRef(storageMock));
        EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getFramePositions())
            .WillRepeatedly(::testing::ReturnRef(framePositions));
        EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNextVirtualPosition(TEST_UUID, storageMock, pair(0, 16UL), false))
            .WillOnce(::testing::Return(pair(0, 17UL)));
        EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getPreviousVirtualPosition(TEST_UUID, storageMock, pair(0, 16UL)))
            .WillOnce(::testing::Return(pair(0, 15UL)));
        EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getContinousCharacters(pair(0, 16UL), MAX_UTF8_CHAR_BYTES, storageMock, TEST_UUID, false, std::map<pair, pair>{}))
            .WillOnce(::testing::Return(std::make_pair(std::vector<char>{'\n'}, pair(0, 17UL))));

        // when
        auto result = action->execute();

        // then
        ASSERT_THAT(result.getResult(), ::testing::Eq(pair(0, 15)));
    }

    TEST_F(MoveToFileEndActionTest, properlyMoveToFileEndWithNewLine_binaryMode)
    {
        // before
        configuration->forceHexMode(testFile);
        utils::windows::FramePositions framePositions = utils::windows::FramePositions{pair{0, 0}, pair{0, 16}};
        std::unique_ptr<MoveToFileEndAction> action = std::make_unique<MoveToFileEndAction>(MoveToFileEndActionInput(nCursesMainWindowDecoratorMock, printingOrchestratorMock));
        EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getLastValidPosition(TEST_UUID, storageMock))
            .WillOnce(::testing::Return(std::make_pair<size_t, size_t>(0, 16)));
        EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
            .WillRepeatedly(::testing::ReturnRef(storageMock));
        EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getFramePositions())
            .WillRepeatedly(::testing::ReturnRef(framePositions));
        EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getNextVirtualPosition(TEST_UUID, storageMock, pair(0, 16UL), false))
            .WillOnce(::testing::Return(pair(0, 17UL)));
        EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getPreviousVirtualPosition(TEST_UUID, storageMock, pair(0, 16UL)))
            .WillOnce(::testing::Return(pair(0, 15UL)));
        EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getContinousCharacters(pair(0, 16UL), MAX_UTF8_CHAR_BYTES, storageMock, TEST_UUID, false, std::map<pair, pair>{}))
            .WillOnce(::testing::Return(std::make_pair(std::vector<char>{'\n'}, pair(0, 17UL))));

        // when
        auto result = action->execute();

        // then
        ASSERT_THAT(result.getResult(), ::testing::Eq(pair(0, 16)));
    }

} // namespace feather::action
