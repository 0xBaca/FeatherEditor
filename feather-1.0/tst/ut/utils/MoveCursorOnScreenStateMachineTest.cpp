#include "config/Config.hpp"
#include "helpers/TestBase.hpp"
#include "utils/Direction.hpp"
#include "utils/FeatherMode.hpp"
#include "mocks/NCursesWrapperMock.hpp"
#include "utils/MoveCursorOnScreenStateMachine.hpp"
#include "utils/windows/LinesAroundPositionInformation.hpp"
#include "utils/windows/WindowInformationInterface.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace feather::utils;
using namespace ::testing::mocks;

extern feather::utils::FEATHER_MODE currentFeatherMode;
extern std::unique_ptr<const feather::config::Configuration> configuration;

class MoveCursorOnScreenStateMachineTest : public ::testing::Test, public ::feather::test::TestBase
{
protected:
    std::unique_ptr<NCursesWrapper> nCursesWrapperMock;
    std::string testFileName;

public:
    void SetUp() override
    {
        testFileName = "testFile";
        nCursesWrapperMock = std::make_unique<::testing::mocks::NCursesWrapperMock>();
    }

    void TearDown() override
    {
    }
};

TEST_F(MoveCursorOnScreenStateMachineTest, moveCursorRightReadMode)
{
    configuration->forceTextMode(testFileName);
    currentFeatherMode = feather::utils::FEATHER_MODE::READ_MODE;
    //before
    feather::utils::ScreenBuffer screenBuffer(3);
    screenBuffer.fillBufferLine({0x8A989FF0, 97, 0x9584E2});
    screenBuffer.fillBufferLine({0xba98e2, 0x84C5, '\n'});
    screenBuffer.fillBufferLine({97, 0x9D84E2});
    size_t currCursorX = 0UL, currCursorY = 0UL;
    size_t screenRows = 100UL, screenColumns = 8UL;
    std::shared_ptr<feather::utils::windows::WindowInformationInterface> linesInformation = std::make_shared<feather::utils::windows::LinesAroundPositionInformation>(std::make_tuple(std::make_pair(0, 0), std::make_pair(4UL, false), std::make_pair(3UL, true)));

    //when LINE_0
    feather::pair newCursorPosition = MoveCursorOnScreenStateMachine::getCursorNewPosition(currCursorY, currCursorX, screenRows, screenColumns, linesInformation, Direction::RIGHT, std::nullopt, screenBuffer, false);

    //then
    ASSERT_EQ(newCursorPosition.first, 0UL);
    ASSERT_EQ(newCursorPosition.second, 2UL);

    //when
    currCursorX = 4;
    currCursorY = 0;
    newCursorPosition = MoveCursorOnScreenStateMachine::getCursorNewPosition(currCursorY, currCursorX, screenRows, screenColumns, linesInformation, Direction::RIGHT, std::nullopt, screenBuffer, false);

    //then
    ASSERT_EQ(newCursorPosition.first, 0UL);
    ASSERT_EQ(newCursorPosition.second, 6UL);

    //when LINE_1
    linesInformation = std::make_shared<feather::utils::windows::LinesAroundPositionInformation>(std::make_tuple(std::make_pair(4UL, false), std::make_pair(3UL, true), std::make_pair(2UL, false)));
    currCursorX = 0;
    currCursorY = 1UL;
    newCursorPosition = MoveCursorOnScreenStateMachine::getCursorNewPosition(currCursorY, currCursorX, screenRows, screenColumns, linesInformation, Direction::RIGHT, std::nullopt, screenBuffer, false);

    //then
    ASSERT_EQ(newCursorPosition.first, 1UL);
    ASSERT_EQ(newCursorPosition.second, 1UL);

    //when
    currCursorX = 3UL;
    currCursorY = 1UL;
    newCursorPosition = MoveCursorOnScreenStateMachine::getCursorNewPosition(currCursorY, currCursorX, screenRows, screenColumns, linesInformation, Direction::RIGHT, std::nullopt, screenBuffer, false);

    //then
    ASSERT_EQ(newCursorPosition.first, 1UL);
    ASSERT_EQ(newCursorPosition.second, 5UL);

    //when LINE_2
    linesInformation = std::make_shared<feather::utils::windows::LinesAroundPositionInformation>(std::make_tuple(std::make_pair(3UL, true), std::make_pair(2UL, false), std::make_pair(0UL, false)));
    currCursorX = 0;
    currCursorY = 2UL;
    newCursorPosition = MoveCursorOnScreenStateMachine::getCursorNewPosition(currCursorY, currCursorX, screenRows, screenColumns, linesInformation, Direction::RIGHT, std::nullopt, screenBuffer, false);

    //then
    ASSERT_EQ(newCursorPosition.first, 2UL);
    ASSERT_EQ(newCursorPosition.second, 1UL);

    //when
    currCursorX = 1;
    currCursorY = 2UL;
    newCursorPosition = MoveCursorOnScreenStateMachine::getCursorNewPosition(currCursorY, currCursorX, screenRows, screenColumns, linesInformation, Direction::RIGHT, std::nullopt, screenBuffer, false);

    //then
    ASSERT_EQ(newCursorPosition.first, 2UL);
    ASSERT_EQ(newCursorPosition.second, 1UL);
}

TEST_F(MoveCursorOnScreenStateMachineTest, moveCursorRightWriteMode)
{
    currentFeatherMode = feather::utils::FEATHER_MODE::WRITE_MODE;
    //before
    configuration->forceTextMode(testFileName);
    feather::utils::ScreenBuffer screenBuffer(3);
    screenBuffer.fillBufferLine({0x8A989FF0, 97, 0x9584E2});
    screenBuffer.fillBufferLine({0xba98e2, 0x84C5, '\n'});
    screenBuffer.fillBufferLine({97, 0x9D84E2});
    size_t currCursorX = 0UL, currCursorY = 0UL;
    size_t screenRows = 100UL, screenColumns = 8UL;
    std::shared_ptr<feather::utils::windows::WindowInformationInterface> linesInformation = std::make_shared<feather::utils::windows::LinesAroundPositionInformation>(std::make_tuple(std::make_pair(0, 0), std::make_pair(4UL, false), std::make_pair(3UL, true)));

    //when LINE_0
    feather::pair newCursorPosition = MoveCursorOnScreenStateMachine::getCursorNewPosition(currCursorY, currCursorX, screenRows, screenColumns, linesInformation, Direction::RIGHT, std::nullopt, screenBuffer, false);

    //then
    ASSERT_EQ(newCursorPosition.first, 0UL);
    ASSERT_EQ(newCursorPosition.second, 2UL);

    //when
    currCursorX = 3;
    currCursorY = 0;
    newCursorPosition = MoveCursorOnScreenStateMachine::getCursorNewPosition(currCursorY, currCursorX, screenRows, screenColumns, linesInformation, Direction::RIGHT, std::nullopt, screenBuffer, false);

    //then
    ASSERT_EQ(newCursorPosition.first, 1UL);
    ASSERT_EQ(newCursorPosition.second, 0UL);

    //when LINE_1
    linesInformation = std::make_shared<feather::utils::windows::LinesAroundPositionInformation>(std::make_tuple(std::make_pair(4UL, false), std::make_pair(3UL, true), std::make_pair(2UL, false)));
    currCursorX = 0;
    currCursorY = 1UL;
    newCursorPosition = MoveCursorOnScreenStateMachine::getCursorNewPosition(currCursorY, currCursorX, screenRows, screenColumns, linesInformation, Direction::RIGHT, std::nullopt, screenBuffer, false);

    //then
    ASSERT_EQ(newCursorPosition.first, 1UL);
    ASSERT_EQ(newCursorPosition.second, 1UL);

    //when
    currCursorX = 3UL;
    currCursorY = 1UL;
    newCursorPosition = MoveCursorOnScreenStateMachine::getCursorNewPosition(currCursorY, currCursorX, screenRows, screenColumns, linesInformation, Direction::RIGHT, std::nullopt, screenBuffer, false);

    //then
    ASSERT_EQ(newCursorPosition.first, 1UL);
    ASSERT_EQ(newCursorPosition.second, 5UL);

    //when
    currCursorX = 3UL;
    currCursorY = 1UL;
    newCursorPosition = MoveCursorOnScreenStateMachine::getCursorNewPosition(currCursorY, currCursorX, screenRows, screenColumns, linesInformation, Direction::RIGHT, std::nullopt, screenBuffer, false);

    //then
    ASSERT_EQ(newCursorPosition.first, 1UL);
    ASSERT_EQ(newCursorPosition.second, 5UL);

    //when LINE_2
    linesInformation = std::make_shared<feather::utils::windows::LinesAroundPositionInformation>(std::make_tuple(std::make_pair(3UL, true), std::make_pair(2UL, false), std::make_pair(0UL, false)));
    currCursorX = 0;
    currCursorY = 2UL;
    newCursorPosition = MoveCursorOnScreenStateMachine::getCursorNewPosition(currCursorY, currCursorX, screenRows, screenColumns, linesInformation, Direction::RIGHT, std::nullopt, screenBuffer, false);

    //then
    ASSERT_EQ(newCursorPosition.first, 2UL);
    ASSERT_EQ(newCursorPosition.second, 1UL);

    //when
    currCursorX = 1;
    currCursorY = 2UL;
    newCursorPosition = MoveCursorOnScreenStateMachine::getCursorNewPosition(currCursorY, currCursorX, screenRows, screenColumns, linesInformation, Direction::RIGHT, std::nullopt, screenBuffer, false);

    //then
    ASSERT_EQ(newCursorPosition.first, 2UL);
    ASSERT_EQ(newCursorPosition.second, 2UL);

    //when
    currCursorX = 4;
    currCursorY = 2UL;
    newCursorPosition = MoveCursorOnScreenStateMachine::getCursorNewPosition(currCursorY, currCursorX, screenRows, screenColumns, linesInformation, Direction::RIGHT, std::nullopt, screenBuffer, false);

    //then
    ASSERT_EQ(newCursorPosition.first, 2UL);
    ASSERT_EQ(newCursorPosition.second, 4UL);
}

TEST_F(MoveCursorOnScreenStateMachineTest, moveCursorLeft)
{
    currentFeatherMode = feather::utils::FEATHER_MODE::READ_MODE;
    //before
    configuration->forceTextMode(testFileName);
    feather::utils::ScreenBuffer screenBuffer(3);
    screenBuffer.fillBufferLine({0x8A989FF0, 97, 0x9584E2});
    screenBuffer.fillBufferLine({0xba98e2, 0x84C5, '\n'});
    screenBuffer.fillBufferLine({97, 0x9D84E2});
    size_t currCursorX = 1UL, currCursorY = 2UL;
    size_t screenRows = 100UL, screenColumns = 8UL;
    std::shared_ptr<feather::utils::windows::WindowInformationInterface> linesInformation = std::make_shared<feather::utils::windows::LinesAroundPositionInformation>(std::make_tuple(std::make_pair(3UL, true), std::make_pair(2UL, false), std::make_pair(0UL, false)));

    //when LINE_3
    feather::pair newCursorPosition = MoveCursorOnScreenStateMachine::getCursorNewPosition(currCursorY, currCursorX, screenRows, screenColumns, linesInformation, Direction::LEFT, std::nullopt, screenBuffer, false);

    //then
    ASSERT_EQ(newCursorPosition.first, 2UL);
    ASSERT_EQ(newCursorPosition.second, 0UL);

    //when
    currCursorX = 0UL;
    currCursorY = 2UL;
    newCursorPosition = MoveCursorOnScreenStateMachine::getCursorNewPosition(currCursorY, currCursorX, screenRows, screenColumns, linesInformation, Direction::LEFT, std::nullopt, screenBuffer, false);

    //then
    ASSERT_EQ(newCursorPosition.first, 2UL);
    ASSERT_EQ(newCursorPosition.second, 0UL);

    //when LINE_2_
    linesInformation = std::make_shared<feather::utils::windows::LinesAroundPositionInformation>(std::make_tuple(std::make_pair(3UL, false), std::make_pair(3UL, true), std::make_pair(2UL, false)));
    currCursorX = 5UL;
    currCursorY = 1UL;
    newCursorPosition = MoveCursorOnScreenStateMachine::getCursorNewPosition(currCursorY, currCursorX, screenRows, screenColumns, linesInformation, Direction::LEFT, std::nullopt, screenBuffer, false);

    //then
    ASSERT_EQ(newCursorPosition.first, 1UL);
    ASSERT_EQ(newCursorPosition.second, 4UL);

    //when
    currCursorX = 3UL;
    currCursorY = 1UL;
    newCursorPosition = MoveCursorOnScreenStateMachine::getCursorNewPosition(currCursorY, currCursorX, screenRows, screenColumns, linesInformation, Direction::LEFT, std::nullopt, screenBuffer, false);

    //then
    ASSERT_EQ(newCursorPosition.first, 1UL);
    ASSERT_EQ(newCursorPosition.second, 2UL);

    //when
    currCursorX = 0UL;
    currCursorY = 1UL;
    newCursorPosition = MoveCursorOnScreenStateMachine::getCursorNewPosition(currCursorY, currCursorX, screenRows, screenColumns, linesInformation, Direction::LEFT, std::nullopt, screenBuffer, false);

    //then
    ASSERT_EQ(newCursorPosition.first, 0UL);
    ASSERT_EQ(newCursorPosition.second, 3UL);

    //when LINE_1
    linesInformation = std::make_shared<feather::utils::windows::LinesAroundPositionInformation>(std::make_tuple(std::make_pair(0UL, false), std::make_pair(3UL, false), std::make_pair(3UL, true)));
    currCursorX = 5UL;
    currCursorY = 0UL;
    newCursorPosition = MoveCursorOnScreenStateMachine::getCursorNewPosition(currCursorY, currCursorX, screenRows, screenColumns, linesInformation, Direction::LEFT, std::nullopt, screenBuffer, false);

    //then
    ASSERT_EQ(newCursorPosition.first, 0UL);
    ASSERT_EQ(newCursorPosition.second, 4UL);

    //when
    currCursorX = 4;
    currCursorY = 0UL;
    newCursorPosition = MoveCursorOnScreenStateMachine::getCursorNewPosition(currCursorY, currCursorX, screenRows, screenColumns, linesInformation, Direction::LEFT, std::nullopt, screenBuffer, false);

    //then
    ASSERT_EQ(newCursorPosition.first, 0UL);
    ASSERT_EQ(newCursorPosition.second, 3UL);
}

TEST_F(MoveCursorOnScreenStateMachineTest, moveCursorLeftWhenCharDeletedInLeftTopCorner)
{
    currentFeatherMode = feather::utils::FEATHER_MODE::READ_MODE;
    //before
    configuration->forceTextMode(testFileName);
    feather::utils::ScreenBuffer screenBuffer(3);
    screenBuffer.fillBufferLine({0x8A989FF0, 97, '\n'});
    screenBuffer.fillBufferLine({0xba98e2, 0x84C5});
    screenBuffer.fillBufferLine({97, 0x9D84E2});
    size_t currCursorX = 0UL, currCursorY = 1UL;
    size_t screenRows = 100UL, screenColumns = 8UL;
    std::shared_ptr<feather::utils::windows::WindowInformationInterface> linesInformation = std::make_shared<feather::utils::windows::LinesAroundPositionInformation>(std::make_tuple(std::make_pair(2UL, true), std::make_pair(2UL, false), std::make_pair(2UL, false)));

    //when LINE_2
    feather::pair newCursorPosition = MoveCursorOnScreenStateMachine::getCursorNewPosition(currCursorY, currCursorX, screenRows, screenColumns, linesInformation, Direction::LEFT, 3, screenBuffer, false);

    //then
    ASSERT_EQ(newCursorPosition.first, 0UL);
    ASSERT_EQ(newCursorPosition.second, 3UL);

    //when LINE_3
    currCursorX = 0UL;
    currCursorY = 2UL;
    linesInformation = std::make_shared<feather::utils::windows::LinesAroundPositionInformation>(std::make_tuple(std::make_pair(2UL, false), std::make_pair(2UL, false), std::make_pair(0UL, false)));
    newCursorPosition = MoveCursorOnScreenStateMachine::getCursorNewPosition(currCursorY, currCursorX, screenRows, screenColumns, linesInformation, Direction::LEFT, 2, screenBuffer, false);

    //then
    ASSERT_EQ(newCursorPosition.first, 1UL);
    ASSERT_EQ(newCursorPosition.second, 1UL);
}

TEST_F(MoveCursorOnScreenStateMachineTest, moveCursorUpWhenReadMode)
{
    currentFeatherMode = feather::utils::FEATHER_MODE::READ_MODE;
    //before
    configuration->forceTextMode(testFileName);
    feather::utils::ScreenBuffer screenBuffer(3);
    screenBuffer.fillBufferLine({97, '\n'});
    screenBuffer.fillBufferLine({0xba98e2, 0x84C5});
    screenBuffer.fillBufferLine({97, 0x9D84E2});
    size_t currCursorX = 3UL, currCursorY = 1UL;
    size_t screenRows = 100UL, screenColumns = 8UL;
    std::shared_ptr<feather::utils::windows::WindowInformationInterface> linesInformation = std::make_shared<feather::utils::windows::LinesAroundPositionInformation>(std::make_tuple(std::make_pair(2UL, true), std::make_pair(2UL, false), std::make_pair(2UL, false)));

    //when LINE_2
    feather::pair newCursorPosition = MoveCursorOnScreenStateMachine::getCursorNewPosition(currCursorY, currCursorX, screenRows, screenColumns, linesInformation, Direction::UP, std::nullopt, screenBuffer, false);

    //then
    ASSERT_EQ(newCursorPosition.first, 0UL);
    ASSERT_EQ(newCursorPosition.second, 0UL);

    //when LINE_3
    currCursorX = 1UL;
    currCursorY = 2UL;
    linesInformation = std::make_shared<feather::utils::windows::LinesAroundPositionInformation>(std::make_tuple(std::make_pair(2UL, false), std::make_pair(2UL, false), std::make_pair(0UL, false)));
    newCursorPosition = MoveCursorOnScreenStateMachine::getCursorNewPosition(currCursorY, currCursorX, screenRows, screenColumns, linesInformation, Direction::UP, std::nullopt, screenBuffer, false);

    //then
    ASSERT_EQ(newCursorPosition.first, 1UL);
    ASSERT_EQ(newCursorPosition.second, 0UL);
}

TEST_F(MoveCursorOnScreenStateMachineTest, moveCursorUpWhenWriteMode)
{
    //before
    currentFeatherMode = feather::utils::FEATHER_MODE::WRITE_MODE;
    configuration->forceTextMode(testFileName);
    feather::utils::ScreenBuffer screenBuffer(3);
    screenBuffer.fillBufferLine({97, '\n'});
    screenBuffer.fillBufferLine({0xba98e2, 0x84C5});
    screenBuffer.fillBufferLine({97, 0x9D84E2});
    size_t currCursorX = 3UL, currCursorY = 1UL;
    size_t screenRows = 100UL, screenColumns = 8UL;
    std::shared_ptr<feather::utils::windows::WindowInformationInterface> linesInformation = std::make_shared<feather::utils::windows::LinesAroundPositionInformation>(std::make_tuple(std::make_pair(2UL, true), std::make_pair(2UL, false), std::make_pair(2UL, false)));

    //when LINE_2
    feather::pair newCursorPosition = MoveCursorOnScreenStateMachine::getCursorNewPosition(currCursorY, currCursorX, screenRows, screenColumns, linesInformation, Direction::UP, std::nullopt, screenBuffer, false);

    //then
    ASSERT_EQ(newCursorPosition.first, 0UL);
    ASSERT_EQ(newCursorPosition.second, 1UL);

    //when LINE_3
    currCursorX = 1UL;
    currCursorY = 2UL;
    linesInformation = std::make_shared<feather::utils::windows::LinesAroundPositionInformation>(std::make_tuple(std::make_pair(2UL, false), std::make_pair(2UL, false), std::make_pair(0UL, false)));
    newCursorPosition = MoveCursorOnScreenStateMachine::getCursorNewPosition(currCursorY, currCursorX, screenRows, screenColumns, linesInformation, Direction::UP, std::nullopt, screenBuffer, false);

    //then
    ASSERT_EQ(newCursorPosition.first, 1UL);
    ASSERT_EQ(newCursorPosition.second, 1UL);
}

TEST_F(MoveCursorOnScreenStateMachineTest, moveCursorDownWhenReadMode)
{
    currentFeatherMode = feather::utils::FEATHER_MODE::READ_MODE;
    //before
    configuration->forceTextMode(testFileName);
    feather::utils::ScreenBuffer screenBuffer(3);
    screenBuffer.fillBufferLine({97, '\n'});
    screenBuffer.fillBufferLine({0xba98e2, 0x84C5});
    screenBuffer.fillBufferLine({97, 0x9D84E2});
    size_t currCursorX = 1UL, currCursorY = 0UL;
    size_t screenRows = 100UL, screenColumns = 8UL;
    std::shared_ptr<feather::utils::windows::WindowInformationInterface> linesInformation = std::make_shared<feather::utils::windows::LinesAroundPositionInformation>(std::make_tuple(std::make_pair(0UL, false), std::make_pair(1UL, true), std::make_pair(2UL, false)));

    //when LINE_1
    feather::pair newCursorPosition = MoveCursorOnScreenStateMachine::getCursorNewPosition(currCursorY, currCursorX, screenRows, screenColumns, linesInformation, Direction::DOWN, std::nullopt, screenBuffer, false);

    //then
    ASSERT_EQ(newCursorPosition.first, 1UL);
    ASSERT_EQ(newCursorPosition.second, 1UL);

    //when LINE_2
    currCursorX = 3UL;
    currCursorY = 1UL;
    linesInformation = std::make_shared<feather::utils::windows::LinesAroundPositionInformation>(std::make_tuple(std::make_pair(2UL, true), std::make_pair(2UL, false), std::make_pair(2UL, false)));
    newCursorPosition = MoveCursorOnScreenStateMachine::getCursorNewPosition(currCursorY, currCursorX, screenRows, screenColumns, linesInformation, Direction::DOWN, std::nullopt, screenBuffer, false);

    //then
    ASSERT_EQ(newCursorPosition.first, 2UL);
    ASSERT_EQ(newCursorPosition.second, 1UL);
}

TEST_F(MoveCursorOnScreenStateMachineTest, moveCursorDownWhenWriteMode)
{
    currentFeatherMode = feather::utils::FEATHER_MODE::WRITE_MODE;
    //before
    configuration->forceTextMode(testFileName);
    feather::utils::ScreenBuffer screenBuffer(3);
    screenBuffer.fillBufferLine({97, '\n'});
    screenBuffer.fillBufferLine({0xba98e2, 0x84C5});
    screenBuffer.fillBufferLine({97, 0x9D84E2});
    size_t currCursorX = 1UL, currCursorY = 0UL;
    size_t screenRows = 100UL, screenColumns = 8UL;
    std::shared_ptr<feather::utils::windows::WindowInformationInterface> linesInformation = std::make_shared<feather::utils::windows::LinesAroundPositionInformation>(std::make_tuple(std::make_pair(0UL, false), std::make_pair(1UL, true), std::make_pair(2UL, false)));

    //when LINE_1
    feather::pair newCursorPosition = MoveCursorOnScreenStateMachine::getCursorNewPosition(currCursorY, currCursorX, screenRows, screenColumns, linesInformation, Direction::DOWN, std::nullopt, screenBuffer, false);

    //then
    ASSERT_EQ(newCursorPosition.first, 1UL);
    ASSERT_EQ(newCursorPosition.second, 1UL);

    //when LINE_2
    currCursorX = 6UL;
    currCursorY = 1UL;
    linesInformation = std::make_shared<feather::utils::windows::LinesAroundPositionInformation>(std::make_tuple(std::make_pair(2UL, true), std::make_pair(2UL, false), std::make_pair(2UL, false)));
    newCursorPosition = MoveCursorOnScreenStateMachine::getCursorNewPosition(currCursorY, currCursorX, screenRows, screenColumns, linesInformation, Direction::DOWN, std::nullopt, screenBuffer, false);

    //then
    ASSERT_EQ(newCursorPosition.first, 2UL);
    ASSERT_EQ(newCursorPosition.second, 2UL);
}
