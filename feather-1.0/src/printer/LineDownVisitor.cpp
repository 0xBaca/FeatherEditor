#include "printer/LineDownVisitor.hpp"
#include "utils/datatypes/Strings.hpp"
#include "utils/helpers/Lambda.hpp"

extern feather::utils::FEATHER_MODE currentFeatherMode;

namespace feather::printer
{
    LineDownVisitor::LineDownVisitor(std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestartor)
        : printingOrchestrator(printingOrchestartor)
    {
    }

    void LineDownVisitor::visit(windows::NCursesMainWindowDecorator *window)
    {
        window->print(window->getBufferFiller()->getNewWindowBuffer(window, utils::FilledChunk::LINE_DOWN, printingOrchestrator));
    }

    void LineDownVisitor::visit(windows::NCursesAgreementWindowDecorator *window, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryPosition)
    {
    }

    void LineDownVisitor::visit(windows::NCursesBottomBarWindowDecorator *window, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryPosition)
    {
        if (utils::FEATHER_MODE::ACTION_MODE == currentFeatherMode || utils::FEATHER_MODE::ACTION_MODE_SAVE == currentFeatherMode || utils::FEATHER_MODE::SEARCH_MODE == currentFeatherMode || utils::FEATHER_MODE::HEX_SEARCH_MODE == currentFeatherMode || utils::FEATHER_MODE::ACTION_MODE_COLOR == currentFeatherMode || utils::FEATHER_MODE::ACTION_MODE_UNCOLOR == currentFeatherMode)
        {
            auto nextContent = window->getCurrentReader()->getNextFromHistory();
            if (nextContent)
            {
                window->print(utils::ScreenBuffer({(*nextContent).cbegin(), (*nextContent).cend()}, 1));
            }
        }
        else if (utils::FEATHER_MODE::WRITE_MODE == currentFeatherMode)
        {
            window->print(utils::ScreenBuffer({utils::datatypes::Strings::WRITE_MODE.begin(), utils::datatypes::Strings::WRITE_MODE.end()}, 1));
        }
        else if (utils::FEATHER_MODE::READ_MODE == currentFeatherMode)
        {
            window->print(utils::ScreenBuffer({utils::datatypes::Strings::READ_MODE.begin(), utils::datatypes::Strings::READ_MODE.end()}, 1));
        }
    }

    void LineDownVisitor::visit(windows::NCursesExitWindowDecorator *window, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryPosition)
    {
    }

    void LineDownVisitor::visit(windows::NCursesInfoWindowDecorator *window, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryPosition)
    {
    }
    void LineDownVisitor::visit(windows::NCursesProgressWindowDecorator *window, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryPosition)
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

    void LineDownVisitor::visit(windows::NCursesShortcutsWindowDecorator *window, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryPosition)
    {
        size_t shortWindowLines = utils::NCursesWrapper::getScreenDimenstions().first - 2UL;
        utils::ScreenBuffer newBuffer(shortWindowLines);
        newBuffer.fillBufferLine(U"");
        newBuffer.fillBufferLine(U" Session ID :");
        newBuffer.fillBufferLine(U" " + utils::helpers::Lambda::mapStringToWideString(window->getParentWindowHandler()->getUUID().getString()));
        size_t currLineNumber = 0;
        window->incrementFirstLineNumber();
        for (auto const &e : window->getShortcuts())
        {
            if (currLineNumber < window->getFirstLineNumber())
            {
                ++currLineNumber;
                continue;
            }
            newBuffer.fillBufferLine(e);
            if (newBuffer.getFilledRows() == shortWindowLines)
            {
                break;
            }
        }
        window->print(std::move(newBuffer));
    }

    void LineDownVisitor::visit(windows::NCursesTopWindowDecorator *window, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryposition)
    {
    }
} // namespace feather::printer
