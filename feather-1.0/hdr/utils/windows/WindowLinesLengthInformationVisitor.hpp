#pragma once

#include "printer/PrintingOrchestrator.hpp"
#include "windows/NCursesMainWindowDecorator.hpp"
#include "windows/NCursesBottomBarWindowDecorator.hpp"
#include "windows/NCursesProgressWindowDecorator.hpp"

namespace feather::utils::windows
{
  class WindowLinesLengthInformationVisitor : public WindowInformationVisitorInterface
  {
  public:
    WindowLinesLengthInformationVisitor();
    std::unique_ptr<WindowInformationInterface> visit(feather::windows::NCursesMainWindowDecorator *) override;
    std::unique_ptr<WindowInformationInterface> visit(feather::windows::NCursesBottomBarWindowDecorator *) override;
    std::unique_ptr<WindowInformationInterface> visit(feather::windows::NCursesProgressWindowDecorator *) override;

  private:
    inline size_t getLineDownLen(size_t currY, feather::windows::NCursesMainWindowDecorator *window, ScreenBuffer const &buffer)
    {
      //Last line in window
      if (currY == window->getWindowDimensions().first - 1 || buffer[currY + 1].empty())
      {
        return 0;
      }
      return buffer[currY + 1].size();
    }
  };
} // namespace feather::utils::windows
