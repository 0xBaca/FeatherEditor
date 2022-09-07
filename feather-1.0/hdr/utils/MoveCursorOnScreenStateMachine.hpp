#pragma once

#include "utils/ScreenBuffer.hpp"
#include "utils/Direction.hpp"
#include "utils/windows/WindowInformationInterface.hpp"

namespace feather::utils
{
  class MoveCursorOnScreenStateMachine
  {
  public:
    static bool doLineExist(std::pair<size_t, bool> const &);
    static bool hasNewLine(std::pair<size_t, bool> const &);
    static bool isCursorAtLastScreenBufferColumn(pair, std::pair<size_t, bool> const &, std::pair<size_t, bool> const &, utils::ScreenBuffer const &, bool);
    static pair getCursorNewPosition(size_t, size_t, size_t, size_t, std::shared_ptr<utils::windows::WindowInformationInterface> const, Direction const &, std::optional<size_t>, utils::ScreenBuffer const &, bool) noexcept;
    static size_t mapCursorColumnToScreenBufferColumn(pair, utils::ScreenBuffer const &, bool);
    static size_t mapCursorColumnToFirstNotLessColumn(pair, utils::ScreenBuffer const &);
    static size_t mapScreenBufferColumnToCursorColumn(size_t, size_t, utils::ScreenBuffer const &);
  };
} // namespace feather::utils