#include "mocks/BufferFillerMock.hpp"
#include "mocks/NCursesMainWindowDecoratorMock.hpp"
#include "mocks/NCursesWindowMock.hpp"
#include "mocks/NCursesWrapperMock.hpp"
#include "mocks/PrintingOrchestratorMock.hpp"
#include "printer/PrintingOrchestrator.hpp"
#include "utils/ScreenBuffer.hpp"
#include "utils/windows/WindowLinesLengthInformationVisitor.hpp"
#include "utils/windows/LinesAroundPositionInformation.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace ::testing::mocks;
using namespace feather::utils::windows;

class WindowLinesAroundPositionInformationVisitorTest : public ::testing::Test
{
protected:
    std::unique_ptr<BufferFillerInterface> bufferFillerMock;
    std::shared_ptr<MainWindowInterface> nCursesMainWindowDecoratorMock;

public:
    void SetUp() override
    {
        bufferFillerMock = std::make_unique<BufferFillerMock>();
        nCursesMainWindowDecoratorMock = std::make_shared<NCursesMainWindowDecoratorMock>();
    }

    void TearDown() override
    {
    }
};

TEST_F(WindowLinesAroundPositionInformationVisitorTest, fileEmpty)
{
    // before
    feather::utils::ScreenBuffer buffer(std::vector<char32_t>(), 10);
    EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getCursorPosition())
        .WillOnce(::testing::Return(std::make_pair<size_t, size_t>(0, 0)));
    EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getWindowDimensions())
        .WillRepeatedly(::testing::Return(std::make_pair<size_t, size_t>(10, 10)));
    EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getBufferFiller())
        .WillOnce(::testing::ReturnRef(bufferFillerMock));
    EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getScreenBuffer())
        .WillOnce(::testing::ReturnRef(buffer));
    std::shared_ptr<WindowLinesLengthInformationVisitor> lineLenInformation = std::make_shared<WindowLinesLengthInformationVisitor>();

    // when
    auto result = lineLenInformation->visit(static_cast<NCursesMainWindowDecorator *>(nCursesMainWindowDecoratorMock.get()))->getInformation();

    // then
    ASSERT_EQ(std::get<0>(result).first, 0U);
    ASSERT_EQ(std::get<1>(result).first, 0U);
    ASSERT_EQ(std::get<2>(result).first, 0U);
}

TEST_F(WindowLinesAroundPositionInformationVisitorTest, fileEmptyButWithOneChange)
{
    // before
    feather::utils::ScreenBuffer buffer(std::vector<char32_t>(), 10);
    buffer[0].push_back('a');
    EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getCursorPosition())
        .WillOnce(::testing::Return(std::make_pair<size_t, size_t>(0, 1)));
    EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getWindowDimensions())
        .WillRepeatedly(::testing::Return(std::make_pair<size_t, size_t>(10, 10)));
    EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getBufferFiller())
        .WillOnce(::testing::ReturnRef(bufferFillerMock));
    EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getScreenBuffer())
        .WillOnce(::testing::ReturnRef(buffer));

    // when
    std::shared_ptr<WindowLinesLengthInformationVisitor> lineLenInformation = std::make_shared<WindowLinesLengthInformationVisitor>();
    auto result = dynamic_cast<feather::utils::windows::LinesAroundPositionInformation *>(lineLenInformation->visit(static_cast<NCursesMainWindowDecorator *>(nCursesMainWindowDecoratorMock.get())).get())->getInformation();

    // then
    ASSERT_EQ(std::get<0>(result).first, 0U);
    ASSERT_EQ(std::get<1>(result).first, 1U);
    ASSERT_EQ(std::get<2>(result).first, 0U);
}

TEST_F(WindowLinesAroundPositionInformationVisitorTest, oneChangeInCurrLineAndBelow)
{
    // before
    feather::utils::ScreenBuffer buffer(std::vector<char32_t>(), 10);
    buffer[0].push_back('a');
    buffer[1].push_back('a');
    EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getCursorPosition())
        .WillOnce(::testing::Return(std::make_pair<size_t, size_t>(0, 1)));
    EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getWindowDimensions())
        .WillRepeatedly(::testing::Return(std::make_pair<size_t, size_t>(10, 10)));
    EXPECT_CALL(*dynamic_cast<NCursesMainWindowDecoratorMock *>(nCursesMainWindowDecoratorMock.get()), getBufferFiller())
        .WillOnce(::testing::ReturnRef(bufferFillerMock));
    EXPECT_CALL(*dynamic_cast<BufferFillerMock *>(bufferFillerMock.get()), getScreenBuffer())
        .WillOnce(::testing::ReturnRef(buffer));

    // when
    std::shared_ptr<WindowLinesLengthInformationVisitor>
        lineLenInformation = std::make_shared<WindowLinesLengthInformationVisitor>();
    auto result = dynamic_cast<feather::utils::windows::LinesAroundPositionInformation *>(lineLenInformation->visit(static_cast<NCursesMainWindowDecorator *>(nCursesMainWindowDecoratorMock.get())).get())->getInformation();

    // then
    ASSERT_EQ(std::get<0>(result).first, 0U);
    ASSERT_EQ(std::get<1>(result).first, 1U);
    ASSERT_EQ(std::get<2>(result).first, 1U);
}
