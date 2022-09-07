#include "helpers/TestBase.hpp"
#include "helpers/FileStorageHelper.hpp"
#include "utils/ScreenBuffer.hpp"

namespace feather::utils
{
  class ScreenBufferTest : public ::testing::Test, public ::feather::test::TestBase
  {
  protected:
    std::shared_ptr<Utf8Util> utf8Util;

  public:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
  };

  TEST_F(ScreenBufferTest, createScreenBufferWithoutException)
  {
    // before
    std::unique_ptr<ScreenBuffer> rawWindowBuffer;

    // when
    ASSERT_NO_THROW(rawWindowBuffer = std::make_unique<ScreenBuffer>(10));

    // then
    ASSERT_THAT(rawWindowBuffer->getFilledRows(), ::testing::Eq(0));
  }

  TEST_F(ScreenBufferTest, fillBufferLine)
  {
    // before
    std::unique_ptr<ScreenBuffer> rawWindowBuffer;

    // when
    ASSERT_NO_THROW(rawWindowBuffer = std::make_unique<ScreenBuffer>(10));
    rawWindowBuffer->fillBufferLine(std::u32string(U"Test line"));

    // then
    FileStorageHelper::compareLine((*rawWindowBuffer)[0].begin(), (*rawWindowBuffer)[0].end(), utf8Util, std::u32string(U"Test line").begin());
  }

  TEST_F(ScreenBufferTest, applyBlinkingForHexString)
  {
    // before
    std::unique_ptr<ScreenBuffer> rawWindowBuffer;

    // when
    ASSERT_NO_THROW(rawWindowBuffer = std::make_unique<ScreenBuffer>(1UL));
    rawWindowBuffer->fillBufferLine(U"9A9B");
    rawWindowBuffer->posMappedToHex.insert(pair(0, 0));
    rawWindowBuffer->posMappedToHex.insert(pair(0, 1UL));
    rawWindowBuffer->posMappedToHex.insert(pair(0, 2UL));
    rawWindowBuffer->posMappedToHex.insert(pair(0, 3UL));
    rawWindowBuffer->posMappedToHexStart.insert(pair(0, 0));
    rawWindowBuffer->posMappedToHexStart.insert(pair(0, 2UL));
    rawWindowBuffer->applyBlinking(std::pair(pair(0UL, 0UL), pair(1UL, 0UL)));

    // then
    ASSERT_FALSE(rawWindowBuffer->blinkMap.empty());
    ASSERT_TRUE(rawWindowBuffer->blinkMap[pair(0, 0UL)] == pair(1UL, 0UL));
  }
}