#pragma once

#include "printer/PrintingVisitorInterface.hpp"

#include <gmock/gmock.h>

using namespace feather::printer;

namespace testing::mocks
{
    class PrintingVisitorMock : public PrintingVisitorInterface
    {
    public:
        MOCK_METHOD1(visit, void(feather::windows::NCursesMainWindowDecorator *));
        MOCK_METHOD2(visit, void(feather::windows::NCursesAgreementWindowDecorator*, std::optional<std::pair<feather::utils::windows::FramePositions, feather::pair>>));
        MOCK_METHOD2(visit, void(feather::windows::NCursesBottomBarWindowDecorator *, std::optional<std::pair<feather::utils::windows::FramePositions, feather::pair>>));
        MOCK_METHOD2(visit, void(feather::windows::NCursesExitWindowDecorator *, std::optional<std::pair<feather::utils::windows::FramePositions, feather::pair>>));
        MOCK_METHOD2(visit, void(feather::windows::NCursesInfoWindowDecorator *, std::optional<std::pair<feather::utils::windows::FramePositions, feather::pair>>));
        MOCK_METHOD2(visit, void(feather::windows::NCursesProgressWindowDecorator *, std::optional<std::pair<feather::utils::windows::FramePositions, feather::pair>>));
        MOCK_METHOD2(visit, void(feather::windows::NCursesShortcutsWindowDecorator *, std::optional<std::pair<feather::utils::windows::FramePositions, feather::pair>>));
        MOCK_METHOD2(visit, void(feather::windows::NCursesTopWindowDecorator *, std::optional<std::pair<feather::utils::windows::FramePositions, feather::pair>>));
    };
} // namespace testing::mocks