#include <windows/SubWindowInterface.hpp>

namespace feather::windows
{
    SubWindowInterface::SubWindowInterface(std::shared_ptr<MainWindowInterface> parentWindowArg, WindowImplInterface *nCursesLibraryWindowArg) : parentWindow(parentWindowArg), nCursesLibraryWindow(nCursesLibraryWindowArg)
    {
    }

    SubWindowInterface::SubWindowInterface()
    {
    }

    void SubWindowInterface::enableCursor()
    {
        nCursesLibraryWindow->enableCursor();
    }

    void SubWindowInterface::disableCursor()
    {
        nCursesLibraryWindow->disableCursor();
    }

    void SubWindowInterface::hide()
    {
        nCursesLibraryWindow->hide();
    }

    bool SubWindowInterface::isWindowVisible() const
    {
        return nCursesLibraryWindow->isWindowVisible();
    }

    void SubWindowInterface::moveWindow(size_t newWindowStartPosX, size_t newWindowStartPosY)
    {
        nCursesLibraryWindow->moveWindow(newWindowStartPosX, newWindowStartPosY);
    }

    void SubWindowInterface::resizeWindow(size_t newWindowStartRow, size_t newWindowStartColumn, size_t newWindowNoRows, size_t newWindowNoColumns)
    {
        nCursesLibraryWindow->resizeWindow(newWindowStartRow, newWindowStartColumn, newWindowNoRows, newWindowNoColumns);
    }

    void SubWindowInterface::show()
    {
        nCursesLibraryWindow->show();
    }

    pair SubWindowInterface::getCursorPosition() const
    {
        return nCursesLibraryWindow->getCursorPosition(false);
    }

    WindowImplInterface const *SubWindowInterface::getLibraryWindowHandler() const
    {
        return nCursesLibraryWindow.get();
    }

    utils::datatypes::Uuid const &SubWindowInterface::getUUID() const
    {
        return nCursesLibraryWindow->getUUID();
    }

    pair SubWindowInterface::getWindowDimensions() const
    {
        return nCursesLibraryWindow->getWindowDimensions();
    }

    void SubWindowInterface::setNewParent(std::shared_ptr<MainWindowInterface> newParent)
    {
        parentWindow = newParent;
    }

    std::shared_ptr<MainWindowInterface> SubWindowInterface::getParentWindowHandler() const
    {
        return parentWindow;
    }

    void SubWindowInterface::setCursor(pair pos)
    {
        nCursesLibraryWindow->setCursorAtPos(pos, false);
    }

    SubWindowInterface::~SubWindowInterface()
    {
    }

    std::set<utils::datatypes::COLOR> SubWindowInterface::windowBackgroundColors = {utils::datatypes::COLOR::FEATHER_COLOR_WHITE, utils::datatypes::COLOR::FEATHER_COLOR_CYAN_BCKG, utils::datatypes::COLOR::FEATHER_COLOR_MAGENTA_BCKG, utils::datatypes::COLOR::FEATHER_COLOR_GREEN_BCKG};
    std::set<utils::datatypes::COLOR> SubWindowInterface::windowBackgroundLimitedColors = {utils::datatypes::COLOR::FEATHER_COLOR_GREY_BCKG};
    std::set<utils::datatypes::COLOR> SubWindowInterface::windowBackgroundLimitedColorsBlack = {utils::datatypes::COLOR::FEATHER_COLOR_BLACK, utils::datatypes::COLOR::FEATHER_COLOR_GREY_BCKG};
    std::set<utils::datatypes::COLOR> SubWindowInterface::windowBackgroundLimitedColorsRed = {utils::datatypes::COLOR::FEATHER_COLOR_RED, utils::datatypes::COLOR::FEATHER_COLOR_GREY_BCKG};
    std::set<utils::datatypes::COLOR> SubWindowInterface::windowBackgroundLimitedColorsGreen = {utils::datatypes::COLOR::FEATHER_COLOR_GREEN, utils::datatypes::COLOR::FEATHER_COLOR_GREY_BCKG};
    std::set<utils::datatypes::COLOR> SubWindowInterface::windowBackgroundLimitedColorsYellow = {utils::datatypes::COLOR::FEATHER_COLOR_YELLOW, utils::datatypes::COLOR::FEATHER_COLOR_GREY_BCKG};
    std::set<utils::datatypes::COLOR> SubWindowInterface::windowBackgroundLimitedColorsBlue = {utils::datatypes::COLOR::FEATHER_COLOR_BLUE, utils::datatypes::COLOR::FEATHER_COLOR_GREY_BCKG};
    std::set<utils::datatypes::COLOR> SubWindowInterface::windowBackgroundLimitedColorsMagenta = {utils::datatypes::COLOR::FEATHER_COLOR_MAGENTA, utils::datatypes::COLOR::FEATHER_COLOR_GREY_BCKG};
    std::set<utils::datatypes::COLOR> SubWindowInterface::windowBackgroundLimitedColorsCyan = {utils::datatypes::COLOR::FEATHER_COLOR_CYAN, utils::datatypes::COLOR::FEATHER_COLOR_GREY_BCKG};
    std::set<utils::datatypes::COLOR> SubWindowInterface::windowBackgroundLimitedColorsWhite = {utils::datatypes::COLOR::FEATHER_COLOR_WHITE, utils::datatypes::COLOR::FEATHER_COLOR_GREY_BCKG};
}