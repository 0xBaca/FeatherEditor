#include "helpers/FileStorageHelper.hpp"
#include "helpers/TestBase.hpp"
#include "utils/Utf8Util.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace feather::utils
{
    class Utf8UtilTest : public ::testing::Test, public ::feather::test::TestBase
    {
    public:
        void SetUp() override
        {
        }

        void TearDown() override
        {
        }
    };
    /*
    TEST_F(Utf8UtilTest, correclySqeuezeUtf8String)
    {
        //before "fəˈnɛtık"
        std::u32string input;
        input.push_back(U'f');
        input.push_back(0x99C9);
        input.push_back(U'\'');
        input.push_back(U'n');
        input.push_back(0x9BC9);
        input.push_back(U't');
        input.push_back(U'i');
        input.push_back(U'k');

        //when
        auto result = feather::utils::Utf8Util::flatterUtf8String(input);

        //then
        ASSERT_EQ(result[0], 'f');
        ASSERT_EQ((char)result[1], (char)0xC9);
        ASSERT_EQ((char)result[2], (char)0x27);
        ASSERT_EQ(result[3], '\'');
        ASSERT_EQ(result[4], 'n');
        ASSERT_EQ((char)result[5], (char)0xC9);
        ASSERT_EQ((char)result[6], (char)0x9B);
        ASSERT_EQ(result[7], 't');
        ASSERT_EQ(result[8], 'i');
        ASSERT_EQ(result[9], 'k');
        ASSERT_EQ(result.size(), 10);
    }
*/
    TEST_F(Utf8UtilTest, getCharacterBytesTaken)
    {
        //before
        configuration->forceTextMode(testFile);

        //when
        //then
        ASSERT_EQ(Utf8Util::getCharacterBytesTaken<std::vector<char>>({'\n'}), 1UL);
        ASSERT_EQ(Utf8Util::getCharacterBytesTaken<std::vector<char>>({0}), 1UL);
        ASSERT_EQ(Utf8Util::getCharacterBytesTaken<std::vector<char>>({(char)0xD8}), 1UL);

        //when
        //then
        ASSERT_EQ(Utf8Util::getCharacterBytesTaken<std::vector<char>>(TWO_BYTE_UTF8_CHARACTER), 2UL);

        //when
        auto twoTwoBytes = TWO_BYTE_UTF8_CHARACTER;
        std::copy(TWO_BYTE_UTF8_CHARACTER.begin(), TWO_BYTE_UTF8_CHARACTER.end(), std::back_inserter(twoTwoBytes));

        //then
        ASSERT_EQ(Utf8Util::getCharacterBytesTaken<std::vector<char>>(twoTwoBytes), 2UL);

        //when
        auto threeBytesInvalid = THREE_BYTE_UTF8_CHARACTER;
        threeBytesInvalid.pop_back();

        //then
        ASSERT_EQ(Utf8Util::getCharacterBytesTaken<std::vector<char>>(threeBytesInvalid), 1UL);

        //when
        auto twoBytesInvalid = THREE_BYTE_UTF8_CHARACTER;
        twoBytesInvalid.insert(twoBytesInvalid.begin(), {'x'});

        //then
        ASSERT_EQ(Utf8Util::getCharacterBytesTaken<std::vector<char>>(twoBytesInvalid), 1UL);
    }

    TEST_F(Utf8UtilTest, isValidUtf8Character)
    {
        //before
        configuration->forceTextMode(testFile);

        //when
        //then
        ASSERT_TRUE(Utf8Util::isValidUtf8Character('\t'));
        ASSERT_FALSE(Utf8Util::isValidUtf8Character(0xD8));
    }

    TEST_F(Utf8UtilTest, getCharacterBytesTakenBackward)
    {
        //before
        configuration->forceTextMode(testFile);

        //when
        //then
        ASSERT_EQ(Utf8Util::getCharacterBytesTakenBackward({'\n'}), 1UL);
        ASSERT_EQ(Utf8Util::getCharacterBytesTakenBackward({0}), 1UL);
        ASSERT_EQ(Utf8Util::getCharacterBytesTakenBackward({(char)0xD8}), 1UL);
        //ASSERT_EQ(Utf8Util::getCharacterBytesTakenBackward(TWO_BYTE_UTF8_CHARACTER), 2UL);
        //ASSERT_EQ(Utf8Util::getCharacterBytesTakenBackward(FOUR_BYTE_UTF8_CHARACTER), 4UL);

        //when
        auto two = TWO_BYTE_UTF8_CHARACTER;
        two.insert(two.begin(), '\n');

        //then
        //ASSERT_EQ(Utf8Util::getCharacterBytesTakenBackward(two), 2UL);

        //when
        auto three = THREE_BYTE_UTF8_CHARACTER;
        three.insert(three.begin(), '\n');

        //then
        //ASSERT_EQ(Utf8Util::getCharacterBytesTakenBackward(three), 2UL);
    }
} // namespace feather::utils