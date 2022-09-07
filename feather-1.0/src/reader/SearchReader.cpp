#include "action/input/SaveChangesActionInput.hpp"
#include "action/input/SearchSubstringActionInput.hpp"
#include "reader/SearchReader.hpp"
#include "utils/Utf8Util.hpp"
#include "utils/datatypes/Strings.hpp"
#include "utils/exception/FeatherInvalidArgumnetException.hpp"
#include "utils/storage/InMemoryStorageFactory.hpp"
#include "windows/NCursesBottomBarWindowDecorator.hpp"

extern feather::utils::FEATHER_MODE currentFeatherMode;

namespace feather::reader
{
    SearchReader::SearchReader(std::unique_ptr<windows::SubWindowInterface> &currBottomBarWindow, std::shared_ptr<action::ActionHandler> actionHandler, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator, std::shared_ptr<utils::storage::AbstractStorageFactory> storageFactory, std::shared_ptr<windows::WindowsManager> windowsManager, std::shared_ptr<utils::helpers::KeyReader> keyReader, std::list<pair> &breakPointsArg, std::set<pair> &breakPointsAddedArg, std::list<pair>::iterator &currentBreakPointArg, std::pair<utils::windows::FramePositions, pair> framePosition, pair cursorPosArg, utils::Direction directionArg)
        : Reader(currBottomBarWindow, actionHandler, printingOrchestrator, storageFactory, windowsManager, keyReader, framePosition, breakPointsArg, breakPointsAddedArg, currentBreakPointArg), cursorPos(cursorPosArg), trialVersionTimeout(std::chrono::steady_clock::now()), direction(directionArg)
    {
        lastAddBreakpointPressed = std::chrono::steady_clock::now();
        searchToken = (currentFeatherMode == utils::FEATHER_MODE::SEARCH_MODE) ? (utils::Direction::UP == direction ? U'?' : U'/') : (utils::Direction::UP == direction ? U'|' : U'\\');
        commandBuffer.push_back(searchToken);
        currBottomBarWindow->enableCursor();
        dynamic_cast<windows::NCursesBottomBarWindowDecorator *>(currBottomBarWindow.get())->setReader(this);
    }

    bool SearchReader::isAtHistoryBegin() const
    {
        if (utils::FEATHER_MODE::SEARCH_MODE == currentFeatherMode)
        {
            return dynamic_cast<windows::NCursesBottomBarWindowDecorator *>(currBottomBarWindow.get())->isSearchHistoryIteratorAtBegining();
        }
        else if (utils::FEATHER_MODE::HEX_SEARCH_MODE == currentFeatherMode)
        {
            return dynamic_cast<windows::NCursesBottomBarWindowDecorator *>(currBottomBarWindow.get())->isSearchHexHistoryIteratorAtBegining();
        }
        return false;
    }

    bool SearchReader::isAtHistoryEnd() const
    {
        if (utils::FEATHER_MODE::SEARCH_MODE == currentFeatherMode)
        {
            return dynamic_cast<windows::NCursesBottomBarWindowDecorator *>(currBottomBarWindow.get())->isSearchHistoryIteratorAtLastPos();
        }
        else if (utils::FEATHER_MODE::HEX_SEARCH_MODE == currentFeatherMode)
        {
            return dynamic_cast<windows::NCursesBottomBarWindowDecorator *>(currBottomBarWindow.get())->isSearchHexHistoryIteratorAtLastPos();
        }
        return false;
    }

    std::optional<std::u32string> SearchReader::getCurrFromHistory() const
    {
        std::optional<std::u32string> currFromHistory = std::nullopt;
        if (utils::FEATHER_MODE::SEARCH_MODE == currentFeatherMode)
        {
            currFromHistory = dynamic_cast<windows::NCursesBottomBarWindowDecorator *>(currBottomBarWindow.get())->getCurrFromSearchHistory();
        }
        else if (utils::FEATHER_MODE::HEX_SEARCH_MODE == currentFeatherMode)
        {
            currFromHistory = dynamic_cast<windows::NCursesBottomBarWindowDecorator *>(currBottomBarWindow.get())->getCurrFromSearchHexHistory();
        }

        if (!currFromHistory.has_value())
        {
            return commandBuffer;
        }
        return currFromHistory;
    }

    std::optional<std::u32string> SearchReader::getNextFromHistory() const
    {
        std::optional<std::u32string> nextFromHistory = std::nullopt;
        if (utils::FEATHER_MODE::SEARCH_MODE == currentFeatherMode)
        {
            nextFromHistory = dynamic_cast<windows::NCursesBottomBarWindowDecorator *>(currBottomBarWindow.get())->getNextFromSearchHistory();
        }
        else if (utils::FEATHER_MODE::HEX_SEARCH_MODE == currentFeatherMode)
        {
            nextFromHistory = dynamic_cast<windows::NCursesBottomBarWindowDecorator *>(currBottomBarWindow.get())->getNextFromSearchHexHistory();
        }
        return nextFromHistory;
    }

    std::optional<std::u32string> SearchReader::getPreviousFromHistory() const
    {
        std::optional<std::u32string> prevFromHistory = std::nullopt;
        if (utils::FEATHER_MODE::SEARCH_MODE == currentFeatherMode)
        {
            prevFromHistory = dynamic_cast<windows::NCursesBottomBarWindowDecorator *>(currBottomBarWindow.get())->getPreviousFromSearchHistory();
        }
        else if (utils::FEATHER_MODE::HEX_SEARCH_MODE == currentFeatherMode)
        {
            prevFromHistory = dynamic_cast<windows::NCursesBottomBarWindowDecorator *>(currBottomBarWindow.get())->getPreviousFromSearchHexHistory();
        }
        return prevFromHistory;
    }

    pair SearchReader::operator()()
    {
        char32_t scannedCharacter = EOF;
        printWithoutDelay(commandBuffer, false);
        while (true)
        {
            size_t bottomBarWindowWidth = utils::NCursesWrapper::getScreenDimenstions().second - feather::windows::WindowsAbstractFactory::PROGRESS_WINDOW_WIDTH - 2UL;
            auto cursorIdx = utils::helpers::Lambda::getIndexFromOffset(commandBuffer, commandBufferFirstIdx, currBottomBarWindow->getCursorPosition().second, bottomBarWindowWidth);
            auto lastIdxInWindow = utils::helpers::Lambda::getLastIndexInWindow(commandBuffer, commandBufferFirstIdx, bottomBarWindowWidth, false, false);
            switch (scannedCharacter = getNextCharacter(currBottomBarWindow->getLibraryWindowHandler()))
            {
            case static_cast<char32_t>(ERR):
                break;
            case KEY_RESIZE:
                terminalDimensionsChangeHandler(commandBuffer, cursorIdx);
                break;
            case KEY_LEFT:
            {
                if (currBottomBarWindow->getCursorPosition().second <= 1)
                {
                    shiftCursorHalfWindowLeft(cursorIdx);
                }
                if (cursorIdx >= 1UL)
                {
                    if (cursorIdx == commandBufferFirstIdx)
                    {
                        commandBufferFirstIdx -= 1UL;
                        printWithoutDelay(commandBuffer, false);
                    }
                    if (cursorIdx > 1UL)
                    {
                        setCursor(cursorIdx - 1UL);
                    }
                    else
                    {
                        setCursor(1UL);
                    }
                }
                break;
            }
            case KEY_RIGHT:
            {
                if (cursorIdx < commandBuffer.length())
                {
                    if (lastIdxInWindow == cursorIdx)
                    {
                        while (utils::helpers::Lambda::getStringWidth(commandBuffer, commandBufferFirstIdx, cursorIdx + 1UL, false, false) > bottomBarWindowWidth)
                        {
                            commandBufferFirstIdx += 1;
                        }
                        printWithoutDelay(commandBuffer, false);
                    }
                    setCursor(cursorIdx + 1UL);
                }
                break;
            }
            case 10:
            case KEY_ENTER:
            {
                if (utils::FEATHER_MODE::SEARCH_MODE == currentFeatherMode)
                {
                    dynamic_cast<windows::NCursesBottomBarWindowDecorator *>(currBottomBarWindow.get())->addToSearchHistory(searchToken + commandBuffer.substr(1));
                }
                else if (utils::FEATHER_MODE::HEX_SEARCH_MODE == currentFeatherMode)
                {
                    dynamic_cast<windows::NCursesBottomBarWindowDecorator *>(currBottomBarWindow.get())->addToSearchHexHistory(searchToken + commandBuffer.substr(1));
                }
                return handleCommand(commandBuffer.substr(1));
            }
            case KEY_DOWN:
            {
                if (anyChangeMade)
                {
                    printWithoutDelay(commandBuffer, false);
                    break;
                }
                if ((utils::FEATHER_MODE::SEARCH_MODE == currentFeatherMode && dynamic_cast<windows::NCursesBottomBarWindowDecorator *>(currBottomBarWindow.get())->isSearchHistoryIteratorAtLastPos()) ||
                    (utils::FEATHER_MODE::HEX_SEARCH_MODE == currentFeatherMode && dynamic_cast<windows::NCursesBottomBarWindowDecorator *>(currBottomBarWindow.get())->isSearchHexHistoryIteratorAtLastPos()))
                {
                    commandBuffer = searchToken;
                    printWithoutDelay(commandBuffer, false);
                    break;
                }
                currBottomBarWindow->refreshBuffer(lineDownVisitor.get());
                if (utils::FEATHER_MODE::SEARCH_MODE == currentFeatherMode)
                {
                    commandBuffer = *dynamic_cast<windows::NCursesBottomBarWindowDecorator *>(currBottomBarWindow.get())->getCurrFromSearchHistory();
                    printWithoutDelay(commandBuffer, false);
                    commandBuffer.erase(std::remove_if(commandBuffer.begin(), commandBuffer.end(), utils::helpers::Lambda::isNewLineChar), commandBuffer.end());
                }
                else if (utils::FEATHER_MODE::HEX_SEARCH_MODE == currentFeatherMode)
                {
                    commandBuffer = *dynamic_cast<windows::NCursesBottomBarWindowDecorator *>(currBottomBarWindow.get())->getCurrFromSearchHexHistory();
                    printWithoutDelay(commandBuffer, false);
                }
                break;
            }
            case KEY_UP:
            {
                if (anyChangeMade || (utils::FEATHER_MODE::SEARCH_MODE == currentFeatherMode && dynamic_cast<windows::NCursesBottomBarWindowDecorator *>(currBottomBarWindow.get())->isSearchHistoryEmpty()) || (utils::FEATHER_MODE::HEX_SEARCH_MODE == currentFeatherMode && dynamic_cast<windows::NCursesBottomBarWindowDecorator *>(currBottomBarWindow.get())->isSearchHexHistoryEmpty()))
                {
                    break;
                }
                currBottomBarWindow->refreshBuffer(lineUpVisitor.get());
                if (utils::FEATHER_MODE::SEARCH_MODE == currentFeatherMode)
                {
                    commandBuffer = *dynamic_cast<windows::NCursesBottomBarWindowDecorator *>(currBottomBarWindow.get())->getCurrFromSearchHistory();
                    printWithoutDelay(commandBuffer, false);
                    commandBuffer.erase(std::remove_if(commandBuffer.begin(), commandBuffer.end(), utils::helpers::Lambda::isNewLineChar), commandBuffer.end());
                }
                else if (utils::FEATHER_MODE::HEX_SEARCH_MODE == currentFeatherMode)
                {
                    commandBuffer = *dynamic_cast<windows::NCursesBottomBarWindowDecorator *>(currBottomBarWindow.get())->getCurrFromSearchHexHistory();
                    printWithoutDelay(commandBuffer, false);
                }
                break;
            }
            case 0x08:
            case 127:
            case KEY_BACKSPACE:
            {
                if (currBottomBarWindow->getCursorPosition().second <= 1)
                {
                    shiftCursorHalfWindowLeft(cursorIdx);
                }
                anyChangeMade = true;
                if (cursorIdx > 1UL)
                {
                    if (cursorIdx == commandBufferFirstIdx + 1UL)
                    {
                        commandBufferFirstIdx -= 1UL;
                        printWithoutDelay(commandBuffer, false);
                    }
                    if (1UL != cursorIdx)
                    {
                        commandBuffer.erase(cursorIdx - 1UL, 1);
                    }
                    printWithoutDelay(commandBuffer, false);
                    setCursor(cursorIdx - 1UL);
                }
                if (1UL == commandBuffer.size())
                {
                    anyChangeMade = false;
                }
                break;
            }
            case ctrl(U'w'):
                anyChangeMade = false;
                removeWordBackward(cursorIdx);
                break;
            case 27: // QUIT
                printWithoutDelay(utils::datatypes::Strings::EMPTY_STRING, false);
                return framePosition.second;
            case U'\t':
                break;
            default:
            {
                anyChangeMade = true;
                if (cursorIdx > 0 && !utils::helpers::Lambda::isNewLineChar(scannedCharacter))
                {
                    commandBuffer.insert(std::next(commandBuffer.begin(), cursorIdx++), scannedCharacter);
                    while (utils::helpers::Lambda::getStringWidth(commandBuffer, commandBufferFirstIdx, cursorIdx, false, false) > bottomBarWindowWidth)
                    {
                        ++commandBufferFirstIdx;
                    }
                }
                timeout(0);
                while (ERR != static_cast<int>(scannedCharacter = getNextCharacter(currBottomBarWindow->getLibraryWindowHandler())))
                {
                    if (cursorIdx > 0 && !utils::helpers::Lambda::isNewLineChar(scannedCharacter) && !utils::helpers::Lambda::isTabChar(scannedCharacter))
                    {
                        commandBuffer.insert(std::next(commandBuffer.begin(), cursorIdx++), scannedCharacter);
                        while (utils::helpers::Lambda::getStringWidth(commandBuffer, commandBufferFirstIdx, cursorIdx, false, false) > bottomBarWindowWidth)
                        {
                            ++commandBufferFirstIdx;
                        }
                    }
                    timeout(0);
                }
                printWithoutDelay(commandBuffer, false);
                setCursor(cursorIdx);
            }
            }
        }
    }

    void SearchReader::addBreakpoint()
    {
        pair cursorPos = currBottomBarWindow->getParentWindowHandler()->getCursorRealPosition(printingOrchestrator.get());
        if (!breakPointsAdded.insert(cursorPos).second)
        {
            return;
        }
        breakPoints.push_back(cursorPos);
        if (breakPoints.cend() == currentBreakPoint)
        {
            currentBreakPoint = breakPoints.begin();
        }
        currBottomBarWindow->getParentWindowHandler()->disableCursor();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastAddBreakpointPressed) > std::chrono::milliseconds(1000))
        {
            utils::helpers::Lambda::blinkPrint(utils::datatypes::Strings::ADDED_BREAKPOINT, (currentFeatherMode == utils::FEATHER_MODE::BROWSE_SEARCH_RESULT_DOWN_MODE) ? utils::datatypes::Strings::BROWSE_SEARCH_DOWN_MODE : utils::datatypes::Strings::BROWSE_SEARCH_UP_MODE, ONE_SECOND, currBottomBarWindow);
        }
        lastAddBreakpointPressed = std::chrono::steady_clock::now();
        currBottomBarWindow->getParentWindowHandler()->setCursorFromRealPosition(cursorPos, printingOrchestrator, halfFrameUpVisitor);
        currBottomBarWindow->getParentWindowHandler()->enableCursor();
    }

    pair SearchReader::handleCommand(std::u32string const &searchedStringRef)
    {
        currBottomBarWindow->disableCursor();
        if (searchedStringRef.empty())
        {
            return framePosition.second;
        }
        utils::FEATHER_MODE oldMode = currentFeatherMode;
        std::u32string searchedString;
        try
        {
            std::u32string string = searchedStringRef;
            if (currentFeatherMode == utils::FEATHER_MODE::HEX_SEARCH_MODE)
            {
                std::transform(string.begin(), string.end(), string.begin(), ::toupper);
                if (string.length() % 2)
                {
                    string.insert(string.find_first_of(U'X') + 1UL, U"0");
                }
            }

            searchedString = (currentFeatherMode == utils::FEATHER_MODE::SEARCH_MODE) ? utils::helpers::Conversion::escapeSpecialCharacters(string) : utils::helpers::Lambda::convertStrigifiedHexToHex(string);
            if (searchedString.empty())
            {
                printingOrchestrator->setSearchedText(currBottomBarWindow->getParentWindowHandler()->getUUID(), std::nullopt);
                printError(utils::datatypes::Strings::NOTHING_WAS_FOUND);
                currentFeatherMode = oldMode;
                return framePosition.second;
            }
        }
        catch (utils::exception::FeatherInvalidArgumentException const &e)
        {
            printError(utils::datatypes::Strings::INVALID_HEX_STRING);
            windowsManager->refreshProgresWindow(currFrameVisitor, currBottomBarWindow->getParentWindowHandler()->getUUID());
            printingOrchestrator->setSearchedText(currBottomBarWindow->getParentWindowHandler()->getUUID(), std::nullopt);
            currentFeatherMode = oldMode;
            return framePosition.second;
        }
        action::SearchSubstringActionInput actionInput(currBottomBarWindow->getParentWindowHandler()->getUUID(), std::move(searchedString), storageFactory, std::make_shared<utils::storage::InMemoryStorageFactory>(), printingOrchestrator, currBottomBarWindow->getParentWindowHandler()->getBufferFiller(), windowsManager, currFrameVisitor, halfFrameUpVisitor, currBottomBarWindow, currentFeatherMode, direction, cursorPos, breakPoints, currentBreakPoint, breakPointsAdded, currentFeatherMode == utils::FEATHER_MODE::HEX_SEARCH_MODE);
        currentFeatherMode = utils::Direction::DOWN == direction ? utils::FEATHER_MODE::SEARCHING_IN_PROGRESS_DOWN : utils::FEATHER_MODE::SEARCHING_IN_PROGRESS_UP;
        action::SearchSubstringActionOutput result = handleSearchImpl(actionInput);
        if ((utils::datatypes::ERROR::INTERRUPTED == result.result.second) || (utils::datatypes::ERROR::FILE_NO_LONGER_AVAILABLE == result.result.second) || (utils::datatypes::ERROR::ESC_INTERRUPTED == result.result.second))
        {
            if (utils::datatypes::ERROR::INTERRUPTED == result.result.second)
            {
                printingOrchestrator->setSearchedText(currBottomBarWindow->getParentWindowHandler()->getUUID(), std::nullopt);
                printError(utils::datatypes::Strings::INTERRUPTED);
                return result.result.first->first;
            }
            else if (utils::datatypes::ERROR::ESC_INTERRUPTED == result.result.second)
            {
                printingOrchestrator->setSearchedText(currBottomBarWindow->getParentWindowHandler()->getUUID(), std::nullopt);
                return result.result.first->first;
            }
            else
            {
                printingOrchestrator->setSearchedText(currBottomBarWindow->getParentWindowHandler()->getUUID(), std::nullopt);
                printError(utils::datatypes::Strings::FILE_NO_LONGER_AVAILABLE);
            }
            return framePosition.second;
        }
        else if (!result.result.first.has_value())
        {
            printingOrchestrator->setSearchedText(currBottomBarWindow->getParentWindowHandler()->getUUID(), std::nullopt);
            printError(utils::datatypes::Strings::NOTHING_WAS_FOUND);
            currentFeatherMode = oldMode;
            return framePosition.second;
        }
        else
        {
            currentFeatherMode = utils::FEATHER_MODE::BROWSE_SEARCH_RESULT_MODE;
            printingOrchestrator->setSearchedText(currBottomBarWindow->getParentWindowHandler()->getUUID(), result.result.first);
            windowsManager->refreshAllWindows(currFrameVisitor);
            utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, result.result.first->first);
            printWithoutDelay(utils::datatypes::Strings::BROWSE_SEARCH_MODE, false);
            currBottomBarWindow->getParentWindowHandler()->enableCursor();
            while (true)
            {
                windowsManager->refreshProgresWindow(currFrameVisitor, currBottomBarWindow->getParentWindowHandler()->getUUID());
                currBottomBarWindow->getParentWindowHandler()->enableCursor();
                char32_t scannedCharacter = EOF;
                switch (scannedCharacter = getNextCharacter(currBottomBarWindow->getLibraryWindowHandler()))
                {
                case static_cast<char32_t>(ERR):
                    break;
                case KEY_RESIZE:
                {
                    terminalDimensionsChangeHandler(std::nullopt, result.result.first->first);
                    break;
                }
                case U'n':
                {
                    currBottomBarWindow->getParentWindowHandler()->disableCursor();
                    currentFeatherMode = utils::FEATHER_MODE::BROWSE_SEARCH_RESULT_DOWN_MODE;
                    actionInput.searchDirection = utils::Direction::DOWN;
                    result = handleSearchImpl(actionInput);
                    if (utils::datatypes::ERROR::FILE_NO_LONGER_AVAILABLE == result.result.second)
                    {
                        print(utils::datatypes::Strings::FILE_NO_LONGER_AVAILABLE, true);
                        windowsManager->refreshProgresWindow(currFrameVisitor, currBottomBarWindow->getParentWindowHandler()->getUUID());
                        currentFeatherMode = oldMode;
                        currBottomBarWindow->getParentWindowHandler()->enableCursor();
                        printingOrchestrator->setSearchedText(currBottomBarWindow->getParentWindowHandler()->getUUID(), std::nullopt);
                        return framePosition.second;
                    }
                    else if (!result.result.first.has_value())
                    {
                        currentFeatherMode = oldMode;
                        currBottomBarWindow->getParentWindowHandler()->enableCursor();
                        printingOrchestrator->setSearchedText(currBottomBarWindow->getParentWindowHandler()->getUUID(), std::nullopt);
                        windowsManager->refreshAllWindows(currFrameVisitor);
                        return framePosition.second;
                    }
                    printingOrchestrator->setSearchedText(currBottomBarWindow->getParentWindowHandler()->getUUID(), result.result.first);
                    windowsManager->refreshAllWindows(currFrameVisitor);
                    printWithoutDelay(utils::datatypes::Strings::BROWSE_SEARCH_DOWN_MODE);
                    utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, result.result.first->first);
                    framePosition = std::make_pair(currBottomBarWindow->getParentWindowHandler()->getFramePosition(), result.result.first->first);
                    windowsManager->refreshProgresWindow(currFrameVisitor, currBottomBarWindow->getParentWindowHandler()->getUUID(), std::make_pair(currBottomBarWindow->getParentWindowHandler()->getFramePosition(), result.result.first->first));
                    currBottomBarWindow->getParentWindowHandler()->enableCursor();
                    break;
                }
                case U'N':
                {
                    currBottomBarWindow->getParentWindowHandler()->disableCursor();
                    currentFeatherMode = utils::FEATHER_MODE::BROWSE_SEARCH_RESULT_UP_MODE;
                    actionInput.searchDirection = utils::Direction::UP;
                    result = handleSearchImpl(actionInput);
                    if (utils::datatypes::ERROR::FILE_NO_LONGER_AVAILABLE == result.result.second)
                    {
                        print(utils::datatypes::Strings::FILE_NO_LONGER_AVAILABLE, true);
                        currentFeatherMode = oldMode;
                        currBottomBarWindow->getParentWindowHandler()->enableCursor();
                        printingOrchestrator->setSearchedText(currBottomBarWindow->getParentWindowHandler()->getUUID(), std::nullopt);
                        return framePosition.second;
                    }
                    else if (!result.result.first.has_value())
                    {
                        currentFeatherMode = oldMode;
                        currBottomBarWindow->getParentWindowHandler()->enableCursor();
                        printingOrchestrator->setSearchedText(currBottomBarWindow->getParentWindowHandler()->getUUID(), std::nullopt);
                        windowsManager->refreshAllWindows(currFrameVisitor);
                        return framePosition.second;
                    }
                    printingOrchestrator->setSearchedText(currBottomBarWindow->getParentWindowHandler()->getUUID(), result.result.first);
                    windowsManager->refreshAllWindows(currFrameVisitor);
                    printWithoutDelay(utils::datatypes::Strings::BROWSE_SEARCH_UP_MODE);
                    utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, result.result.first->first);
                    framePosition = std::make_pair(currBottomBarWindow->getParentWindowHandler()->getFramePosition(), result.result.first->first);
                    windowsManager->refreshProgresWindow(currFrameVisitor, currBottomBarWindow->getParentWindowHandler()->getUUID(), std::make_pair(currBottomBarWindow->getParentWindowHandler()->getFramePosition(), result.result.first->first));
                    currBottomBarWindow->getParentWindowHandler()->enableCursor();
                    break;
                }
                case ctrl(U'b'): // Add break point
                {
                    addBreakpoint();
                    break;
                }
                case 27:
                {
                    printWithoutDelay(utils::datatypes::Strings::EMPTY_STRING);
                    printingOrchestrator->setSearchedText(currBottomBarWindow->getParentWindowHandler()->getUUID(), std::nullopt);
                    return result.result.first->first;
                }
                default:
                    break;
                }
            }
        }
        print(utils::datatypes::Strings::EMPTY_STRING, false);
        printingOrchestrator->setSearchedText(currBottomBarWindow->getParentWindowHandler()->getUUID(), std::nullopt);
        return framePosition.second;
    }

    action::SearchSubstringActionOutput SearchReader::handleSearchImpl(action::SearchSubstringActionInput actionInput)
    {
        currBottomBarWindow->getParentWindowHandler()->disableCursor();
        currBottomBarWindow->disableCursor();
        windowsManager->refreshInfoWindow(currFrameVisitor, currBottomBarWindow->getParentWindowHandler()->getUUID());
        return actionHandler->handle(std::make_unique<action::SearchSubstringAction>(actionInput));
    }

    SearchReader::~SearchReader()
    {
        dynamic_cast<windows::NCursesBottomBarWindowDecorator *>(currBottomBarWindow.get())->setReader(nullptr);
        dynamic_cast<windows::NCursesBottomBarWindowDecorator *>(currBottomBarWindow.get())->setSearchHistoryIteratorAtEnd();
        dynamic_cast<windows::NCursesBottomBarWindowDecorator *>(currBottomBarWindow.get())->setSearchHexHistoryIteratorAtEnd();
    }
} // namespace feather::reader
