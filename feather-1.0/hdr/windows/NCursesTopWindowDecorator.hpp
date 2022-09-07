#pragma once

#include "utils/windows/WindowInformationInterface.hpp"
#include "windows/MainWindowInterface.hpp"
#include "windows/SubWindowInterface.hpp"
#include "windows/WindowImplInterface.hpp"

namespace feather::windows
{
  class NCursesTopWindowDecorator : public SubWindowInterface
  {
  private:
    std::unordered_multimap<std::u32string, std::pair<size_t, std::set<utils::datatypes::COLOR>>> highlightedStrings;

  public:
    explicit NCursesTopWindowDecorator(std::shared_ptr<MainWindowInterface>, WindowImplInterface *);
    void print(utils::ScreenBuffer const &) override;
    void refreshBuffer(printer::PrintingVisitorInterface *, std::optional<std::pair<utils::windows::FramePositions, pair>> = std::nullopt) override;
    ~NCursesTopWindowDecorator();
  };
} // namespace feather::windows