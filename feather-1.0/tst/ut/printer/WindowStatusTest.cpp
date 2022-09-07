#include "helpers/FileStorageHelper.hpp"
#include "mocks/BufferFillerMock.hpp"
#include "mocks/NCursesMainWindowDecoratorMock.hpp"
#include "mocks/NCursesProgressWindowDecoratorMock.hpp"
#include "mocks/NCursesWindowMock.hpp"
#include "mocks/NCursesWrapperMock.hpp"
#include "mocks/PrintingOrchestratorMock.hpp"
#include "mocks/StorageMock.hpp"
#include "printer/WindowStatus.hpp"
#include "printer/PrintingOrchestrator.hpp"
#include "utils/datatypes/Uuid.hpp"
#include "helpers/TestBase.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace ::testing::mocks;
using namespace feather::printer;

class WindowStatusTest : public ::testing::Test, public ::feather::test::TestBase
{
protected:
    std::unique_ptr<BufferFillerInterface> bufferFillerMock;
    std::shared_ptr<MainWindowInterface> nCursesMainWindowDecoratorMock;
    std::shared_ptr<feather::printer::PrintingOrchestratorInterface> printingOrchestratorMock;
    std::shared_ptr<feather::utils::storage::AbstractStorage> storageMock;
    std::shared_ptr<Utf8Util> utf8Util;
    std::unique_ptr<feather::printer::WindowStatus> windowStatus;
    std::unique_ptr<SubWindowInterface> progressWindow;

public:
    void SetUp() override
    {
        storageMock = std::make_shared<StorageMock>();
        bufferFillerMock = std::make_unique<BufferFillerMock>();
        printingOrchestratorMock = std::make_shared<PrintingOrchestratorMock>();
        progressWindow = std::make_unique<NCursesProgressWindowDecoratorMock>();
        nCursesMainWindowDecoratorMock = std::make_shared<NCursesMainWindowDecoratorMock>();
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getUUID(false))
            .WillRepeatedly(::testing::ReturnRef(TEST_UUID));
        EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getBufferFiller())
            .WillRepeatedly(::testing::ReturnRef(bufferFillerMock));
    }

    void TearDown() override
    {
    }
};

TEST_F(WindowStatusTest, getCorrectPrintProgress)
{
    // before
    feather::utils::windows::FramePositions framePositions{feather::pair{0, 0}, feather::pair{1, 0}};
    EXPECT_CALL(*dynamic_cast<NCursesProgressWindowDecoratorMock *>(progressWindow.get()), getParentWindowHandler())
        .WillOnce(::testing::Return(nCursesMainWindowDecoratorMock));
    EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getFramePositions())
        .WillRepeatedly(::testing::ReturnRef(framePositions));
    EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getStorage())
        .WillRepeatedly(::testing::ReturnRef(storageMock));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getTotalBytesOfDeletions(TEST_UUID))
        .WillRepeatedly(::testing::Return(10UL));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getTotalBytesOfNewChanges(::testing::_))
        .WillRepeatedly(::testing::Return(0));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), convertVirtualPositionToByteOffset(std::pair<size_t, size_t>(0U, 0UL), TEST_UUID))
        .WillOnce(::testing::Return(0U));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), convertVirtualPositionToByteOffset(std::pair<size_t, size_t>(1UL, 0UL), TEST_UUID))
        .WillOnce(::testing::Return(1U))
        .WillOnce(::testing::Return(1U));
    EXPECT_CALL(*dynamic_cast<StorageMock *>(storageMock.get()), getSize())
        .WillOnce(::testing::Return(1));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getLastValidPosition(TEST_UUID, storageMock))
        .WillOnce(::testing::Return(std::pair<size_t, size_t>(1, 0)));

    // when
    std::u32string progress = windowStatus->getPrintedProgress(progressWindow.get(), printingOrchestratorMock, std::nullopt);

    // then
    FileStorageHelper::compareLine(progress.begin(), progress.end(), utf8Util, std::u32string(U"100").begin());
}

TEST_F(WindowStatusTest, getCorrectPrintProgressWhenFileEmptyButWithNewChanges)
{
    // before
    feather::utils::windows::FramePositions framePositions{feather::pair{0, 10}, feather::pair{0, 20}};
    EXPECT_CALL(*dynamic_cast<NCursesProgressWindowDecoratorMock *>(progressWindow.get()), getParentWindowHandler())
        .WillOnce(::testing::Return(nCursesMainWindowDecoratorMock));
    EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getFramePositions())
        .WillRepeatedly(::testing::ReturnRef(framePositions));
    EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getStorage())
        .WillRepeatedly(::testing::ReturnRef(storageMock));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), convertVirtualPositionToByteOffset(std::pair<size_t, size_t>(0UL, 100UL), TEST_UUID))
        .WillOnce(::testing::Return(10U));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), convertVirtualPositionToByteOffset(std::pair<size_t, size_t>(0UL, 10UL), TEST_UUID))
        .WillOnce(::testing::Return(10U));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), convertVirtualPositionToByteOffset(std::pair<size_t, size_t>(0UL, 20UL), TEST_UUID))
        .WillOnce(::testing::Return(20));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getTotalBytesOfDeletions(TEST_UUID))
        .WillRepeatedly(::testing::Return(0));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getTotalBytesOfNewChanges(TEST_UUID))
        .WillRepeatedly(::testing::Return(100));
    EXPECT_CALL(*dynamic_cast<StorageMock *>(storageMock.get()), getSize())
        .WillOnce(::testing::Return(0));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getLastValidPosition(TEST_UUID, storageMock))
        .WillOnce(::testing::Return(std::pair<size_t, size_t>(0, 100)));

    // when
    std::u32string progress = windowStatus->getPrintedProgress(progressWindow.get(), printingOrchestratorMock, std::nullopt);

    // then
    FileStorageHelper::compareLine(progress.begin(), progress.end(), utf8Util, std::u32string(U"20").begin());
}

TEST_F(WindowStatusTest, getCorrectPrintProgressWhenFileEmptyAndNoChanges)
{
    // before
    feather::utils::windows::FramePositions framePositions{feather::pair{0, 0}, feather::pair{0, 0}};
    EXPECT_CALL(*dynamic_cast<NCursesProgressWindowDecoratorMock *>(progressWindow.get()), getParentWindowHandler())
        .WillOnce(::testing::Return(nCursesMainWindowDecoratorMock));
    EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getFramePositions())
        .WillRepeatedly(::testing::ReturnRef(framePositions));
    EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getStorage())
        .WillRepeatedly(::testing::ReturnRef(storageMock));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getTotalBytesOfDeletions(::testing::_))
        .WillRepeatedly(::testing::Return(0));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getTotalBytesOfNewChanges(::testing::_))
        .WillRepeatedly(::testing::Return(0));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), convertVirtualPositionToByteOffset(std::pair<size_t, size_t>(0, 0), TEST_UUID))
        .WillRepeatedly(::testing::Return(0));
    EXPECT_CALL(*dynamic_cast<StorageMock *>(storageMock.get()), getSize())
        .WillOnce(::testing::Return(0));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getLastValidPosition(TEST_UUID, storageMock))
        .WillOnce(::testing::Return(std::pair<size_t, size_t>(0, 0)));

    // when
    std::u32string progress = windowStatus->getPrintedProgress(progressWindow.get(), printingOrchestratorMock, std::nullopt);

    // then
    FileStorageHelper::compareLine(progress.begin(), progress.end(), utf8Util, std::u32string(U"100").begin());
}

TEST_F(WindowStatusTest, getCorrectPrintProgressWhenNotEmpty)
{
    // before
    feather::utils::windows::FramePositions framePositions{feather::pair{100, 10}, feather::pair{250, 19}};
    EXPECT_CALL(*dynamic_cast<NCursesProgressWindowDecoratorMock *>(progressWindow.get()), getParentWindowHandler())
        .WillOnce(::testing::Return(nCursesMainWindowDecoratorMock));
    EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getFramePositions())
        .WillRepeatedly(::testing::ReturnRef(framePositions));
    EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getStorage())
        .WillRepeatedly(::testing::ReturnRef(storageMock));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getTotalBytesOfDeletions(::testing::_))
        .WillRepeatedly(::testing::Return(10));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getTotalBytesOfNewChanges(::testing::_))
        .WillRepeatedly(::testing::Return(30));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), convertVirtualPositionToByteOffset(std::pair<size_t, size_t>(1000U, 0), TEST_UUID))
        .WillOnce(::testing::Return(0U));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), convertVirtualPositionToByteOffset(std::pair<size_t, size_t>(100U, 10), TEST_UUID))
        .WillOnce(::testing::Return(0U));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), convertVirtualPositionToByteOffset(std::pair<size_t, size_t>(250, 19), TEST_UUID))
        .WillRepeatedly(::testing::Return(15));
    EXPECT_CALL(*dynamic_cast<StorageMock *>(storageMock.get()), getSize())
        .WillOnce(::testing::Return(1000));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), getLastValidPosition(TEST_UUID, storageMock))
        .WillOnce(::testing::Return(std::pair<size_t, size_t>(1000, 0)));

    // when
    std::u32string progress = windowStatus->getPrintedProgress(progressWindow.get(), printingOrchestratorMock, std::nullopt);

    // then
    FileStorageHelper::compareLine(progress.begin(), progress.end(), utf8Util, std::u32string(U"27").begin());
}

TEST_F(WindowStatusTest, getCorrectPositionOnScreen)
{
    // before
    EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getUUID(false))
        .WillRepeatedly(::testing::ReturnRef(TEST_UUID));
    EXPECT_CALL(*dynamic_cast<NCursesProgressWindowDecoratorMock *>(progressWindow.get()), getParentWindowHandler())
        .WillOnce(::testing::Return(nCursesMainWindowDecoratorMock));
    EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getCursorRealPosition(::testing::_))
        .WillOnce(::testing::Return(std::make_pair(10, 0)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), convertVirtualPositionToByteOffset(std::pair<size_t, size_t>(10, 0), ::testing::_))
        .WillOnce(::testing::Return(10));

    // when
    std::u32string progress = windowStatus->getCharAbsolutePosition(progressWindow.get(), printingOrchestratorMock, std::nullopt);

    // then
    FileStorageHelper::compareLine(progress.begin(), progress.end(), utf8Util, std::u32string(U"10 B").begin());
}

TEST_F(WindowStatusTest, getCorrectPositionOnScreenWhenFileEmptyAndNoChanges)
{
    // before
    EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getUUID(false))
        .WillRepeatedly(::testing::ReturnRef(TEST_UUID));
    EXPECT_CALL(*dynamic_cast<NCursesProgressWindowDecoratorMock *>(progressWindow.get()), getParentWindowHandler())
        .WillOnce(::testing::Return(nCursesMainWindowDecoratorMock));
    EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getStorage())
        .WillRepeatedly(::testing::ReturnRef(storageMock));
    EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getCursorRealPosition(::testing::_))
        .WillOnce(::testing::Return(std::make_pair(0, 0)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), convertVirtualPositionToByteOffset(std::pair<size_t, size_t>(0, 0), ::testing::_))
        .WillOnce(::testing::Return(0));

    // when
    std::u32string progress = windowStatus->getCharAbsolutePosition(progressWindow.get(), printingOrchestratorMock, std::nullopt);

    // then
    FileStorageHelper::compareLine(progress.begin(), progress.end(), utf8Util, std::u32string(U"0 B").begin());
}

TEST_F(WindowStatusTest, printInfinityCharacter)
{
    // before
    EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getUUID(false))
        .WillRepeatedly(::testing::ReturnRef(TEST_UUID));
    EXPECT_CALL(*dynamic_cast<NCursesProgressWindowDecoratorMock *>(progressWindow.get()), getParentWindowHandler())
        .WillOnce(::testing::Return(nCursesMainWindowDecoratorMock));
    EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getStorage())
        .WillRepeatedly(::testing::ReturnRef(storageMock));
    EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getCursorRealPosition(::testing::_))
        .WillOnce(::testing::Return(std::make_pair(999999999999, 10)));
    EXPECT_CALL(*dynamic_cast<PrintingOrchestratorMock *>(printingOrchestratorMock.get()), convertVirtualPositionToByteOffset(std::pair<size_t, size_t>(999999999999, 10), ::testing::_))
        .WillOnce(::testing::Return(10000000000009));

    // when
    std::u32string progress = windowStatus->getCharAbsolutePosition(progressWindow.get(), printingOrchestratorMock, std::nullopt);

    // then
    ASSERT_EQ(progress[0], WindowStatus::INFINITY_CHARACTER);
}
