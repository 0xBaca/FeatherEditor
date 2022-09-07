#include "mocks/FileLogPolicyMock.hpp"
#include "utils/NCursesWrapper.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace feather::utils
{
  class NCursesWrapperTest : public ::testing::Test
  {
  protected:
    std::shared_ptr<::testing::mocks::FileLogPolicyMock> fileLogPolicyMock;
    NCursesWrapper *nCursesWrapper;

  public:
    void SetUp() override
    {
      //nCursesWrapper = new NCursesWrapper(10, 10, 0, 0, fileLogPolicyMock);
    }

    void TearDown() override
    {
      delete nCursesWrapper;
    }
  };
  /*
TEST_F(NCursesWrapperTest, getProperCharacterWidth)
{
  //before
  //when
  int w1 = nCursesWrapper->getCharacterWidth('\t');
  int w2 = nCursesWrapper->getCharacterWidth(30);
  int w3 = nCursesWrapper->getCharacterWidth(0x9A989FF0);

  //then
  ASSERT_EQ(w1, 8);
  ASSERT_EQ(w2, 2);
  ASSERT_EQ(w3, 2);
}
*/
} // namespace feather::utils
