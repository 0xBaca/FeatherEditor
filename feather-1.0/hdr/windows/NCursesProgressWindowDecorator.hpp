#pragma once

#include "utils/windows/WindowInformationInterface.hpp"
#include "utils/windows/WindowInformationVisitorInterface.hpp"
#include "windows/MainWindowInterface.hpp"
#include "windows/SubWindowInterface.hpp"
#include "windows/WindowImplInterface.hpp"

namespace feather::windows
{
  class NCursesProgressWindowDecorator : public SubWindowInterface
  {
  public:
    explicit NCursesProgressWindowDecorator(std::shared_ptr<MainWindowInterface>, WindowImplInterface *);
    void print(utils::ScreenBuffer const &) override;
    void refreshBuffer(printer::PrintingVisitorInterface *, std::optional<std::pair<utils::windows::FramePositions, pair>> = std::nullopt) override;
    ~NCursesProgressWindowDecorator();
  };
} // namespace feather::windows