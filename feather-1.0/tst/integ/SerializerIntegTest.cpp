#include "helpers/TestBase.hpp"
#include "utils/ProgramOptionsParser.hpp"
#include "utils/Serializer.hpp"
#include "utils/storage/AbstractStorage.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace feather::utils
{
    class SerializerIntegTest : public ::testing::Test, public ::feather::test::TestBase
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

    TEST_F(SerializerIntegTest, serializeDeserializeChange)
    {
        // before
        std::vector<char> text = FIVE_BYTE_UTF8_CHARACTER;
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        configuration->forceTextMode(testFile);
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createEmptyStorage("searilizeChange.fea");

        // when
        Serializer::serialize(0, text, storage);
        auto result = Serializer::deserializeChange(storage);

        // then
        ASSERT_EQ(result.changes, FIVE_BYTE_UTF8_CHARACTER);
        ASSERT_THAT(result.position, 0);
        ASSERT_EQ(result.watermark, FEATHER_WATERMARK);
    }

    TEST_F(SerializerIntegTest, serializeDeserializeColors)
    {
        // before
        std::multimap<std::pair<pair, pair>, std::pair<size_t, std::set<utils::datatypes::COLOR>>> textChunksColors{{{{1, 2}, {3, 3}}, {0, {utils::datatypes::COLOR::FEATHER_COLOR_BLUE}}}, {{{1, 2}, {3, 3}}, {0, {utils::datatypes::COLOR::FEATHER_COLOR_BLUE}}}};
        std::unordered_multimap<std::u32string, std::pair<size_t, std::set<utils::datatypes::COLOR>>> textColors{{U"text", {0, {utils::datatypes::COLOR::FEATHER_COLOR_BLUE}}}};
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        configuration->forceTextMode(testFile);
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createEmptyStorage("serializeDeserializeColors.fea");

        // when
        Serializer::serialize(textChunksColors, textColors, 0, storage);
        auto result = Serializer::deserializeColors(storage);

        // then
        ASSERT_EQ(result.colorPriority, 0);
        ASSERT_THAT(result.textChunksColors, textChunksColors);
        ASSERT_EQ(result.textColors, textColors);
        ASSERT_EQ(result.watermark, FEATHER_WATERMARK);
    }

    TEST_F(SerializerIntegTest, serializeDeserializeDeletions)
    {
        // before
        std::map<pair, pair> deletions{{{1, 2}, {3, 3}}};
        const char *commandLineArguments[] = {"./feather", "file", "--relax=4", "--mode=text"};
        utils::ProgramOptionsParser::parseArguments(4, commandLineArguments);
        configuration->overrideConfiguration();
        configuration->forceTextMode(testFile);
        std::shared_ptr<feather::utils::storage::AbstractStorage> storage = createEmptyStorage("serializeDeserializeColors.fea");

        // when
        Serializer::serialize(deletions, storage);
        auto result = Serializer::deserializeDeletion(storage);

        // then
        ASSERT_EQ(result.deletions, deletions);
        ASSERT_EQ(result.watermark, FEATHER_WATERMARK);
    }
}