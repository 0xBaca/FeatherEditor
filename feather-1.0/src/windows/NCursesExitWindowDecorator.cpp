#include "windows/NCursesExitWindowDecorator.hpp"

namespace feather::windows
{
    NCursesExitWindowDecorator::NCursesExitWindowDecorator(std::shared_ptr<MainWindowInterface> mainWindowArg, WindowImplInterface *nCursesLibraryWindowArg)
        : SubWindowInterface(mainWindowArg, nCursesLibraryWindowArg)
    {
    }

    std::vector<std::u32string> const &NCursesExitWindowDecorator::getText() const
    {
        return text;
    }

    void NCursesExitWindowDecorator::print(utils::ScreenBuffer const &newBuffer)
    {
        std::unordered_map<std::u32string, utils::datatypes::COLOR> highlightedStrings;
        for (auto const &e : text)
        {
            highlightedStrings.insert(std::make_pair(e, utils::datatypes::COLOR::FEATHER_COLOR_INFO_WINDOW));
        }
        highlightedStrings.insert(std::make_pair(NO_CHANGES_HAS_BEEN_MADE, utils::datatypes::COLOR::FEATHER_COLOR_INFO_WINDOW_GREEN_BACKGROUND));
        highlightedStrings.insert(std::make_pair(CHANGES_HAS_BEEN_MADE, utils::datatypes::COLOR::FEATHER_COLOR_INFO_WINDOW_RED_BACKGROUND));
        //newBuffer.applyColors(highlightedStrings);
        nCursesLibraryWindow->print(newBuffer, false);
    }

    void NCursesExitWindowDecorator::refreshBuffer(printer::PrintingVisitorInterface *visitor, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryPosition)
    {
        visitor->visit(this, arbitraryPosition);
        nCursesLibraryWindow->drawWindowFrame();
    }

    NCursesExitWindowDecorator::~NCursesExitWindowDecorator()
    {
    }

    std::vector<std::u32string> NCursesExitWindowDecorator::text = {
        U"\n",
        U"          Quit: y/n"};

    std::u32string NCursesExitWindowDecorator::CHANGES_HAS_BEEN_MADE = U"   You have unsaved changes  ";
    std::u32string NCursesExitWindowDecorator::NO_CHANGES_HAS_BEEN_MADE = U"   No changes has been made  ";
} // namespace feather::windows