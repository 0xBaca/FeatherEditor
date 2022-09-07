#include "helpers/TestBase.hpp"
#include "utils/algorithm/RegexSearch.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace feather::utils::algorithm
{
    class RegexSearchTest : public ::testing::Test, public ::feather::test::TestBase
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

    TEST_F(RegexSearchTest, searchForHighlightedRegex)
    {
        // expect
        // before
        auto searchedBuffer = U"x/*some comments*/";
        auto searchedRegex = U"/\\*.*\\*/";
        utils::algorithm::RegexSearch regexSearch(searchedBuffer);

        // when
        auto result = regexSearch.search(searchedRegex);

        // then
        EXPECT_THAT(result, ::testing::ContainerEq(std::set<std::u32string>{U"/*some comments*/"}));
    }

    TEST_F(RegexSearchTest, searchForHighlightedRegexInMultipleLines)
    {
        // expect
        // before
        auto searchedBuffer = U"x/*some\n comments*/";
        auto searchedRegex = U"/\\*(.|\n)*\\*/";
        utils::algorithm::RegexSearch regexSearch(searchedBuffer);

        // when
        auto result = regexSearch.search(searchedRegex);

        // then
        EXPECT_THAT(result, ::testing::ContainerEq(std::set<std::u32string>{U"/*some\n comments*/"}));
    }
}
