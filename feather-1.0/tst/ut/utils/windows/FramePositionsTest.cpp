#include "utils/windows/FramePositions.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace feather;
using namespace feather::utils::windows;

class FramePositionsTest : public ::testing::Test
{
protected:
public:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(FramePositionsTest, insideFrame)
{
    //before
    //when
    FramePositions framePositions_1 = FramePositions{pair{0, 0}, pair{10, 10}};
    FramePositions framePositions_2 = FramePositions{pair{10, 10}, pair{20, 20}};
    FramePositions framePositions_3 = FramePositions{pair{10, 10}, pair{10, 100}};

    //then
    ASSERT_TRUE(framePositions_1.isInsideFrame(std::make_pair<size_t, size_t>(0, 0)));
    ASSERT_TRUE(framePositions_1.isInsideFrame(std::make_pair<size_t, size_t>(10, 9)));
    ASSERT_TRUE(framePositions_1.isInsideFrame(std::make_pair<size_t, size_t>(5, 10)));

    ASSERT_TRUE(framePositions_2.isInsideFrame(std::make_pair<size_t, size_t>(10, 15)));
    ASSERT_TRUE(framePositions_2.isInsideFrame(std::make_pair<size_t, size_t>(15, 15)));
    ASSERT_TRUE(framePositions_2.isInsideFrame(std::make_pair<size_t, size_t>(20, 15)));

    ASSERT_TRUE(framePositions_3.isInsideFrame(std::make_pair<size_t, size_t>(10, 20)));
}

TEST_F(FramePositionsTest, outsideFrame)
{
    //before
    //when
    FramePositions framePositions_1 = FramePositions{pair{0, 0}, pair{10, 10}};
    FramePositions framePositions_2 = FramePositions{pair{10, 10}, pair{20, 20}};
    FramePositions framePositions_3 = FramePositions{pair{10, 10}, pair{20, 0}};

    //then
    ASSERT_TRUE(framePositions_1.isInsideFrame(std::make_pair<size_t, size_t>(10, 10)));
    ASSERT_FALSE(framePositions_1.isInsideFrame(std::make_pair<size_t, size_t>(10, 11)));
    ASSERT_FALSE(framePositions_1.isInsideFrame(std::make_pair<size_t, size_t>(11, 0)));

    ASSERT_FALSE(framePositions_2.isInsideFrame(std::make_pair<size_t, size_t>(10, 9)));
    ASSERT_FALSE(framePositions_2.isInsideFrame(std::make_pair<size_t, size_t>(9, 9)));
    ASSERT_TRUE(framePositions_2.isInsideFrame(std::make_pair<size_t, size_t>(20, 20)));
    ASSERT_FALSE(framePositions_2.isInsideFrame(std::make_pair<size_t, size_t>(20, 21)));

    ASSERT_TRUE(framePositions_3.isInsideFrame(std::make_pair<size_t, size_t>(20, 0)));
}
