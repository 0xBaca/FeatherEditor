#include "printer/PrintingVisitorInterface.hpp"
#include "utils/NCursesWrapper.hpp"
#include "utils/ScreenBuffer.hpp"
#include "windows/NCursesWindow.hpp"

#include <gmock/gmock.h>

using namespace feather::windows;

namespace testing::mocks
{
  class NCursesWindowMock : public WindowImplInterface
  {
    void applyNewBuffer(feather::utils::ScreenBuffer &&rawBuffer)
    {
      _applyNewBuffer(rawBuffer);
    }

  public:
    MOCK_CONST_METHOD0(destroyDisplayedWindow, void());
    MOCK_CONST_METHOD0(getCharacter, int());
    MOCK_CONST_METHOD0(getInternalBuffer, feather::utils::ScreenBuffer const *());
    MOCK_CONST_METHOD0(getLibraryWindowHandler, NCursesWindow const *());
    MOCK_CONST_METHOD0(getMaxScreenNoRowsColumns, std::pair<size_t, size_t>());
    MOCK_CONST_METHOD0(getNoColumns, size_t());
    MOCK_CONST_METHOD0(getNoRows, size_t());
    MOCK_CONST_METHOD0(getParentWindowHandler, NCursesWindow *());
    MOCK_CONST_METHOD0(getRawWindow, void *());
    MOCK_CONST_METHOD0(getUUID, feather::utils::datatypes::Uuid const &());
    MOCK_CONST_METHOD0(getWindowDimensions, std::pair<size_t, size_t>());
    MOCK_CONST_METHOD0(isCursorEnabled, bool());
    MOCK_CONST_METHOD0(isWindowVisible, bool());
    MOCK_CONST_METHOD0(refreshWindow, void());
    MOCK_CONST_METHOD0(setBlockingRead, void());
    MOCK_CONST_METHOD0(setNonBlockingRead, void());
    MOCK_CONST_METHOD1(getCharacterWidth, size_t(char32_t));
    MOCK_CONST_METHOD1(getCursorPosition, feather::pair(bool));
    MOCK_CONST_METHOD1(switchWrapper, void(feather::utils::NCursesWrapper *));
    MOCK_CONST_METHOD2(setCursorAtPos, void(feather::pair, bool));
    MOCK_METHOD0(disableCursor, void());
    MOCK_METHOD0(drawWindowFrame, void());
    MOCK_METHOD0(enableCursor, void());
    MOCK_METHOD0(hide, void());
    MOCK_METHOD0(show, void());
    MOCK_METHOD1(_applyNewBuffer, void(feather::utils::ScreenBuffer &));
    MOCK_METHOD1(refreshBuffer, void(feather::printer::PrintingVisitorInterface *));
    MOCK_METHOD1(setWindowBackgroundColor, void(feather::utils::datatypes::COLOR));
    MOCK_METHOD2(moveWindow, void(size_t, size_t));
    MOCK_METHOD2(print, void(feather::utils::ScreenBuffer const &, bool));
    MOCK_METHOD4(resizeWindow, void(size_t, size_t, size_t, size_t));
    MOCK_METHOD5(moveCursor, std::pair<feather::pair, bool>(feather::utils::Direction, std::shared_ptr<feather::utils::windows::WindowInformationInterface>, std::optional<size_t>, feather::utils::ScreenBuffer const &, bool));
  };
} // namespace testing::mocks
