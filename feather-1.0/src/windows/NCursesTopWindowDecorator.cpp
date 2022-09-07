#include "windows/NCursesTopWindowDecorator.hpp"

namespace feather::windows
{
    NCursesTopWindowDecorator::NCursesTopWindowDecorator(std::shared_ptr<MainWindowInterface> mainWindowArg, WindowImplInterface *nCursesLibraryWindowArg)
        : SubWindowInterface(mainWindowArg, nCursesLibraryWindowArg)
    {
        highlightedStrings.insert(std::make_pair(U" ", std::make_pair(0, utils::NCursesWrapper::hasTerminalLimitedColorsCapabilities() ? windowBackgroundLimitedColors : windowBackgroundColors)));
    }

    void NCursesTopWindowDecorator::print(utils::ScreenBuffer const &newBuffer)
    {
        std::u32string fileName(newBuffer[0].begin(), newBuffer[0].end());
        if (!highlightedStrings.count(fileName))
        {
            highlightedStrings.insert(std::make_pair(fileName, std::make_pair(0, utils::NCursesWrapper::hasTerminalLimitedColorsCapabilities() ? windowBackgroundLimitedColors : windowBackgroundColors)));
        }
        newBuffer.applyColors(highlightedStrings);
        nCursesLibraryWindow->print(newBuffer, false);
    }

    void NCursesTopWindowDecorator::refreshBuffer(printer::PrintingVisitorInterface *visitor, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryPosition)
    {
        visitor->visit(this, arbitraryPosition);
    }

    NCursesTopWindowDecorator::~NCursesTopWindowDecorator()
    {
    }
} // namespace feather::windows
