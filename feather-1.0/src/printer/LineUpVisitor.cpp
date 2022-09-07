#include "printer/LineUpVisitor.hpp"
#include "utils/datatypes/Strings.hpp"
#include "utils/helpers/Lambda.hpp"

extern feather::utils::FEATHER_MODE currentFeatherMode;

namespace feather::printer
{
    LineUpVisitor::LineUpVisitor(std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestartor)
        : printingOrchestrator(printingOrchestartor)
    {
    }

    void LineUpVisitor::visit(windows::NCursesMainWindowDecorator *window)
    {
        window->print(window->getBufferFiller()->getNewWindowBuffer(window, utils::FilledChunk::LINE_UP, printingOrchestrator));
    }

    void LineUpVisitor::visit(windows::NCursesAgreementWindowDecorator *window, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryPosition)
    {
    }

    void LineUpVisitor::visit(windows::NCursesBottomBarWindowDecorator *window, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryPosition)
    {
        if (utils::FEATHER_MODE::ACTION_MODE == currentFeatherMode || utils::FEATHER_MODE::ACTION_MODE_SAVE == currentFeatherMode || utils::FEATHER_MODE::SEARCH_MODE == currentFeatherMode || utils::FEATHER_MODE::HEX_SEARCH_MODE == currentFeatherMode || utils::FEATHER_MODE::ACTION_MODE_COLOR == currentFeatherMode || utils::FEATHER_MODE::ACTION_MODE_UNCOLOR == currentFeatherMode)
        {
            auto previousContent = window->getCurrentReader()->getPreviousFromHistory();
            if (previousContent.has_value())
            {
                previousContent->erase(std::remove_if(previousContent->begin(), previousContent->end(), utils::helpers::Lambda::isNewLineChar), previousContent->end());
                window->print(utils::ScreenBuffer({(*previousContent).begin(), (*previousContent).end()}, 1));
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

    void LineUpVisitor::visit(windows::NCursesExitWindowDecorator *window, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryPosition)
    {
    }

    void LineUpVisitor::visit(windows::NCursesInfoWindowDecorator *window, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryPosition)
    {
    }

    void LineUpVisitor::visit(windows::NCursesProgressWindowDecorator *window, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryPosition)
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

    void LineUpVisitor::visit(windows::NCursesShortcutsWindowDecorator *window, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryPosition)
    {
        size_t shortWindowLines = utils::NCursesWrapper::getScreenDimenstions().first - 2UL;
        utils::ScreenBuffer newBuffer(shortWindowLines);
        newBuffer.fillBufferLine(U"");
        newBuffer.fillBufferLine(U" Session ID :");
        newBuffer.fillBufferLine(U" " + utils::helpers::Lambda::mapStringToWideString(window->getParentWindowHandler()->getUUID().getString()));
        size_t currLineNumber = 0;
        window->decrementFirstLineNumber();
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

    void LineUpVisitor::visit(windows::NCursesTopWindowDecorator *window, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryposition)
    {
    }
} // namespace feather::printer
