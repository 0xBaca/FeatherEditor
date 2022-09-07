#pragma once

#include "windows/MainWindowInterface.hpp"

namespace feather::windows
{
    class SubWindowInterface
    {
    protected:
        const size_t MIN_WINDOW_WIDTH = 22ULL;
        const size_t MIN_WINDOW_HEIGHT = 10ULL;
        std::shared_ptr<MainWindowInterface> parentWindow;
        std::unique_ptr<WindowImplInterface> nCursesLibraryWindow;
        static std::set<utils::datatypes::COLOR> windowBackgroundColors;
        static std::set<utils::datatypes::COLOR> windowBackgroundLimitedColors;
        static std::set<utils::datatypes::COLOR> windowBackgroundLimitedColorsBlack;
        static std::set<utils::datatypes::COLOR> windowBackgroundLimitedColorsRed;
        static std::set<utils::datatypes::COLOR> windowBackgroundLimitedColorsGreen;
        static std::set<utils::datatypes::COLOR> windowBackgroundLimitedColorsYellow;
        static std::set<utils::datatypes::COLOR> windowBackgroundLimitedColorsBlue;
        static std::set<utils::datatypes::COLOR> windowBackgroundLimitedColorsMagenta;
        static std::set<utils::datatypes::COLOR> windowBackgroundLimitedColorsCyan;
        static std::set<utils::datatypes::COLOR> windowBackgroundLimitedColorsWhite;
        SubWindowInterface(std::shared_ptr<MainWindowInterface>, WindowImplInterface *);
        SubWindowInterface();

    public:
        virtual void disableCursor();
        virtual void enableCursor();
        virtual void hide();
        virtual bool isWindowVisible() const;
        virtual void print(utils::ScreenBuffer const &) = 0;
        virtual void refreshBuffer(feather::printer::PrintingVisitorInterface *, std::optional<std::pair<utils::windows::FramePositions, pair>> = std::nullopt) = 0;
        virtual void resizeWindow(size_t, size_t, size_t, size_t);
        virtual void moveWindow(size_t, size_t);
        virtual void show();
        virtual WindowImplInterface const *getLibraryWindowHandler() const;
        virtual utils::datatypes::Uuid const &getUUID() const;
        virtual pair getWindowDimensions() const;
        virtual pair getCursorPosition() const;
        virtual void setCursor(pair);
        virtual void setNewParent(std::shared_ptr<MainWindowInterface> newParent);
        virtual std::shared_ptr<MainWindowInterface> getParentWindowHandler() const;
        virtual ~SubWindowInterface();
    };
} // namespace feather::windows