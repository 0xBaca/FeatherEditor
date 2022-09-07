#include "helpers/TestBase.hpp"
#include "utils/helpers/Conversion.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace feather::utils::helpers
{
    class ConversionTest : public ::testing::Test, public ::feather::test::TestBase
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

    TEST_F(ConversionTest, escapeSpecialCharacters)
    {
        //before
        std::u32string string{'\\', 'n'};
        std::u32string expectedResult{0x0A};

        //when
        auto result = utils::helpers::Conversion::escapeSpecialCharacters(string);

        //then
        ASSERT_EQ(result, expectedResult);

        //when
        string = {'\\', 'n', 'x', '\\', 'n'};
        expectedResult = {0x0A, 'x', 0x0A};
        result = utils::helpers::Conversion::escapeSpecialCharacters(string);

        //then
        ASSERT_EQ(result, expectedResult);

        //when
        string = {};
        expectedResult = {};
        result = utils::helpers::Conversion::escapeSpecialCharacters(string);

        //then
        ASSERT_EQ(result, expectedResult);

        //when
        string = {'\\', 'n', ' '};
        expectedResult = {0x0A, ' '};

        //then
        result = utils::helpers::Conversion::escapeSpecialCharacters(string);

        //when
        string = {'\\', '\\'};
        expectedResult = {'\\'};

        //then
        result = utils::helpers::Conversion::escapeSpecialCharacters(string);

        //when
        string = {'\\', '\\', '\\'};
        expectedResult = {};

        //then
        result = utils::helpers::Conversion::escapeSpecialCharacters(string);

        //when
        string = {'\\', '\\', '\\', '\\'};
        expectedResult = {'\\', '\\'};

        //then
        result = utils::helpers::Conversion::escapeSpecialCharacters(string);
    }

    TEST_F(ConversionTest, unescapeSlashes)
    {
        //before
        std::u32string string(U"\\\\");
        std::u32string expectedResult(U"\\");

        //when
        auto result = Conversion::unescapeSlashes(string);

        //then
        ASSERT_EQ(result, expectedResult);

        //before
        string = U"\\\\\\";
        expectedResult = U"\\\\";

        //when
        result = Conversion::unescapeSlashes(string);

        //then
        ASSERT_EQ(result, expectedResult);

        //before
        string = U"a\\\\\\a";
        expectedResult = U"a\\\\a";

        //when
        result = Conversion::unescapeSlashes(string);

        //then
        ASSERT_EQ(result, expectedResult);

        //before
        string = {92, 92, 92, 27};
        expectedResult = {92, 92, 27};

        //when
        result = Conversion::unescapeSlashes(string);

        //then
        ASSERT_EQ(result, expectedResult);
    }

    TEST_F(ConversionTest, splitStringWithQuote)
    {
        //before
        std::u32string string(U":color yellow ''");
        std::vector<std::u32string> expectedResult{U":color", U"yellow"};

        //when
        auto result = Conversion::splitStringWithQuote(string);

        //then
        ASSERT_EQ(result, expectedResult);

        //before
        string = U":color yellow '''";
        expectedResult = {};

        //when
        result = Conversion::splitStringWithQuote(string);

        //then
        ASSERT_EQ(result, expectedResult);

        //before
        string = U":color yellow '\\''";
        expectedResult = {U":color", U"yellow", {0x5C, 0x27}};

        //when
        result = Conversion::splitStringWithQuote(string);

        //then
        ASSERT_EQ(result, expectedResult);

        //before
        string = std::u32string{U":color yellow "} + std::u32string{92, 92, 92, 27};
        expectedResult = {U":color", U"yellow", {92, 92, 27}};

        //when
        result = Conversion::splitStringWithQuote(string);

        //then
        ASSERT_EQ(result, expectedResult);

        //before
        string = std::u32string{U":color yellow "} + std::u32string{92, 'a', 92, 92, 27};
        expectedResult = {U":color", U"yellow", {92, 'a', 92, 27}};

        //when
        result = Conversion::splitStringWithQuote(string);

        //then
        ASSERT_EQ(result, expectedResult);

        //before
        string = std::u32string{U":color yellow "} + std::u32string{39, 'a', ' ', 92, 39};
        expectedResult = {U":color", U"yellow", {'a', ' ', 92}};

        //when
        result = Conversion::splitStringWithQuote(string);

        //then
        ASSERT_EQ(result, expectedResult);
    }

    TEST_F(ConversionTest, splitStringWithQuoteNoQuotes)
    {
        //before
        std::u32string string(U":color yellow xx\n");
        std::vector<std::u32string> expectedResult{U":color", U"yellow", U"xx\n"};

        //when
        auto result = Conversion::splitStringWithQuote(string);

        //then
        ASSERT_EQ(result, expectedResult);

        //when
        string = U":color yellow x  x\n";
        expectedResult = {};
        result = Conversion::splitStringWithQuote(string);

        //then
        ASSERT_EQ(result, expectedResult);
    }
} // namespace feather::utils::algorithm