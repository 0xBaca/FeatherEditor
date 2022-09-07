#include "utils/FeatherMode.hpp"
#include "utils/datatypes/Strings.hpp"
#include "windows/NCursesShortcutsWindowDecorator.hpp"

extern feather::utils::FEATHER_MODE currentFeatherMode;

namespace feather::windows
{
    NCursesShortcutsWindowDecorator::NCursesShortcutsWindowDecorator(std::shared_ptr<MainWindowInterface> mainWindowArg, WindowImplInterface *nCursesLibraryWindowArg)
        : SubWindowInterface(mainWindowArg, nCursesLibraryWindowArg), firstLineNumber(0)
    {
    }

    std::vector<std::u32string> const &NCursesShortcutsWindowDecorator::getShortcuts() const
    {
        return (utils::FEATHER_MODE::SHORTCUTS_READ_MODE == currentFeatherMode) ? readModeShortcuts : writeModeShortcuts;
    }

    size_t NCursesShortcutsWindowDecorator::getFirstLineNumber() const
    {
        return firstLineNumber;
    }

    void NCursesShortcutsWindowDecorator::decrementFirstLineNumber()
    {
        firstLineNumber = firstLineNumber ? (firstLineNumber - 1) : firstLineNumber;
    }

    void NCursesShortcutsWindowDecorator::incrementFirstLineNumber()
    {
        if (getShortcuts().size() -  firstLineNumber > (getWindowDimensions().first - 4UL))
        {
            ++firstLineNumber;
        }
    }

    void NCursesShortcutsWindowDecorator::resetFirstLineNumber()
    {
        firstLineNumber = 0;
    }

    void NCursesShortcutsWindowDecorator::print(utils::ScreenBuffer const &newBuffer)
    {
        std::unordered_map<std::u32string, utils::datatypes::COLOR> highlightedStrings;
        auto const &shortcuts = (utils::FEATHER_MODE::READ_MODE == currentFeatherMode) ? readModeShortcuts : writeModeShortcuts;
        for (auto const &e : shortcuts)
        {
            highlightedStrings.insert(std::make_pair(e, utils::datatypes::COLOR::FEATHER_COLOR_INFO_WINDOW));
        }
        highlightedStrings[utils::datatypes::Strings::EDIT_STRING] = utils::datatypes::COLOR::FEATHER_COLOR_INFO_WINDOW_GREEN_BACKGROUND;
        highlightedStrings[utils::datatypes::Strings::MOVE_STRING] = utils::datatypes::COLOR::FEATHER_COLOR_INFO_WINDOW_GREEN_BACKGROUND;
        highlightedStrings[utils::datatypes::Strings::REMOVE_STRING] = utils::datatypes::COLOR::FEATHER_COLOR_INFO_WINDOW_GREEN_BACKGROUND;
        highlightedStrings[utils::datatypes::Strings::OTHERS_STRING] = utils::datatypes::COLOR::FEATHER_COLOR_INFO_WINDOW_GREEN_BACKGROUND;
        //newBuffer.applyColors(highlightedStrings);
        nCursesLibraryWindow->print(newBuffer, false);
    }

    void NCursesShortcutsWindowDecorator::refreshBuffer(printer::PrintingVisitorInterface *visitor, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryPosition)
    {
        visitor->visit(this, arbitraryPosition);
        nCursesLibraryWindow->drawWindowFrame();
    }

    NCursesShortcutsWindowDecorator::~NCursesShortcutsWindowDecorator()
    {
    }

    std::vector<std::u32string> NCursesShortcutsWindowDecorator::readModeShortcuts = {
        U" ",
        utils::datatypes::Strings::EDIT_STRING,
        U" O         at line above",
        U" o         at line below",
        U" A         at line end",
        U" a         at next char",
        U" ",
        utils::datatypes::Strings::MOVE_STRING,
        U" g         file begining",
        U" G         file end",
        U" 0         line begining",
        U" $         line end",
        U" w         next word",
        U" b         prev word",
        U" ctrl(u)   half page up",
        U" ctrl(d)   half page down",
        U" ",
        utils::datatypes::Strings::REMOVE_STRING,
        U" x         character",
        U" dw        word forward",
        U" ctrl(w)   word backward",
        U" dd        line",
        U" d(1-9)    multiple lines",
        U" D         until line end",
        U" dg        until file begin",
        U" dG        until file end",
        U" d[{(<     between brackets",
        U" ds        between white chars",
        U" ",
        utils::datatypes::Strings::OTHERS_STRING,
        U" ctrl(b)   add breakpoint",
        U" ctrl(n)   next breakpoint",
        U" ctrl(p)   prev breakpoint",
        U" 8         prev window",
        U" 9         next window",
        U" p         paste",
        U" v         select text",
        U" hh        color line",
        U" h[{(<     color between brackets",
        U" zz        screen to middle",
        U" .         repeat",
        U" ({[<>]})  jump to bracket"};

    std::vector<std::u32string> NCursesShortcutsWindowDecorator::writeModeShortcuts = {
        U" ",
        utils::datatypes::Strings::MOVE_STRING,
        U" ctrl(u)   half page up",
        U" ctrl(d)   half page down",
        U" ",
        utils::datatypes::Strings::REMOVE_STRING,
        U" ctrl(w)   word backward",
        U" ",
        utils::datatypes::Strings::OTHERS_STRING,
        U" ctrl(b)   add breakpoint",
        U" ctrl(n)   next breakpoint",
        U" ctrl(p)   prev breakpoint"};
} // namespace feather::windows
