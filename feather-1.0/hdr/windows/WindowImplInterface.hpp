#pragma once

#include "utils/ScreenBuffer.hpp"
#include "utils/Direction.hpp"
#include "utils/NCursesWrapper.hpp"
#include "utils/datatypes/Uuid.hpp"
#include "utils/windows/WindowInformationInterface.hpp"

namespace feather::windows
{
  class WindowImplInterface
  {
  public:
    virtual bool isCursorEnabled() const = 0;
    virtual bool isWindowVisible() const = 0;
    virtual int getCharacter() const = 0;
    virtual pair getCursorPosition(bool = true) const = 0;
    virtual pair getWindowDimensions() const = 0;
    virtual size_t getNoColumns() const = 0;
    virtual size_t getNoRows() const = 0;
    virtual std::pair<pair, bool> moveCursor(feather::utils::Direction, std::shared_ptr<utils::windows::WindowInformationInterface>, std::optional<size_t>, utils::ScreenBuffer const &, bool) = 0;
    virtual utils::datatypes::Uuid const &getUUID() const = 0;
    virtual void destroyDisplayedWindow() const = 0;
    virtual void disableCursor() = 0;
    virtual void drawWindowFrame() = 0;
    virtual void enableCursor() = 0;
    virtual void hide() = 0;
    virtual void moveWindow(size_t, size_t) = 0;
    virtual void print(utils::ScreenBuffer const &, bool = false) = 0;
    virtual void refreshWindow() const = 0;
    virtual void resizeWindow(size_t, size_t, size_t, size_t) = 0;
    virtual void setBlockingRead() const = 0;
    virtual void setCursorAtPos(pair, bool = true) const = 0;
    virtual void setNonBlockingRead() const = 0;
    virtual void show() = 0;
    virtual void switchWrapper(utils::NCursesWrapper *) const = 0;
    virtual ~WindowImplInterface() = 0;
  };
} // namespace feather::windows