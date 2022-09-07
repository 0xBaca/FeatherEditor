#include "printer/PrintingOrchestrator.hpp"
#include "printer/PrintingVisitorInterface.hpp"
#include "windows/NCursesMainWindowDecorator.hpp"

#include <gmock/gmock.h>

using namespace feather::windows;

namespace testing::mocks
{
  class NCursesMainWindowDecoratorMock : public MainWindowInterface
  {
  public:
    MOCK_CONST_METHOD0(getBufferFiller, std::unique_ptr<feather::utils::BufferFillerInterface> const &());
    MOCK_CONST_METHOD0(getCursorPosition, feather::pair());
    MOCK_CONST_METHOD0(getFrameBeginingPos, size_t());
    MOCK_CONST_METHOD0(getFramePosition, feather::utils::windows::FramePositions const &());
    MOCK_CONST_METHOD0(getInternalBuffer, feather::utils::ScreenBuffer const *());
    MOCK_CONST_METHOD0(getLastRealPosition, feather::pair());
    MOCK_CONST_METHOD0(getLibraryWindowHandler, WindowImplInterface const *());
    MOCK_CONST_METHOD0(getParentWindowHandler, std::unique_ptr<NCursesMainWindowDecorator> &());
    MOCK_CONST_METHOD0(getState, feather::utils::windows::WindowStateContainer());
    MOCK_CONST_METHOD0(getStorage, std::shared_ptr<feather::utils::storage::AbstractStorage> const &());
    MOCK_CONST_METHOD0(getWindowDimensions, feather::pair());
    MOCK_CONST_METHOD0(isSecondaryHexModeMainWindow, bool());
    MOCK_CONST_METHOD1(getCursorRealPosition, feather::pair(feather::printer::PrintingOrchestratorInterface *));
    MOCK_CONST_METHOD1(getUUID, feather::utils::datatypes::Uuid const &(bool));
    MOCK_METHOD0(disableCursor, void());
    MOCK_METHOD0(enableCursor, void());
    MOCK_METHOD0(hide, void());
    MOCK_METHOD0(show, void());
    MOCK_METHOD1(accept, std::unique_ptr<feather::utils::windows::WindowInformationInterface>(std::shared_ptr<feather::utils::windows::WindowInformationVisitorInterface>));
    MOCK_METHOD1(print, void(feather::utils::ScreenBuffer const &));
    MOCK_METHOD1(refreshBuffer, void(feather::printer::PrintingVisitorInterface *));
    MOCK_METHOD1(setCursorPosition, void(feather::pair));
    MOCK_METHOD1(setFramePosition, void(feather::pair));
    MOCK_METHOD1(setState, void(feather::utils::windows::WindowStateContainer const &));
    MOCK_METHOD2(showSecondaryWindowCursor, void(std::pair<feather::pair, feather::pair> const &, std::shared_ptr<feather::printer::PrintingOrchestratorInterface>));
    MOCK_METHOD3(moveCursor, void(feather::utils::Direction, std::shared_ptr<feather::utils::windows::WindowInformationVisitorInterface>, std::optional<size_t>));
    MOCK_METHOD3(setCursorFromRealPosition, void(feather::pair, std::shared_ptr<feather::printer::PrintingOrchestratorInterface>, bool));
    MOCK_METHOD4(resizeWindow, void(size_t, size_t, size_t, size_t));
    MOCK_METHOD4(setCursorFromRealPosition, void(feather::pair, std::shared_ptr<feather::printer::PrintingOrchestratorInterface>, std::unique_ptr<feather::printer::PrintingVisitorInterface> &, bool));
  };
} // namespace testing::mocks
