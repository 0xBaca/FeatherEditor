#pragma once

#include "windows/WindowsManager.hpp"

#include <gmock/gmock.h>

using namespace feather::windows;

namespace testing::mocks
{
    class WindowsManagerMock : public WindowsManager
    {
        void refreshAllWindows(std::unique_ptr<feather::printer::PrintingVisitorInterface> &&rVal, std::set<feather::utils::datatypes::Uuid> uuidsToIgnore)
        {
            refreshAllWindows(rVal, uuidsToIgnore);
        }

    public:
        MOCK_METHOD1(getMainWindow, std::shared_ptr<MainWindowInterface>(feather::utils::datatypes::Uuid const &));
        MOCK_METHOD2(refreshAllWindows, void(std::unique_ptr<feather::printer::PrintingVisitorInterface> &, std::set<feather::utils::datatypes::Uuid>));
        MOCK_METHOD3(refreshProgresWindow, void(std::unique_ptr<feather::printer::PrintingVisitorInterface> &, feather::utils::datatypes::Uuid const &, std::optional<std::pair<feather::utils::windows::FramePositions, feather::pair>>));
    };
} // namespace testing::mocks