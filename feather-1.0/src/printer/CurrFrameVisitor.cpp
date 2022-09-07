#include "printer/CurrFrameVisitor.hpp"
#include "utils/datatypes/Strings.hpp"
#include "utils/helpers/Lambda.hpp"

extern feather::utils::FEATHER_MODE currentFeatherMode;

namespace feather::printer
{
    CurrFrameVisitor::CurrFrameVisitor(std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestartor)
        : printingOrchestrator(printingOrchestartor)
    {
    }

    void CurrFrameVisitor::visit(windows::NCursesMainWindowDecorator *window)
    {
        window->print(window->getBufferFiller()->getNewWindowBuffer(window, utils::FilledChunk::CURR_FRAME, printingOrchestrator));
    }

    void CurrFrameVisitor::visit(windows::NCursesAgreementWindowDecorator *window, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryPosition)
    {
        size_t shortWindowLines = utils::NCursesWrapper::getScreenDimenstions().first - 2UL;
        utils::ScreenBuffer newBuffer(shortWindowLines);
        for (auto e : window->getText())
        {
            newBuffer.fillBufferLine(e);
            if (newBuffer.getFilledRows() == shortWindowLines)
            {
                break;
            }
        }
        window->print(std::move(newBuffer));
    }

    void CurrFrameVisitor::visit(windows::NCursesBottomBarWindowDecorator *window, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryPosition)
    {
        if (utils::FEATHER_MODE::ACTION_MODE == currentFeatherMode || utils::FEATHER_MODE::SEARCH_MODE == currentFeatherMode || utils::FEATHER_MODE::HEX_SEARCH_MODE == currentFeatherMode || currentFeatherMode == utils::FEATHER_MODE::ACTION_MODE_SAVE || currentFeatherMode == utils::FEATHER_MODE::ACTION_MODE_COLOR || currentFeatherMode == utils::FEATHER_MODE::ACTION_MODE_UNCOLOR)
        {
            auto currContent = window->getCurrentReader()->getCurrFromHistory();
            if (currContent.has_value())
            {
                window->print(utils::ScreenBuffer({(*currContent).begin(), (*currContent).end()}, 1));
            }
        }
        else if (utils::FEATHER_MODE::BROWSE_SEARCH_RESULT_MODE == currentFeatherMode)
        {
            window->print(utils::ScreenBuffer({utils::datatypes::Strings::BROWSE_SEARCH_MODE.begin(), utils::datatypes::Strings::BROWSE_SEARCH_MODE.end()}, 1));
        }
        else if (utils::FEATHER_MODE::BROWSE_SEARCH_RESULT_DOWN_MODE == currentFeatherMode)
        {
            window->print(utils::ScreenBuffer({utils::datatypes::Strings::BROWSE_SEARCH_DOWN_MODE.begin(), utils::datatypes::Strings::BROWSE_SEARCH_DOWN_MODE.end()}, 1));
        }
        else if (utils::FEATHER_MODE::BROWSE_SEARCH_RESULT_UP_MODE == currentFeatherMode)
        {
            window->print(utils::ScreenBuffer({utils::datatypes::Strings::BROWSE_SEARCH_UP_MODE.begin(), utils::datatypes::Strings::BROWSE_SEARCH_UP_MODE.end()}, 1));
        }
        else if (utils::FEATHER_MODE::READ_MODE_WAITING_FOR_INPUT == currentFeatherMode || utils::FEATHER_MODE::READ_MODE == currentFeatherMode)
        {
            window->print(utils::ScreenBuffer({utils::datatypes::Strings::READ_MODE.begin(), utils::datatypes::Strings::READ_MODE.end()}, 1));
        }
        else if (utils::FEATHER_MODE::WRITE_MODE == currentFeatherMode)
        {
            window->print(utils::ScreenBuffer({utils::datatypes::Strings::WRITE_MODE.begin(), utils::datatypes::Strings::WRITE_MODE.end()}, 1));
        }
        else if (utils::FEATHER_MODE::SEARCHING_IN_PROGRESS_DOWN == currentFeatherMode)
        {
            window->print(utils::ScreenBuffer({utils::datatypes::Strings::SEARCHING_DOWN.begin(), utils::datatypes::Strings::SEARCHING_DOWN.end()}, 1));
        }
        else if (utils::FEATHER_MODE::SEARCHING_IN_PROGRESS_UP == currentFeatherMode)
        {
            window->print(utils::ScreenBuffer({utils::datatypes::Strings::SEARCHING_UP.begin(), utils::datatypes::Strings::SEARCHING_UP.end()}, 1));
        }
        else if (utils::FEATHER_MODE::WORKING_MODE == currentFeatherMode)
        {
            window->print(utils::ScreenBuffer({utils::datatypes::Strings::WORKING_STRING.begin(), utils::datatypes::Strings::WORKING_STRING.end()}, 1UL));
        }
    }

    void CurrFrameVisitor::visit(windows::NCursesExitWindowDecorator *window, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryPosition)
    {
        utils::ScreenBuffer newBuffer(30);
        newBuffer.fillBufferLine(U"");
        if (printingOrchestrator->isAnyChangesMade(window->getParentWindowHandler()->getUUID()))
        {
            newBuffer.fillBufferLine(windows::NCursesExitWindowDecorator::CHANGES_HAS_BEEN_MADE);
        }
        else
        {
            newBuffer.fillBufferLine(windows::NCursesExitWindowDecorator::NO_CHANGES_HAS_BEEN_MADE);
        }

        for (auto e : window->getText())
        {
            newBuffer.fillBufferLine(e);
        }
        window->print(std::move(newBuffer));
    }

    void CurrFrameVisitor::visit(windows::NCursesInfoWindowDecorator *window, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryPosition)
    {
        size_t maxBufferLength = window->getWindowDimensions().second;
        utils::ScreenBuffer newBuffer(1UL, 1UL, maxBufferLength);
        auto const &helpCommands = window->getHelpCommands();
        size_t charactersNeeded = std::accumulate(helpCommands.begin(), helpCommands.end(), 0, [](size_t val, std::u32string const &s)
                                                  { return val + s.size(); });
        size_t spaceBetweenCommands = ((maxBufferLength > charactersNeeded) && helpCommands.size() > 1UL) ? ((maxBufferLength - charactersNeeded) / helpCommands.size()) : 1UL;
        spaceBetweenCommands = (0 == spaceBetweenCommands) ? 1UL : spaceBetweenCommands;
        for (auto &e : helpCommands)
        {
            if (newBuffer[0].size() + e.size() >= maxBufferLength)
            {
                break;
            }
            std::copy(e.begin(), e.end(), std::back_inserter(newBuffer[0]));
            for (size_t s = 0UL; s < spaceBetweenCommands; ++s)
            {
                newBuffer[0].push_back(U' ');
            }
        }
        if (newBuffer[0].size() >= maxBufferLength)
        {
            utils::ScreenBuffer tailoredBuffer({newBuffer[0].begin(), std::next(newBuffer[0].begin(), maxBufferLength)}, 1);
            window->print(tailoredBuffer);
            return;
        }
        bool frontBackSwith = false;
        while (newBuffer[0].size() < maxBufferLength)
        {
            if (frontBackSwith)
            {
                newBuffer[0].push_back(U' ');
            }
            else
            {
                newBuffer[0].insert(newBuffer[0].begin(), U' ');
            }
            frontBackSwith ^= true;
        }
        window->print(newBuffer);
    }

    void CurrFrameVisitor::visit(windows::NCursesProgressWindowDecorator *window, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryPosition)
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

    void CurrFrameVisitor::visit(windows::NCursesShortcutsWindowDecorator *window, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryPosition)
    {
        size_t shortWindowLines = utils::NCursesWrapper::getScreenDimenstions().first - 2UL;
        utils::ScreenBuffer newBuffer(shortWindowLines);
        newBuffer.fillBufferLine(U"");
        newBuffer.fillBufferLine(U" Session ID :");
        newBuffer.fillBufferLine(U" " + utils::helpers::Lambda::mapStringToWideString(window->getParentWindowHandler()->getUUID().getString()));
        for (auto const &e : window->getShortcuts())
        {
            newBuffer.fillBufferLine(e);
            if (newBuffer.getFilledRows() == shortWindowLines)
            {
                break;
            }
        }
        window->print(std::move(newBuffer));
    }

    void CurrFrameVisitor::visit(windows::NCursesTopWindowDecorator *window, std::optional<std::pair<utils::windows::FramePositions, pair>> arbitraryposition)
    {
        size_t noColumns = utils::NCursesWrapper::getScreenDimenstions().second;
        utils::ScreenBuffer newBuffer(1, 1, noColumns);
        std::string flatFileName = window->getParentWindowHandler()->getBufferFiller()->getStorage()->getName();
        flatFileName = (flatFileName.rfind('/') == std::string::npos) ? flatFileName : flatFileName.substr(flatFileName.rfind('/') + 1UL);
        if (flatFileName.size() > noColumns)
        {
            flatFileName.erase(flatFileName.begin(), std::next(flatFileName.begin(), flatFileName.size() - noColumns));
            flatFileName = utils::helpers::Lambda::alignBufferBeginingForString(std::move(flatFileName));
        }
        std::u32string fileName(utils::helpers::Lambda::mapStringToWideString(flatFileName));
        std::u32string line(noColumns, ' ');
        size_t fileNameWidth = utils::helpers::Lambda::getStringWidth(fileName, 0, fileName.length(), false, false);
        if (fileNameWidth + 2UL < noColumns)
        {
            std::copy(fileName.begin(), fileName.end(), std::next(line.begin(), line.size() / 2UL - fileNameWidth / 2UL));
            std::copy(line.begin(), line.end(), std::back_inserter(newBuffer[0]));
        }
        window->print(std::move(newBuffer));
    }
} // namespace feather::printer
