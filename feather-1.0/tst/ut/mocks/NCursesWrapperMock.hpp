#include "utils/NCursesWrapper.hpp"
#include <gmock/gmock.h>

using namespace feather::utils;

namespace testing::mocks
{
  class NCursesWrapperMock : public NCursesWrapper
  {
  public:
    MOCK_CONST_METHOD0(getCursorPosition, std::pair<size_t, size_t>());
    MOCK_CONST_METHOD0(getMaxScreenNoRowsColumns, std::pair<size_t, size_t>());
    MOCK_METHOD1(print, void(std::pair<std::set<feather::utils::datatypes::COLOR> const, std::map<feather::pair, std::pair<std::vector<wchar_t>, bool>>> &));
    MOCK_METHOD4(resizeWindow, void(size_t, size_t, size_t, size_t));
  };
} // namespace testing::mocks