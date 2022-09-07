#include "printer/CurrFrameVisitor.hpp"
#include "printer/HalfFrameUpVisitor.hpp"
#include "printer/LineDownVisitor.hpp"
#include "printer/LineUpVisitor.hpp"
#include "utils/datatypes/Strings.hpp"
#include "reader/Reader.hpp"

#include <csignal>
#include <thread>

namespace feather::reader
{
    Reader::Reader(std::unique_ptr<windows::SubWindowInterface> &currBottomBarWindowArg, std::shared_ptr<action::ActionHandler> actionHandlerArg, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestratorArg, std::shared_ptr<utils::storage::AbstractStorageFactory> storageFactoryArg, std::shared_ptr<windows::WindowsManager> windowsManagerArg, std::shared_ptr<utils::helpers::KeyReader> keyReaderArg, std::pair<utils::windows::FramePositions, pair> framePositionArg, std::list<pair> &breakPointsArg, std::set<pair> &breakPointsAddedArg, std::list<pair>::iterator &currentBreakPointArg)
        : framePosition(framePositionArg), anyChangeMade(false), actionHandler(actionHandlerArg), currFrameVisitor(std::make_unique<printer::CurrFrameVisitor>(printingOrchestratorArg)), halfFrameUpVisitor(std::make_unique<printer::HalfFrameUpVisitor>(printingOrchestratorArg)), lineDownVisitor(std::make_unique<printer::LineDownVisitor>(printingOrchestratorArg)), lineUpVisitor(std::make_unique<printer::LineUpVisitor>(printingOrchestratorArg)), printingOrchestrator(printingOrchestratorArg), keyReader(keyReaderArg), storageFactory(storageFactoryArg), currBottomBarWindow(currBottomBarWindowArg), windowsManager(windowsManagerArg), breakPoints(breakPointsArg), breakPointsAdded(breakPointsAddedArg), currentBreakPoint(currentBreakPointArg)
    {
        commandBuffer.clear();
        commandBufferFirstIdx = 0;
    }

    void Reader::blockSignals()
    {
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGBUS);
        sigaddset(&mask, SIGINT);
        pthread_sigmask(SIG_BLOCK, &mask, NULL);
    }

    void Reader::unblockSignals()
    {
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGBUS);
        sigaddset(&mask, SIGINT);
        pthread_sigmask(SIG_UNBLOCK, &mask, NULL);
    }

    void Reader::print(std::u32string toPrint, bool withErase)
    {
        if (withErase)
        {
            commandBuffer.clear();
        }
        utils::helpers::Lambda::blinkPrint(toPrint, utils::datatypes::Strings::EMPTY_STRING, ONE_SECOND, currBottomBarWindow);
    }

    void Reader::printError(std::u32string toPrint)
    {
        commandBuffer.clear();
        utils::helpers::Lambda::subWindowPrint(toPrint, TWO_SECOND, currBottomBarWindow, windowsManager->getSubWindow(windowsManager->getProgressBarWindow(currBottomBarWindow->getParentWindowHandler()->getUUID()).value()), currFrameVisitor);
    }

    void Reader::printWithoutDelay(std::u32string toPrint, bool withErase)
    {
        size_t bottomBarWindowWidth = utils::NCursesWrapper::getScreenDimenstions().second - feather::windows::WindowsAbstractFactory::PROGRESS_WINDOW_WIDTH - 2UL;
        auto const cursorIdx = utils::helpers::Lambda::getIndexFromOffset(commandBuffer, commandBufferFirstIdx, currBottomBarWindow->getCursorPosition().second, bottomBarWindowWidth);

        if (withErase)
        {
            commandBuffer.clear();
        }
        toPrint.erase(0, commandBufferFirstIdx);
        utils::helpers::Lambda::subWindowPrintWithoutDelay(toPrint, currBottomBarWindow, windowsManager->getSubWindow(windowsManager->getProgressBarWindow(currBottomBarWindow->getParentWindowHandler()->getUUID()).value()), currFrameVisitor);
        setCursor(cursorIdx);
    }

    void Reader::refreshBuffer(std::u32string s)
    {
        currBottomBarWindow->print(utils::ScreenBuffer({s.begin(), s.end()}, 1));
    }

    void Reader::setCursor(size_t currIdx)
    {
        if (currIdx < commandBufferFirstIdx)
        {
            commandBufferFirstIdx = currIdx;
            shiftCursorHalfWindowLeft(currIdx);
        }
        size_t cursorPos = 0, firstIdx = commandBufferFirstIdx;
        auto bottomBarWindowWidth = utils::NCursesWrapper::getScreenDimenstions().second - feather::windows::WindowsAbstractFactory::PROGRESS_WINDOW_WIDTH - 2UL;
        auto lastIdxInWindow = utils::helpers::Lambda::getLastIndexInWindow(commandBuffer, commandBufferFirstIdx, bottomBarWindowWidth, false, false);

        while (firstIdx < commandBuffer.size() && firstIdx < lastIdxInWindow && firstIdx != currIdx)
        {
            cursorPos += utils::helpers::Lambda::getCharacterWidth(commandBuffer[firstIdx++], false, false);
        }
        currBottomBarWindow->setCursor(pair(0, cursorPos));
        currBottomBarWindow->enableCursor();
    }

    void Reader::setCursorAtTheEnd()
    {
        auto bottomBarWindowWidth = utils::NCursesWrapper::getScreenDimenstions().second - feather::windows::WindowsAbstractFactory::PROGRESS_WINDOW_WIDTH - 2UL;
        while (utils::helpers::Lambda::getStringWidth(commandBuffer, commandBufferFirstIdx, commandBuffer.length(), false, false) >= bottomBarWindowWidth)
        {
            ++commandBufferFirstIdx;
        }
        setCursor(commandBuffer.length());
    }

    void Reader::setCursorAtProperPosition(bool shouldEnableCursor)
    {
        windowsManager->refreshAllWindows(currFrameVisitor);
        if (shouldEnableCursor)
        {
            currBottomBarWindow->enableCursor();
        }
    }

    void Reader::shiftCursorHalfWindowLeft(size_t cursorIdx)
    {
        size_t bottomBarWindowWidth = utils::NCursesWrapper::getScreenDimenstions().second - feather::windows::WindowsAbstractFactory::PROGRESS_WINDOW_WIDTH - 2UL;
        size_t currWidth = 0;
        while (currWidth < bottomBarWindowWidth / 2UL && commandBufferFirstIdx > 0)
        {
            currWidth += utils::helpers::Lambda::getCharacterWidth(commandBuffer[commandBufferFirstIdx--], false, false);
        }
        setCursor(cursorIdx);
        printWithoutDelay(commandBuffer, false);
    }

    void Reader::terminalDimensionsChangeHandler(std::optional<std::u32string> toPrint, size_t lastCursorIdx)
    {
        auto bottomBarWindowWidth = utils::NCursesWrapper::getScreenDimenstions().second - feather::windows::WindowsAbstractFactory::PROGRESS_WINDOW_WIDTH - 2UL;
        commandBufferFirstIdx = lastCursorIdx;
        windowsManager->recreateWindows();
        windowsManager->refreshAllWindows(currFrameVisitor);
        if (toPrint.has_value())
        {
            utils::helpers::Lambda::subWindowPrint(toPrint.value(), std::nullopt, currBottomBarWindow, windowsManager->getSubWindow(windowsManager->getProgressBarWindow(currBottomBarWindow->getParentWindowHandler()->getUUID()).value()), currFrameVisitor);
        }
        bottomBarWindowWidth = utils::NCursesWrapper::getScreenDimenstions().second - feather::windows::WindowsAbstractFactory::PROGRESS_WINDOW_WIDTH - 2UL;
        bool isCursorEnabled = currBottomBarWindow->getLibraryWindowHandler()->isCursorEnabled();
        if (isCursorEnabled)
        {
            size_t currWidth = 0;
            while (currWidth < bottomBarWindowWidth / 2UL && commandBufferFirstIdx > 0)
            {
                currWidth += utils::helpers::Lambda::getCharacterWidth(commandBuffer[commandBufferFirstIdx--], false, false);
            }
            setCursor(lastCursorIdx);
            printWithoutDelay(commandBuffer, false);
            currBottomBarWindow->enableCursor();
        }
        utils::NCursesWrapper::applyRefresh();
    }

    void Reader::terminalDimensionsChangeHandler(std::optional<std::u32string> toPrint, pair lastCursorPos)
    {
        windowsManager->recreateWindows();
        windowsManager->refreshAllWindows(currFrameVisitor);
        if (toPrint.has_value())
        {
            utils::helpers::Lambda::subWindowPrint(toPrint.value(), std::nullopt, currBottomBarWindow, windowsManager->getSubWindow(windowsManager->getProgressBarWindow(currBottomBarWindow->getParentWindowHandler()->getUUID()).value()), currFrameVisitor);
        }
        utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, lastCursorPos);
        currBottomBarWindow->getParentWindowHandler()->enableCursor();
        utils::NCursesWrapper::applyRefresh();
    }

    void Reader::trialEndedApplicationHandler()
    {
        currBottomBarWindow->disableCursor();
        char32_t scannedCharacter = EOF;
        while (true)
        {
            switch (scannedCharacter = keyReader->getNextCharacterFromInputBuffer(currBottomBarWindow->getParentWindowHandler()->getLibraryWindowHandler()))
            {
            case static_cast<char32_t>(ERR):
                break;
            case KEY_RESIZE:
            {
                terminalDimensionsChangeHandler(commandBuffer, 0);
                break;
            }
            case 10:
            case KEY_ENTER:
            {
                return;
            }
            default:
            {
                break;
            }
            }
        }
    }

    bool Reader::isAtHistoryBegin() const
    {
        return false;
    }

    bool Reader::isAtHistoryEnd() const
    {
        return false;
    }

    bool Reader::isBufferEmpty() const
    {
        return commandBuffer.size() == 1;
    }

    char32_t Reader::getNextCharacter(windows::WindowImplInterface const *windowImplementation)
    {
        lastKeyPressed = std::chrono::steady_clock::now();
        return keyReader->getNextCharacterFromInputBuffer(windowImplementation);
    }

    std::u32string Reader::getCommandBuffer() const
    {
        return commandBuffer;
    }

    std::optional<std::u32string> Reader::getCurrFromHistory() const
    {
        return std::nullopt;
    }

    std::optional<std::u32string> Reader::getNextFromHistory() const
    {
        return std::nullopt;
    }

    std::optional<std::u32string> Reader::getPreviousFromHistory() const
    {
        return std::nullopt;
    }

    size_t Reader::removeWordBackward(size_t cursorIdx)
    {
        if (cursorIdx > 1UL)
        {
            anyChangeMade = true;
            bool belongingToWordRemoved = false, trailingWhiteCharsRemoved = false;
            while (cursorIdx > 1UL && utils::helpers::Lambda::characterBelongToWord(commandBuffer[cursorIdx - 1UL]))
            {
                commandBuffer.erase(--cursorIdx, 1UL);
                belongingToWordRemoved = true;
            }

            while (!belongingToWordRemoved && cursorIdx > 1UL && utils::helpers::Lambda::isWhiteCharOrNewLine(commandBuffer[cursorIdx - 1UL]))
            {
                commandBuffer.erase(--cursorIdx, 1UL);
                trailingWhiteCharsRemoved = true;
            }

            while (!belongingToWordRemoved && !trailingWhiteCharsRemoved && cursorIdx > 1UL && utils::helpers::Lambda::characterDontBelongToWord(commandBuffer[cursorIdx - 1UL]))
            {
                commandBuffer.erase(--cursorIdx, 1UL);
            }
            setCursor(cursorIdx);
            printWithoutDelay(commandBuffer, false);
        }
        return cursorIdx;
    }

    Reader::~Reader() {}
} // namespace feather::reader
