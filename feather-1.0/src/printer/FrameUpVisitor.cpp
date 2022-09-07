#include "printer/FrameUpVisitor.hpp"
#include "utils/helpers/Lambda.hpp"

namespace feather::printer
{
    FrameUpVisitor::FrameUpVisitor(std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestartor)
        : printingOrchestrator(printingOrchestartor)
    {
    }

    void FrameUpVisitor::visit(windows::NCursesMainWindowDecorator *window)
    {
        window->print(window->getBufferFiller()->getNewWindowBuffer(window, utils::FilledChunk::FRAME_UP, printingOrchestrator));
        window->setCursorPosition(std::make_pair(0, 0));
    }

    void FrameUpVisitor::visit(windows::NCursesAgreementWindowDecorator *window, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryPosition)
    {
    }

    void FrameUpVisitor::visit(windows::NCursesBottomBarWindowDecorator *window, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryPosition)
    {
    }

    void FrameUpVisitor::visit(windows::NCursesExitWindowDecorator *window, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryPosition)
    {
    }

    void FrameUpVisitor::visit(windows::NCursesInfoWindowDecorator *window, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryPosition)
    {
    }

    void FrameUpVisitor::visit(windows::NCursesProgressWindowDecorator *window, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryPosition)
    {
        utils::ScreenBuffer newBuffer(1, 1, utils::NCursesWrapper::getScreenDimenstions().second);
        auto progress = getPrintedProgress(window, printingOrchestrator, arbitraryPosition);
        auto characterNumber = getCharAbsolutePosition(window, printingOrchestrator, arbitraryPosition);
        std::copy(progress.begin(), progress.end(), std::back_inserter(newBuffer[0]));
        newBuffer[0].push_back('%');
        newBuffer[0].push_back(',');
        newBuffer[0].push_back(' ');
        std::copy(characterNumber.begin(), characterNumber.end(), std::back_inserter(newBuffer[0]));
        window->print(std::move(newBuffer));
    }

    void FrameUpVisitor::visit(windows::NCursesShortcutsWindowDecorator *window, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryPosition)
    {
    }

    void FrameUpVisitor::visit(windows::NCursesTopWindowDecorator *window, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryposition)
    {
    }
} // namespace feather::printer
