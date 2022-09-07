#include "printer/HalfFrameUpVisitor.hpp"
#include "utils/datatypes/Strings.hpp"
#include "utils/helpers/Lambda.hpp"

extern feather::utils::FEATHER_MODE currentFeatherMode;

namespace feather::printer
{
    HalfFrameUpVisitor::HalfFrameUpVisitor(std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestartor)
        : printingOrchestrator(printingOrchestartor)
    {
    }

    void HalfFrameUpVisitor::visit(windows::NCursesMainWindowDecorator *window)
    {
        window->print(window->getBufferFiller()->getNewWindowBuffer(window, utils::FilledChunk::HALF_FRAME_UP, printingOrchestrator));
        window->setCursorPosition(std::make_pair(0, 0));
    }

    void HalfFrameUpVisitor::visit(windows::NCursesAgreementWindowDecorator *window, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryPosition)
    {
    }

    void HalfFrameUpVisitor::visit(windows::NCursesBottomBarWindowDecorator *window, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryPosition)
    {
        if (utils::FEATHER_MODE::WRITE_MODE == currentFeatherMode)
        {
            window->print(utils::ScreenBuffer({utils::datatypes::Strings::WRITE_MODE.begin(), utils::datatypes::Strings::WRITE_MODE.end()}, 1));
        }
        else if (utils::FEATHER_MODE::READ_MODE == currentFeatherMode)
        {
            window->print(utils::ScreenBuffer({utils::datatypes::Strings::READ_MODE.begin(), utils::datatypes::Strings::READ_MODE.end()}, 1));
        }
    }

    void HalfFrameUpVisitor::visit(windows::NCursesExitWindowDecorator *window, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryPosition)
    {
    }

    void HalfFrameUpVisitor::visit(windows::NCursesInfoWindowDecorator *window, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryPosition)
    {
        //TODO
    }

    void HalfFrameUpVisitor::visit(windows::NCursesProgressWindowDecorator *window, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryPosition)
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

    void HalfFrameUpVisitor::visit(windows::NCursesShortcutsWindowDecorator *window, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryPosition)
    {
    }

    void HalfFrameUpVisitor::visit(windows::NCursesTopWindowDecorator *window, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryposition)
    {
    }
} // namespace feather::printer
