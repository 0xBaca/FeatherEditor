/* This file has to be distributed as it is. It cannot be modified as per license */
#include "windows/NCursesAgreementWindowDecorator.hpp"

namespace feather::windows
{
    NCursesAgreementWindowDecorator::NCursesAgreementWindowDecorator(std::shared_ptr<MainWindowInterface> mainWindowArg, WindowImplInterface *nCursesLibraryWindowArg)
        : SubWindowInterface(mainWindowArg, nCursesLibraryWindowArg)
    {
        lines.push_back(U" ");
        lines.push_back(U"           " + REGISTERED_TO);
        lines.push_back(U" ");
        lines.push_back(U" Mateusz Wojtczak");
        lines.push_back(U" ");
        lines.push_back(U" 0xbaca@gmail.com");
    }

    std::vector<std::u32string> const &NCursesAgreementWindowDecorator::getText() const
    {
        return lines;
    }

    void NCursesAgreementWindowDecorator::print(utils::ScreenBuffer const &newBuffer)
    {
        nCursesLibraryWindow->print(newBuffer, false);
    }

    void NCursesAgreementWindowDecorator::refreshBuffer(printer::PrintingVisitorInterface *visitor, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryPosition)
    {
        visitor->visit(this, arbitraryPosition);
        nCursesLibraryWindow->drawWindowFrame();
    }

    NCursesAgreementWindowDecorator::~NCursesAgreementWindowDecorator()
    {
    }
} // namespace feather::windows