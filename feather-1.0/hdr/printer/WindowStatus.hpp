#pragma once

#include "printer/PrintingOrchestratorInterface.hpp"
#include "windows/SubWindowInterface.hpp"

#include <string>

namespace feather::printer
{
  class WindowStatus
  {
  public:
    static char32_t INFINITY_CHARACTER;
    std::u32string getCharAbsolutePosition(feather::windows::SubWindowInterface *, std::shared_ptr<PrintingOrchestratorInterface>, std::optional<std::pair<utils::windows::FramePositions, pair>>);
    std::u32string getPrintedProgress(feather::windows::SubWindowInterface *, std::shared_ptr<PrintingOrchestratorInterface>, std::optional<std::pair<utils::windows::FramePositions, pair>>);

  private:
    std::u32string sizeToString(size_t);
  };
} // namespace feather::printer