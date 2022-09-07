#pragma once

#include "utils/windows/WindowInformationVisitorInterface.hpp"
#include "windows/WindowImplInterface.hpp"

namespace feather::windows
{
  class NCursesWindow : public WindowImplInterface
  {
  public:
    NCursesWindow();
    explicit NCursesWindow(utils::NCursesWrapper *nCursesWrapper);
    explicit NCursesWindow(utils::NCursesWrapper *nCursesWrapper, std::optional<utils::datatypes::Uuid>);
    void destroyDisplayedWindow() const override;
    void disableCursor() override;
    void drawWindowFrame() override;
    void enableCursor() override;
    void hide() override;
    std::pair<pair, bool> moveCursor(feather::utils::Direction, std::shared_ptr<utils::windows::WindowInformationInterface>, std::optional<size_t>, utils::ScreenBuffer const &, bool) override;
    pair getCursorPosition(bool = true) const override;
    pair getWindowDimensions() const override;
    utils::datatypes::Uuid const &getUUID() const override;
    bool isCursorEnabled() const override;
    bool isWindowVisible() const override;
    void print(utils::ScreenBuffer const &, bool = false) override;
    void resizeWindow(size_t, size_t, size_t, size_t) override;
    void moveWindow(size_t, size_t) override;
    void refreshWindow() const override;
    void setBlockingRead() const override;
    void setNonBlockingRead() const override;
    void setCursorAtPos(pair, bool = true) const override;
    void show() override;
    void switchWrapper(utils::NCursesWrapper *) const override;
    size_t getNoRows() const override;
    size_t getNoColumns() const override;
    int getCharacter() const override;
    virtual ~NCursesWindow();

  private:
    bool cursorIsEnabled;
    bool windowIsVisible;
    mutable pair cursorPosition;
    mutable std::unique_ptr<utils::NCursesWrapper> ncurses;
    utils::datatypes::Uuid uuid;
  };
} // namespace feather::windows