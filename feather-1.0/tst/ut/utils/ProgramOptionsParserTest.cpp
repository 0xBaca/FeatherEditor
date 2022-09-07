#include "helpers/TestBase.hpp"
#include "utils/ProgramOptionsParser.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace feather::utils;

class ProgramOptionsParserTest : public ::testing::Test, public ::feather::test::TestBase
{
public:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(ProgramOptionsParserTest, testWindowUUIDNoParam)
{
    // before
    int argc = 2;
    const char *argv[] = {"./feather", "test"};
    int result = ProgramOptionsParser::parseArguments(argc, argv);

    // when
    auto windowUUID = ProgramOptionsParser::getExistingWindowUUID();

    // then
    ASSERT_EQ(result, 0);
    ASSERT_EQ(windowUUID, std::nullopt);
}

TEST_F(ProgramOptionsParserTest, testSessionIdParamEmpty)
{
    // before
    int argc = 3;
    const char *argv[] = {"./feather", "test", "--sessionId="};
    int result = ProgramOptionsParser::parseArguments(argc, argv);

    // when
    auto windowUUID = ProgramOptionsParser::getExistingWindowUUID();

    // then
    ASSERT_EQ(result, 0);
    ASSERT_EQ(windowUUID, std::string());
}

TEST_F(ProgramOptionsParserTest, testSessionIdParamValid)
{
    // before
    int argc = 3;
    const char *argv[] = {"./feather", "test", "--sessionId=220097a9-67fc-4110-b92e-c60227359c68"};
    int result = ProgramOptionsParser::parseArguments(argc, argv);

    // when
    auto windowUUID = ProgramOptionsParser::getExistingWindowUUID();

    // then
    ASSERT_EQ(result, 0);
    ASSERT_NE(windowUUID, std::nullopt);
    ASSERT_STREQ("220097a9-67fc-4110-b92e-c60227359c68", windowUUID->getString().c_str());
}

TEST_F(ProgramOptionsParserTest, testInvalidParam)
{
    // before
    int argc = 3;
    const char *argv[] = {"./feather", "test", "--moe=binary"};

    // when
    // then
    ASSERT_EQ(ProgramOptionsParser::parseArguments(argc, argv), -1);
}

TEST_F(ProgramOptionsParserTest, testHexModeParam)
{
    // before
    int argc = 3;
    const char *argv[] = {"./feather", "test", "--mode=hex"};
    int result = ProgramOptionsParser::parseArguments(argc, argv);

    // when
    auto binaryMode = ProgramOptionsParser::isHex();

    // then
    ASSERT_EQ(result, 0);
    ASSERT_TRUE(binaryMode);
}

TEST_F(ProgramOptionsParserTest, fileNameIsNotProvided)
{
    // before
    int argc = 1;
    const char *argv[] = {"./feather"};

    // when
    int result = ProgramOptionsParser::parseArguments(argc, argv);

    // then
    ASSERT_EQ(result, -1);
}

TEST_F(ProgramOptionsParserTest, unrecognizedArgument)
{
    // before
    int argc = 1;
    const char *argv[] = {"./feather", "file", "--bin"};

    // when
    int result = ProgramOptionsParser::parseArguments(argc, argv);

    // then
    ASSERT_EQ(result, -1);
}