#pragma once

#include "windows/WindowsAbstractFactory.hpp"

#include <gmock/gmock.h>

using namespace feather::windows;

namespace testing::mocks
{
  class WindowsFactoryMock : public WindowsAbstractFactory
  {
  public:
    MOCK_METHOD0(getAgreementWindowDimensions, std::pair<feather::pair, feather::pair>());
    MOCK_METHOD0(getBottomBarWidowDimensions, std::pair<feather::pair, feather::pair>());
    MOCK_METHOD0(getExitWidowDimensions, std::pair<feather::pair, feather::pair>());
    MOCK_METHOD0(getInfoWidowDimensions, std::pair<feather::pair, feather::pair>());
    MOCK_METHOD0(getLog, std::shared_ptr<feather::utils::logger::policy::FileLogPolicy>());
    MOCK_METHOD0(getMainWindowDimensions, std::pair<feather::pair, feather::pair>());
    MOCK_METHOD0(getProgressWidowDimensions, std::pair<feather::pair, feather::pair>());
    MOCK_METHOD0(getSecondaryHexModeMainWindowDimensions, std::pair<feather::pair, feather::pair>());
    MOCK_METHOD0(getShortcutsWidowDimensions, std::pair<feather::pair, feather::pair>());
    MOCK_METHOD0(getTopWidowDimensions, std::pair<feather::pair, feather::pair>());
    MOCK_METHOD0(getUnregisteredWidowDimensions, std::pair<feather::pair, feather::pair>());
    MOCK_METHOD1(createAgreementWindow, std::unique_ptr<SubWindowInterface>(std::shared_ptr<MainWindowInterface>));
    MOCK_METHOD1(createBottomBarWindow, std::unique_ptr<SubWindowInterface>(std::shared_ptr<MainWindowInterface>));
    MOCK_METHOD1(createExitWindow, std::unique_ptr<SubWindowInterface>(std::shared_ptr<MainWindowInterface>));
    MOCK_METHOD1(createInfoWindow, std::unique_ptr<SubWindowInterface>(std::shared_ptr<MainWindowInterface>));
    MOCK_METHOD1(createProgressWindow, std::unique_ptr<SubWindowInterface>(std::shared_ptr<MainWindowInterface>));
    MOCK_METHOD1(createShortcutsWindow, std::unique_ptr<SubWindowInterface>(std::shared_ptr<MainWindowInterface>));
    MOCK_METHOD1(createTopWindow, std::unique_ptr<SubWindowInterface>(std::shared_ptr<MainWindowInterface>));
    MOCK_METHOD1(createUnregisteredWindow, std::unique_ptr<SubWindowInterface>(std::shared_ptr<MainWindowInterface>));
  };
} // namespace testing::mocks
