#pragma once

#include "utils/windows/FramePositions.hpp"

#include <optional>

namespace feather::windows
{
  class NCursesAgreementWindowDecorator;
  class NCursesMainWindowDecorator;
  class NCursesBottomBarWindowDecorator;
  class NCursesExitWindowDecorator;
  class NCursesInfoWindowDecorator;
  class NCursesProgressWindowDecorator;
  class NCursesShortcutsWindowDecorator;
  class NCursesTopWindowDecorator;
} // namespace feather::windows

namespace feather::printer
{
  class PrintingVisitorInterface
  {
  public:
    virtual void visit(windows::NCursesMainWindowDecorator *) = 0;
    virtual void visit(windows::NCursesAgreementWindowDecorator *, std::optional<std::pair<utils::windows::FramePositions, pair>> = std::nullopt) = 0;
    virtual void visit(windows::NCursesBottomBarWindowDecorator *, std::optional<std::pair<utils::windows::FramePositions, pair>> = std::nullopt) = 0;
    virtual void visit(windows::NCursesExitWindowDecorator *, std::optional<std::pair<utils::windows::FramePositions, pair>> = std::nullopt) = 0;
    virtual void visit(windows::NCursesInfoWindowDecorator *, std::optional<std::pair<utils::windows::FramePositions, pair>> = std::nullopt) = 0;
    virtual void visit(windows::NCursesProgressWindowDecorator *, std::optional<std::pair<utils::windows::FramePositions, pair>> = std::nullopt) = 0;
    virtual void visit(windows::NCursesShortcutsWindowDecorator *, std::optional<std::pair<utils::windows::FramePositions, pair>> = std::nullopt) = 0;
    virtual void visit(windows::NCursesTopWindowDecorator *, std::optional<std::pair<utils::windows::FramePositions, pair>> = std::nullopt) = 0;
    virtual ~PrintingVisitorInterface() {}
  };
} // namespace feather::printer