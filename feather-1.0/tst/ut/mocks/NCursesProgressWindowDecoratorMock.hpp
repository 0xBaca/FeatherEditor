#include "printer/PrintingVisitorInterface.hpp"
#include "windows/SubWindowInterface.hpp"

#include <gmock/gmock.h>

using namespace feather::windows;

namespace testing::mocks
{
  class NCursesProgressWindowDecoratorMock : public SubWindowInterface
  {
  public:
    MOCK_CONST_METHOD0(getInternalBuffer, ScreenBuffer const *());
    MOCK_CONST_METHOD0(getLibraryWindowHandler, WindowImplInterface const *());
    MOCK_CONST_METHOD0(getParentWindowHandler, std::shared_ptr<MainWindowInterface>());
    MOCK_CONST_METHOD0(getUUID, feather::utils::datatypes::Uuid const &());
    MOCK_CONST_METHOD0(getWindowDimensions, feather::pair());
    MOCK_CONST_METHOD0(isWindowVisible, bool());
    MOCK_METHOD0(disableCursor, void());
    MOCK_METHOD0(enableCursor, void());
    MOCK_METHOD0(hide, void());
    MOCK_METHOD0(show, void());
    MOCK_METHOD1(print, void(ScreenBuffer const &));
    MOCK_METHOD2(moveWindow, void(size_t, size_t));
    MOCK_METHOD2(refreshBuffer, void(feather::printer::PrintingVisitorInterface *, std::optional<std::pair<feather::utils::windows::FramePositions, feather::pair>>));
    MOCK_METHOD4(resizeWindow, void(size_t, size_t, size_t, size_t));
  };
} // namespace testing::mocks
