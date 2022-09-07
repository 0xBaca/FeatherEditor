#pragma once

#include <memory>

namespace feather::windows
{
  class MainWindowInterface;
  class NCursesMainWindowDecorator;
  class NCursesBottomBarWindowDecorator;
  class NCursesProgressWindowDecorator;
} // namespace feather::windows

namespace feather::utils::windows
{
  struct WindowInformationInterface;

  class WindowInformationVisitorInterface
  {
  public:
    virtual std::unique_ptr<WindowInformationInterface> visit(feather::windows::NCursesMainWindowDecorator *) = 0;
    virtual std::unique_ptr<WindowInformationInterface> visit(feather::windows::NCursesBottomBarWindowDecorator *) = 0;
    virtual std::unique_ptr<WindowInformationInterface> visit(feather::windows::NCursesProgressWindowDecorator *) = 0;
    virtual ~WindowInformationVisitorInterface() {}
  };
} // namespace feather::utils::windows
