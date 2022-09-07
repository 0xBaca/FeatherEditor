#include "windows/NCursesProgressWindowDecorator.hpp"

namespace feather::windows
{
    NCursesProgressWindowDecorator::NCursesProgressWindowDecorator(std::shared_ptr<MainWindowInterface> mainWindowArg, WindowImplInterface *nCursesLibraryWindowArg)
        : SubWindowInterface(mainWindowArg, nCursesLibraryWindowArg)
    {
    }

    void NCursesProgressWindowDecorator::print(utils::ScreenBuffer const &newBuffer)
    {
        nCursesLibraryWindow->print(newBuffer, false);
    }

    void NCursesProgressWindowDecorator::refreshBuffer(printer::PrintingVisitorInterface *visitor, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryPosition)
    {
        visitor->visit(this, arbitraryPosition);
    }

    NCursesProgressWindowDecorator::~NCursesProgressWindowDecorator()
    {
    }
} // namespace feather::windows
