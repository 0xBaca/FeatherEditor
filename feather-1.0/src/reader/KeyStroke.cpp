#include "printer/CurrFrameVisitor.hpp"
#include "printer/FilePrintingStorage.hpp"
#include "printer/MemoryPrintingStorage.hpp"
#include "printer/HalfFrameDownVisitor.hpp"
#include "printer/HalfFrameUpVisitor.hpp"
#include "printer/FrameDownVisitor.hpp"
#include "printer/FrameUpVisitor.hpp"
#include "printer/LineDownVisitor.hpp"
#include "printer/LineUpVisitor.hpp"
#include "reader/ActionReader.hpp"
#include "reader/SearchReader.hpp"
#include "reader/KeyStroke.hpp"
#include "utils/datatypes/Strings.hpp"
#include "utils/Utf8Util.hpp"
#include "utils/exception/FeatherInterruptedException.hpp"
#include "utils/windows/WindowLinesLengthInformationVisitor.hpp"

#include <thread>

bool exitFeather = false;
std::chrono::time_point<std::chrono::steady_clock> lastKeyPressed;
std::chrono::time_point<std::chrono::steady_clock> startTime;

extern feather::utils::FEATHER_MODE currentFeatherMode;

namespace feather::reader
{
    KeyStroke::KeyStroke(
        std::shared_ptr<windows::WindowsManager> windowsManagerArg,
        std::shared_ptr<utils::logger::policy::FileLogPolicy> logArg,
        std::shared_ptr<utils::storage::AbstractStorageFactory> storageFactoryArg)
        : trialVersionTimeout(std::chrono::steady_clock::now()), closeApplication(false), log(logArg), storageFactory(storageFactoryArg), windowsManager(windowsManagerArg), inputBuffer(1024), selectedText(std::nullopt)
    {
        currentBreakPoint[windowsManager->getActiveMainWindow()] = std::end(breakPoints[windowsManager->getActiveMainWindow()]);
        filePrintingStorage = std::make_shared<printer::FilePrintingStorage>(storageFactoryArg);
        keyReader = std::make_shared<utils::helpers::KeyReader>();
        memoryPrintingStorage = std::make_shared<printer::MemoryPrintingStorage>();
        changesStack = std::make_shared<printer::ChangesStack>();
        lastCommand[windowsManager->getActiveMainWindow()] = utils::datatypes::Command::NONE;
        filesystem = std::make_shared<utils::Filesystem>();
        printingOrchestrator = std::make_shared<printer::PrintingOrchestrator>(storageFactoryArg, configuration->isCacheOn() ? memoryPrintingStorage : filePrintingStorage, configuration->isCacheOn() ? filePrintingStorage : nullptr, changesStack, filesystem, windowsManager->getMainWindow(windowsManager->getActiveMainWindow())->getStorage());
        currFrameVisitor = std::make_unique<printer::CurrFrameVisitor>(printingOrchestrator);
        lineDownVisitor = std::make_unique<printer::LineDownVisitor>(printingOrchestrator);
        lineUpVisitor = std::make_unique<printer::LineUpVisitor>(printingOrchestrator);
        frameDownVisitor = std::make_unique<printer::FrameDownVisitor>(printingOrchestrator);
        frameUpVisitor = std::make_unique<printer::FrameUpVisitor>(printingOrchestrator);
        halfFrameDownVisitor = std::make_unique<printer::HalfFrameDownVisitor>(printingOrchestrator);
        halfFrameUpVisitor = std::make_unique<printer::HalfFrameUpVisitor>(printingOrchestrator);
        linesLengthInformationVisitor = std::make_shared<utils::windows::WindowLinesLengthInformationVisitor>();
        actionHandler = std::make_shared<action::ActionHandler>(printingOrchestrator, windowsManager);
        lastKeyPressed = std::chrono::steady_clock::now();
        startTime = std::chrono::steady_clock::now();
        actionType = std::make_pair(action::ACTION_TYPE::UNKNOWN, 0);
        windows::MainWindowInterface::setPrintingOrchestrator(this->printingOrchestrator);
    }

    void KeyStroke::operator()()
    {
        char32_t utf8Character = 0;
        auto activeMainWindow = windowsManager->getMainWindow(windowsManager->getActiveMainWindow());
        auto &bottomBarWindow = windowsManager->addNCursesBottomBarWindow(activeMainWindow);
        if (utils::helpers::Lambda::isScreenBigEnough())
        {
            windowsManager->addNCursesAgreementWindow(activeMainWindow);
            windowsManager->addNCursesExitWindow(activeMainWindow);
            windowsManager->addNCursesShortcutsWindow(activeMainWindow);
        }
        windowsManager->refreshAllWindows(currFrameVisitor);
        refresh(activeMainWindow, true, std::nullopt);
        while (true)
        {
            activeMainWindow = windowsManager->getMainWindow(windowsManager->getActiveMainWindow());
            timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
            pair currCursorPos = activeMainWindow->getCursorPosition();
            //Safe guard when comming back from working mode
            if (utils::FEATHER_MODE::WORKING_MODE == currentFeatherMode)
            {
                setModeToReadMode(bottomBarWindow);
            }
            switch (utf8Character = keyReader->getNextCharacterFromInputBuffer(activeMainWindow->getLibraryWindowHandler()))
            {
            case static_cast<char32_t>(ERR):
                activeMainWindow->getLibraryWindowHandler()->setBlockingRead();
                highlightTextIfNeeded(activeMainWindow);
                break;
            case KEY_RESIZE:
                defaultRedrawWindows(activeMainWindow);
                break;
            case ctrl('u'): // half frame up
                if (currentFeatherMode == utils::FEATHER_MODE::READ_MODE || currentFeatherMode == utils::FEATHER_MODE::READ_MODE_SELECT_TEXT || utils::FEATHER_MODE::WRITE_MODE == currentFeatherMode)
                {
                    if (printingOrchestrator->isCharAtFirstPosition(activeMainWindow->getBufferFiller()->getFramePositions().startFramePosition, activeMainWindow->getUUID()))
                    {
                        if (currentFeatherMode == utils::FEATHER_MODE::READ_MODE_SELECT_TEXT)
                        {
                            auto firstValidPos = printingOrchestrator->getFirstValidPosition(activeMainWindow->getUUID());
                            utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, firstValidPos);
                            highlightTextIfNeeded(activeMainWindow);
                            refreshProgressWindow();
                        }
                        break;
                    }
                    auto lastCursorPos = activeMainWindow->getCursorPosition();
                    auto lastCursorRealPos = activeMainWindow->getCursorRealPosition(printingOrchestrator.get());
                    if (currentFeatherMode == utils::FEATHER_MODE::READ_MODE_SELECT_TEXT && activeMainWindow->getCursorPosition().first > activeMainWindow->getWindowDimensions().first / 2)
                    {
                        windowsManager->refreshAllWindows(currFrameVisitor);
                        updateCursorPosIfFileModified(activeMainWindow, lastCursorPos, lastCursorRealPos);
                        activeMainWindow->setCursorPosition(pair(activeMainWindow->getCursorPosition().first - activeMainWindow->getWindowDimensions().first / 2, 0));
                    }
                    else
                    {
                        windowsManager->refreshAllWindows(halfFrameUpVisitor);
                        updateCursorPosIfFileModified(activeMainWindow, lastCursorPos, lastCursorRealPos);

                        if (activeMainWindow->getBufferFiller()->getFramePositions().isInsideFrame(lastCursorRealPos))
                        {
                            activeMainWindow->setCursorFromRealPosition(lastCursorRealPos, printingOrchestrator, halfFrameUpVisitor);
                        }
                        else
                        {
                            activeMainWindow->setCursorPosition(pair(0, 0));
                        }
                    }
                    activeMainWindow->setCursorFromRealPosition(
                        actionHandler->handle(std::make_unique<action::SkipEmptyCharactersAction>(action::SkipEmptyCharactersActionInput(activeMainWindow, printingOrchestrator, windowsManager))).getResult(),
                        printingOrchestrator,
                        currFrameVisitor);
                    utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, activeMainWindow->getCursorRealPosition(printingOrchestrator.get()));
                    activeMainWindow->enableCursor();
                    highlightTextIfNeeded(activeMainWindow);
                    refreshProgressWindow();
                }
                break;
            case ctrl('d'): // half frame down
            {
                if (isLastValidPosInFrame())
                {
                    if (currentFeatherMode == utils::FEATHER_MODE::READ_MODE_SELECT_TEXT)
                    {
                        auto lastValidPos = printingOrchestrator->getLastValidPosition(activeMainWindow->getUUID(), activeMainWindow->getStorage());
                        auto pos = !utils::helpers::Lambda::isCharAtPos(lastValidPos, utils::helpers::Lambda::isNewLineChar, activeMainWindow->getUUID(), activeMainWindow->getStorage(), printingOrchestrator).first ? printingOrchestrator->getNextVirtualPosition(activeMainWindow->getUUID(), activeMainWindow->getStorage(), lastValidPos) : lastValidPos;
                        utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, pos);
                        highlightTextIfNeeded(activeMainWindow);
                        refreshProgressWindow();
                    }
                    break;
                }
                if (currentFeatherMode == utils::FEATHER_MODE::READ_MODE || currentFeatherMode == utils::FEATHER_MODE::READ_MODE_SELECT_TEXT || utils::FEATHER_MODE::WRITE_MODE == currentFeatherMode)
                {
                    auto lastCursorPos = activeMainWindow->getCursorPosition();
                    auto lastCursorRealPos = activeMainWindow->getCursorRealPosition(printingOrchestrator.get());

                    if (isCursorAtLastLine())
                    {
                        break;
                    }
                    if (currentFeatherMode == utils::FEATHER_MODE::READ_MODE_SELECT_TEXT && activeMainWindow->getCursorPosition().first < activeMainWindow->getWindowDimensions().first / 2)
                    {
                        windowsManager->refreshAllWindows(currFrameVisitor);
                        updateCursorPosIfFileModified(activeMainWindow, lastCursorPos, lastCursorRealPos);
                        activeMainWindow->setCursorPosition(pair(activeMainWindow->getCursorPosition().first + activeMainWindow->getWindowDimensions().first / 2, 0));
                    }
                    else
                    {
                        windowsManager->refreshAllWindows(halfFrameDownVisitor);
                        updateCursorPosIfFileModified(activeMainWindow, lastCursorPos, lastCursorRealPos);
                        if (activeMainWindow->getBufferFiller()->getFramePositions().isInsideFrame(lastCursorPos))
                        {
                            activeMainWindow->setCursorFromRealPosition(lastCursorRealPos, printingOrchestrator, halfFrameUpVisitor);
                        }
                        else
                        {
                            activeMainWindow->setCursorPosition(pair(0, 0));
                        }
                    }
                    activeMainWindow->setCursorFromRealPosition(
                        actionHandler->handle(std::make_unique<action::SkipEmptyCharactersAction>(action::SkipEmptyCharactersActionInput(activeMainWindow, printingOrchestrator, windowsManager))).getResult(),
                        printingOrchestrator,
                        currFrameVisitor);
                    utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, activeMainWindow->getCursorRealPosition(printingOrchestrator.get()));
                    activeMainWindow->enableCursor();
                    highlightTextIfNeeded(activeMainWindow);
                    refreshProgressWindow();
                }
                break;
            }
            case KEY_UP:
            {
                if (printingOrchestrator->isCharAtFirstPosition(activeMainWindow->getBufferFiller()->getFramePositions().startFramePosition, activeMainWindow->getUUID()) && !currCursorPos.first)
                {
                    break;
                }
                else if (0 == currCursorPos.first)
                {
                    auto lastCursorPos = activeMainWindow->getCursorPosition();
                    auto lastCursorRealPos = activeMainWindow->getCursorRealPosition(printingOrchestrator.get());
                    windowsManager->refreshAllWindows(lineUpVisitor);
                    updateCursorPosIfFileModified(activeMainWindow, lastCursorPos, lastCursorRealPos);
                    activeMainWindow->setCursorPosition(lastCursorPos);
                    activeMainWindow->moveCursor(utils::Direction::DOWN, linesLengthInformationVisitor, std::nullopt);
                }
                activeMainWindow->moveCursor(feather::utils::Direction::UP, linesLengthInformationVisitor, std::nullopt);
                utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, activeMainWindow->getCursorRealPosition(printingOrchestrator.get()));
                highlightTextIfNeeded(activeMainWindow);
                refreshProgressWindow();
                break;
            }
            case KEY_DOWN:
            {
                if (isCursorAtLastLine())
                {
                    break;
                }
                if (currCursorPos.first == activeMainWindow->getWindowDimensions().first - 1)
                {
                    auto lastCursorPos = activeMainWindow->getCursorPosition();
                    auto lastCursorRealPos = activeMainWindow->getCursorRealPosition(printingOrchestrator.get());
                    activeMainWindow->disableCursor();
                    windowsManager->refreshAllWindows(lineDownVisitor);
                    updateCursorPosIfFileModified(activeMainWindow, lastCursorPos, lastCursorRealPos);
                    activeMainWindow->setCursorPosition(lastCursorPos);
                    activeMainWindow->moveCursor(utils::Direction::UP, linesLengthInformationVisitor, std::nullopt);
                }
                activeMainWindow->moveCursor(feather::utils::Direction::DOWN, linesLengthInformationVisitor, std::nullopt);
                utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, activeMainWindow->getCursorRealPosition(printingOrchestrator.get()));
                highlightTextIfNeeded(activeMainWindow);
                refreshProgressWindow();
                break;
            }
            case KEY_LEFT:
            {
                moveCursorLeft(activeMainWindow);
                utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, activeMainWindow->getCursorRealPosition(printingOrchestrator.get()));
                highlightTextIfNeeded(activeMainWindow);
                refreshProgressWindow();
                break;
            }
            case KEY_RIGHT:
                moveCursorRight(activeMainWindow);
                utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, activeMainWindow->getCursorRealPosition(printingOrchestrator.get()));
                highlightTextIfNeeded(activeMainWindow);
                refreshProgressWindow();
                break;
            case 27: //{ESC} EXIT WRITE MODE
            {
                activeMainWindow->moveCursor(utils::Direction::ALIGN_TO_READ_MODE, linesLengthInformationVisitor, std::nullopt);
                setModeToReadMode(bottomBarWindow);
                break;
            }
            // case ctrl(U's'): //Split vertically
            //{
            //     break;
            // }
            // case ctrl(U'v'): //Split horizontally
            //{
            //     break;
            // }
            case ctrl(U'k'): // Shortcuts window
            {
                if ((utils::FEATHER_MODE::READ_MODE != currentFeatherMode && utils::FEATHER_MODE::WRITE_MODE != currentFeatherMode) || !windowsManager->getShortcutsWindow(activeMainWindow->getUUID()).has_value())
                {
                    break;
                }
                auto prevFeatherMode = currentFeatherMode;
                auto cursorPosition = activeMainWindow->getCursorRealPosition(printingOrchestrator.get());
                currentFeatherMode = prevFeatherMode == utils::FEATHER_MODE::READ_MODE ? utils::FEATHER_MODE::SHORTCUTS_READ_MODE : utils::FEATHER_MODE::SHORTCUTS_WRITE_MODE;
                windowsManager->showWindow(windowsManager->getShortcutsWindow(activeMainWindow->getUUID()).value(), currFrameVisitor);
                refreshInfoWindow(currFrameVisitor);
                activeMainWindow->disableCursor();
                bool continueScanning = true;
                while (continueScanning)
                {
                    switch (utf8Character = keyReader->getNextCharacterFromInputBuffer(activeMainWindow->getLibraryWindowHandler()))
                    {
                    case KEY_UP:
                    {
                        refreshShortcutsWindow(lineUpVisitor);
                        break;
                    }
                    case KEY_DOWN:
                    {
                        refreshShortcutsWindow(lineDownVisitor);
                        break;
                    }
                    default:
                        continueScanning = false;
                    }
                }
                currentFeatherMode = prevFeatherMode;
                windowsManager->hideWindow(windowsManager->getShortcutsWindow(activeMainWindow->getUUID()).value(), currFrameVisitor);
                utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, cursorPosition);
                highlightTextIfNeeded(activeMainWindow);
                refreshProgressWindow();
                break;
            }
            /*case ctrl(U'l'): //Switch mode
        {
            auto newCursorPosition = actionHandler->handle(std::make_unique<action::SwitchModeAction>(action::SwitchModeActionInput(activeMainWindow->getCursorRealPosition(printingOrchestrator.get()), activeMainWindow->getUUID(), activeMainWindow->getStorage(), printingOrchestrator))).getResult();
            windowsManager->refreshAllWindows(currFrameVisitor);
            activeMainWindow->setCursorFromRealPosition(
                newCursorPosition,
                printingOrchestrator,
                halfFrameUpVisitor);
            refreshProgressWindow();
            if (utils::FEATHER_MODE::READ_MODE == currentFeatherMode)
            {
                activeMainWindow->moveCursor(utils::Direction::ALIGN_TO_READ_MODE, linesLengthInformationVisitor, std::nullopt);
                setModeToReadMode(bottomBarWindow);
            }
            break;
        }
        */
            case ctrl(U'x'): // QUIT APP
            {
                if ((utils::FEATHER_MODE::READ_MODE != currentFeatherMode) || !windowsManager->getExitWindow(activeMainWindow->getUUID()).has_value())
                {
                    break;
                }
                auto lastCursorPos = activeMainWindow->getCursorRealPosition(printingOrchestrator.get());
                activeMainWindow->disableCursor();
                windowsManager->showWindow(windowsManager->getExitWindow(activeMainWindow->getUUID()).value(), currFrameVisitor);
                exitFeather = yesNoPrompt(activeMainWindow);
                if (exitFeather)
                {
                    utils::helpers::Lambda::removeFilesContainingSubstring(activeMainWindow->getUUID(), utils::datatypes::Strings::SECONDARY_SEARCH);
                    return;
                }
                if (windowsManager->getExitWindow(activeMainWindow->getUUID()).has_value())
                {
                    windowsManager->hideWindow(windowsManager->getExitWindow(activeMainWindow->getUUID()).value(), currFrameVisitor);
                }
                else
                {
                    windowsManager->setExitWindowVisibility(false);
                }
                windowsManager->refreshProgresWindow(currFrameVisitor, activeMainWindow->getUUID(), std::make_pair(activeMainWindow->getFramePosition(), lastCursorPos));
                utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, lastCursorPos);
                activeMainWindow->enableCursor();
                highlightTextIfNeeded(activeMainWindow);
                refreshProgressWindow();
                break;
            }
            default:
            {
                if (currentFeatherMode == feather::utils::FEATHER_MODE::WRITE_MODE)
                {
                    if (utils::helpers::Lambda::isBackspace(utf8Character))
                    {
                        backspaceHandler(activeMainWindow);
                        activeMainWindow->getLibraryWindowHandler()->setBlockingRead();
                    }
                    else if (ctrl('w') == utf8Character)
                    {
                        auto cursorPos = activeMainWindow->getCursorRealPosition(printingOrchestrator.get());
                        try
                        {
                            activeMainWindow->disableCursor();
                            auto newCursorPosition = actionHandler->handle(std::make_unique<action::RemoveWordBackwardAction>(action::RemoveWordBackwardActionInput(activeMainWindow->getUUID(), windowsManager, cursorPos, activeMainWindow->getStorage(), printingOrchestrator))).getResult();
                            utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, newCursorPosition);
                            if (isCursorAtFirstLine())
                            {
                                activeMainWindow->disableCursor();
                                windowsManager->refreshAllWindows(halfFrameUpVisitor);
                            }
                            else
                            {
                                activeMainWindow->disableCursor();
                                windowsManager->refreshAllWindows(currFrameVisitor);
                            }
                            utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, newCursorPosition);
                            activeMainWindow->enableCursor();
                            refreshProgressWindow();
                            activeMainWindow->getLibraryWindowHandler()->setBlockingRead();
                        }
                        catch (utils::exception::FeatherInterruptedException &e)
                        {
                            auto firstValidPos = printingOrchestrator->getNextVirtualPosition(activeMainWindow->getUUID(), activeMainWindow->getStorage(), cursorPos);
                            utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, firstValidPos);
                            refreshProgressWindow();
                        }
                    }
                    else if (ctrl(U'b') == utf8Character) // Add break point
                    {
                        addBreakpoint(activeMainWindow, bottomBarWindow);
                        activeMainWindow->getLibraryWindowHandler()->setBlockingRead();
                    }
                    else if (ctrl(U'p') == utf8Character) // Goto previous breakpoint
                    {
                        previousBreakpoint(activeMainWindow, bottomBarWindow);
                        activeMainWindow->getLibraryWindowHandler()->setBlockingRead();
                    }
                    else if (ctrl(U'n') == utf8Character) // Goto next breakpoint
                    {
                        nextBreakpoint(activeMainWindow, bottomBarWindow);
                        activeMainWindow->getLibraryWindowHandler()->setBlockingRead();
                    }
                    else
                    {
                        addNewCharacter(utf8Character, activeMainWindow, bottomBarWindow);
                    }
                }
                else if (utf8Character == U'8')
                {
                    auto currWindowFilePath = activeMainWindow->getStorage()->getName();
                    auto prevWindow = windowsManager->getPrevWindowToLoad(currWindowFilePath, filesystem);
                    if (prevWindow.has_value() && *prevWindow != currWindowFilePath)
                    {
                        auto newCursorPos = windowsManager->reloadWindowContent(*prevWindow, storageFactory);
                        windowsManager->refreshAllWindows(currFrameVisitor);
                        if (newCursorPos.has_value())
                        {
                            utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, newCursorPos.value());
                        }
                    }
                    refreshProgressWindow();
                    break;
                }
                else if (utf8Character == U'9')
                {
                    auto currWindowFilePath = activeMainWindow->getStorage()->getName();
                    auto nextWindow = windowsManager->getNexWindowToLoad(currWindowFilePath, filesystem);
                    if (nextWindow.has_value() && *nextWindow != currWindowFilePath)
                    {
                        auto newCursorPos = windowsManager->reloadWindowContent(*nextWindow, storageFactory);
                        windowsManager->refreshAllWindows(currFrameVisitor);
                        if (newCursorPos.has_value())
                        {
                            utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, newCursorPos.value());
                        }
                    }
                    refreshProgressWindow();
                    break;
                }
                else if (utf8Character == U'.') // repeat last command
                {
                    if (!lastCommand.count(activeMainWindow->getUUID()))
                    {
                        break;
                    }
                    else if (utils::datatypes::Command::BREAKPOINT_ADD == lastCommand[activeMainWindow->getUUID()])
                    {
                        goto add_breakpoint_command;
                    }
                    else if (utils::datatypes::Command::BREAKPOINT_NEXT == lastCommand[activeMainWindow->getUUID()])
                    {
                        goto next_breakpoint_command;
                    }
                    else if (utils::datatypes::Command::BREAKPOINT_PREVIOUS == lastCommand[activeMainWindow->getUUID()])
                    {
                        goto previous_breakpoint_command;
                    }
                    else if (utils::datatypes::Command::COPY_LINE == lastCommand[activeMainWindow->getUUID()])
                    {
                        goto copy_line_command;
                    }
                    else if (utils::datatypes::Command::GOTO_FILE_BEGIN == lastCommand[activeMainWindow->getUUID()])
                    {
                        goto file_begin_action;
                    }
                    else if (utils::datatypes::Command::GOTO_FILE_END == lastCommand[activeMainWindow->getUUID()])
                    {
                        goto file_end_action;
                    }
                    else if (utils::datatypes::Command::GOTO_LINE_BEGIN == lastCommand[activeMainWindow->getUUID()])
                    {
                        goto line_begin_action;
                    }
                    else if (utils::datatypes::Command::GOTO_LINE_END == lastCommand[activeMainWindow->getUUID()])
                    {
                        goto line_end_action;
                    }
                    else if (utils::datatypes::Command::GOTO_NEXT_WORD == lastCommand[activeMainWindow->getUUID()])
                    {
                        goto next_word_command;
                    }
                    else if (utils::datatypes::Command::GOTO_PREVIOUS_WORD == lastCommand[activeMainWindow->getUUID()])
                    {
                        goto previous_word_command;
                    }
                    else if (utils::datatypes::Command::HIGHLIGHT_LINE == lastCommand[activeMainWindow->getUUID()])
                    {
                        goto highlight_line_command;
                    }
                    else if (utils::datatypes::Command::HIGHLIGHT_BETWEEN_BRACKETS == lastCommand[activeMainWindow->getUUID()])
                    {
                        goto highlight_between_brackets_command;
                    }
                    else if (utils::datatypes::Command::PASTE == lastCommand[activeMainWindow->getUUID()])
                    {
                        goto paste_command;
                    }
                    else if (utils::datatypes::Command::REMOVE_LINE == lastCommand[activeMainWindow->getUUID()])
                    {
                        goto remove_line_command;
                    }
                    else if (utils::datatypes::Command::REMOVE_MULTIPLE_LINES == lastCommand[activeMainWindow->getUUID()])
                    {
                        goto remove_multiple_lines_command;
                    }
                    else if (utils::datatypes::Command::REMOVE_UNTIL_FILE_BEGIN == lastCommand[activeMainWindow->getUUID()])
                    {
                        goto remove_until_file_begin_command;
                    }
                    else if (utils::datatypes::Command::REMOVE_UNTIL_FILE_END == lastCommand[activeMainWindow->getUUID()])
                    {
                        goto remove_until_file_end_command;
                    }
                    else if (utils::datatypes::Command::REMOVE_SINGLE_CHAR == lastCommand[activeMainWindow->getUUID()])
                    {
                        goto remove_single_char_action;
                    }
                    else if (utils::datatypes::Command::REMOVE_UNTIL_LINE_END == lastCommand[activeMainWindow->getUUID()])
                    {
                        goto remove_until_line_end_action;
                    }
                    else if (utils::datatypes::Command::REMOVE_WORD_BACKWARD == lastCommand[activeMainWindow->getUUID()])
                    {
                        goto remove_word_backward_command;
                    }
                    else if (utils::datatypes::Command::REMOVE_WORD_FORWARD == lastCommand[activeMainWindow->getUUID()])
                    {
                        goto remove_word_forward_command;
                    }
                    else if (utils::datatypes::Command::REMOVE_BETWEEN_BRACKETS == lastCommand[activeMainWindow->getUUID()])
                    {
                        goto remove_between_brackets_command;
                    }
                    else if (utils::datatypes::Command::REMOVE_BETWEEN_WHITE_CHARACTERS == lastCommand[activeMainWindow->getUUID()])
                    {
                        goto remove_between_white_characters_command;
                    }
                    /*else if (utils::datatypes::Command::SHIFT_RIGHT == lastCommand)
                {
                    goto shift_right_command;
                }
                */
                    else if (utils::datatypes::Command::SHIFT_LEFT == lastCommand[activeMainWindow->getUUID()])
                    {
                        // goto shift_left_command;
                    }
                }
                /*
                else if (U'>' == utf8Character)
                {
                    lastCommand = utils::datatypes::Command::SHIFT_RIGHT;
                    if (selectedText->first > selectedText->second)
                    {
                        std::swap(selectedText->first, selectedText->second);
                    }
                    textToCopy = selectedText;
                    shift_right_command:
                    auto cursorPos = activeMainWindow->getCursorRealPosition(printingOrchestrator.get());
                    try
                    {
                        activeMainWindow->disableCursor();
                        auto newCursorPos = actionHandler->handle(std::make_unique<action::ShiftRightAction>(action::ShiftRightActionInput(selectedText.value(), activeMainWindow->getCursorRealPosition(printingOrchestrator.get()), activeMainWindow->getUUID(), activeMainWindow->getStorage(), printingOrchestrator, windowsManager, currFrameVisitor))).result;
                        selectedText = copiedLine = std::nullopt;
                        windowsManager->refreshAllWindows(currFrameVisitor);
                        activeMainWindow->setCursorFromRealPosition(
                            newCursorPos,
                            printingOrchestrator,
                            currFrameVisitor);
                        activeMainWindow->moveCursor(utils::Direction::ALIGN_TO_READ_MODE, linesLengthInformationVisitor, std::nullopt);
                        utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, activeMainWindow->getCursorRealPosition(printingOrchestrator.get()));
                        activeMainWindow->enableCursor();
                        setModeToReadMode(bottomBarWindow);
                        refreshProgressWindow();
                    }
                    catch (utils::exception::FeatherInterruptedException &e)
                    {
                        utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, cursorPos);
                        refreshProgressWindow();
                    }
                }
                */
                else if (utils::helpers::Lambda::isBracket(utf8Character))
                {
                    auto cursorPos = activeMainWindow->getCursorRealPosition(printingOrchestrator.get());
                    try
                    {
                        auto newPos = actionHandler->handle(std::make_unique<action::JumpToBracketAction>(action::JumpToBracketActionInput(activeMainWindow->getUUID(), activeMainWindow->getCursorRealPosition(printingOrchestrator.get()), activeMainWindow->getStorage(), printingOrchestrator, windowsManager, utf8Character))).newCursorPosition;
                        activeMainWindow->disableCursor();
                        if (newPos.has_value())
                        {
                            activeMainWindow->setCursorFromRealPosition(newPos.value(), printingOrchestrator, halfFrameUpVisitor);
                            activeMainWindow->moveCursor(utils::Direction::ALIGN_TO_READ_MODE, linesLengthInformationVisitor, std::nullopt);
                            utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, activeMainWindow->getCursorRealPosition(printingOrchestrator.get()));
                            highlightTextIfNeeded(activeMainWindow);
                        }
                        refreshProgressWindow();
                    }
                    catch (utils::exception::FeatherInterruptedException &e)
                    {
                        utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, cursorPos);
                        refreshProgressWindow();
                    }
                }
                else if (U'+' == utf8Character) // Agreement window
                {
                    if ((utils::FEATHER_MODE::READ_MODE != currentFeatherMode && utils::FEATHER_MODE::WRITE_MODE != currentFeatherMode) || !windowsManager->getAgreementWindow(activeMainWindow->getUUID()).has_value())
                    {
                        break;
                    }
                    auto prevFeatherMode = currentFeatherMode;
                    auto cursorPosition = activeMainWindow->getCursorRealPosition(printingOrchestrator.get());
                    activeMainWindow->disableCursor();
                    windowsManager->showWindow(windowsManager->getAgreementWindow(activeMainWindow->getUUID()).value(), currFrameVisitor);
                    currentFeatherMode = utils::FEATHER_MODE::READ_MODE_WAITING_FOR_INPUT;
                    getch();
                    currentFeatherMode = prevFeatherMode;
                    windowsManager->hideWindow(windowsManager->getAgreementWindow(activeMainWindow->getUUID()).value(), currFrameVisitor);
                    utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, cursorPosition);
                    highlightTextIfNeeded(activeMainWindow);
                    activeMainWindow->enableCursor();
                    break;
                }
                else if (utils::helpers::Lambda::isBackspace(utf8Character) && utils::FEATHER_MODE::READ_MODE_SELECT_TEXT == currentFeatherMode)
                {
                    if (shouldFrameBeRewindedBeforeDelete(selectedText->first))
                    {
                        windowsManager->refreshAllWindows(halfFrameUpVisitor);
                        activeMainWindow->setCursorFromRealPosition(utils::helpers::Lambda::alignToProperPosition(selectedText->first, activeMainWindow->getStorage(), printingOrchestrator, activeMainWindow->getUUID()), printingOrchestrator, halfFrameUpVisitor);
                    }
                    pair cursorPos = printingOrchestrator->removeCharacters(activeMainWindow->getUUID(), activeMainWindow->getBufferFiller()->getStorage(), selectedText->first, selectedText->second);
                    activeMainWindow->disableCursor();
                    windowsManager->refreshMainWindow(currFrameVisitor, activeMainWindow->getUUID());
                    activeMainWindow->setCursorFromRealPosition(utils::helpers::Lambda::alignToProperPosition(cursorPos, activeMainWindow->getStorage(), printingOrchestrator, activeMainWindow->getUUID()), printingOrchestrator, halfFrameUpVisitor);
                    bottomBarWindow->getParentWindowHandler()->moveCursor(utils::Direction::ALIGN_TO_READ_MODE, linesLengthInformationVisitor, std::nullopt);
                    utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, activeMainWindow->getCursorRealPosition(printingOrchestrator.get()));
                    setModeToReadMode(bottomBarWindow);
                    break;
                }
                else if (U'h' == utf8Character && utils::FEATHER_MODE::READ_MODE_SELECT_TEXT == currentFeatherMode)
                {
                    auto highlightedText = printingOrchestrator->highlightTextChunk(selectedText.value(), activeMainWindow->getUUID());
                    this->changesStack->addChange(
                        utils::datatypes::ChangeType::HIGHLIGHT,
                        activeMainWindow->getUUID(),
                        highlightedText.first.first,
                        highlightedText.first.second,
                        highlightedText);
                    setModeToReadMode(bottomBarWindow);
                    bottomBarWindow->getParentWindowHandler()->moveCursor(utils::Direction::ALIGN_TO_READ_MODE, linesLengthInformationVisitor, std::nullopt);
                    refreshProgressWindow();
                    break;
                }
                else if (ctrl(U'w') == utf8Character && (utils::FEATHER_MODE::READ_MODE == currentFeatherMode))
                {
                    lastCommand[activeMainWindow->getUUID()] = utils::datatypes::Command::REMOVE_WORD_BACKWARD;
                remove_word_backward_command:
                    auto cursorPos = activeMainWindow->getCursorRealPosition(printingOrchestrator.get());
                    try
                    {
                        activeMainWindow->disableCursor();
                        unhiglightSelectedText(activeMainWindow);
                        auto newCursorPosition = actionHandler->handle(std::make_unique<action::RemoveWordBackwardAction>(action::RemoveWordBackwardActionInput(activeMainWindow->getUUID(), windowsManager, cursorPos, activeMainWindow->getStorage(), printingOrchestrator))).getResult();
                        utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, newCursorPosition);
                        if (isCursorAtFirstLine())
                        {
                            activeMainWindow->disableCursor();
                            windowsManager->refreshAllWindows(halfFrameUpVisitor);
                        }
                        else
                        {
                            activeMainWindow->disableCursor();
                            windowsManager->refreshAllWindows(currFrameVisitor);
                        }
                        utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, newCursorPosition);
                        activeMainWindow->enableCursor();
                        refreshProgressWindow();
                    }
                    catch (utils::exception::FeatherInterruptedException &e)
                    {
                        auto firstValidPos = printingOrchestrator->getNextVirtualPosition(activeMainWindow->getUUID(), activeMainWindow->getStorage(), cursorPos);
                        utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, firstValidPos);
                        refreshProgressWindow();
                    }
                    break;
                }
                else if (U'u' == utf8Character && (utils::FEATHER_MODE::READ_MODE == currentFeatherMode)) // Undo command
                {
                    unhiglightSelectedText(activeMainWindow);
                    auto result = changesStack->undoChange(activeMainWindow->getUUID());
                    if (!result.has_value())
                    {
                        auto const currCursorPos = activeMainWindow->getCursorRealPosition(printingOrchestrator.get());
                        activeMainWindow->disableCursor();
                        if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastUndoRedoPressed) > std::chrono::milliseconds(1000))
                        {
                            utils::helpers::Lambda::blinkPrint(utils::datatypes::Strings::NOTHING_TO_UNDO, utils::datatypes::Strings::READ_MODE, ONE_SECOND, bottomBarWindow);
                        }
                        lastUndoRedoPressed = std::chrono::steady_clock::now();
                        refresh(activeMainWindow, true, currCursorPos);
                        break;
                    }
                    // activeMainWindow->disableCursor();
                    if (utils::datatypes::ChangeType::DELETE == result->changeType)
                    {
                        for (auto const &e : result->ranges)
                        {
                            printingOrchestrator->undelete(e.first, e.second, activeMainWindow->getUUID());
                        }
                    }
                    else if (utils::datatypes::ChangeType::INSERT == result->changeType)
                    {
                        printingOrchestrator->applyDeletions(utils::helpers::Lambda::mergeIntervals(printingOrchestrator->getDeletionsPositions(activeMainWindow->getUUID()), result->ranges), activeMainWindow->getUUID());
                    }
                    else if (utils::datatypes::ChangeType::HIGHLIGHT == result->changeType)
                    {
                        printingOrchestrator->unhighlightSelection(result->selectedText.value(), activeMainWindow->getUUID());
                    }
                    utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, result->undoPosition);
                    refresh(activeMainWindow, true, result->undoPosition);
                    break;
                }
                else if (ctrl(U'r') == utf8Character && (utils::FEATHER_MODE::READ_MODE == currentFeatherMode)) // Redo command
                {
                    unhiglightSelectedText(activeMainWindow);
                    auto result = changesStack->redoChange(activeMainWindow->getUUID());
                    if (!result.has_value())
                    {
                        auto const currCursorPos = activeMainWindow->getCursorRealPosition(printingOrchestrator.get());
                        activeMainWindow->disableCursor();
                        if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastUndoRedoPressed) > std::chrono::milliseconds(1000))
                        {
                            utils::helpers::Lambda::blinkPrint(utils::datatypes::Strings::NOTHING_TO_REDO, utils::datatypes::Strings::READ_MODE, ONE_SECOND, bottomBarWindow);
                        }
                        lastUndoRedoPressed = std::chrono::steady_clock::now();
                        refresh(activeMainWindow, true, currCursorPos);
                        break;
                    }
                    // activeMainWindow->disableCursor();
                    if (utils::datatypes::ChangeType::DELETE == result->changeType)
                    {
                        printingOrchestrator->applyDeletions(utils::helpers::Lambda::mergeIntervals(printingOrchestrator->getDeletionsPositions(activeMainWindow->getUUID()), result->ranges), activeMainWindow->getUUID());
                    }
                    else if (utils::datatypes::ChangeType::INSERT == result->changeType)
                    {
                        for (auto range : result->ranges)
                        {
                            printingOrchestrator->undelete(range.first, range.second, activeMainWindow->getUUID());
                        }
                    }
                    else if (utils::datatypes::ChangeType::HIGHLIGHT == result->changeType)
                    {
                        printingOrchestrator->highlightSelection(result->selectedText.value(), activeMainWindow->getUUID());
                    }
                    utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, result->redoPosition);
                    refresh(activeMainWindow, true, result->redoPosition);
                }
                else if (ctrl(U'b') == utf8Character && (utils::FEATHER_MODE::READ_MODE == currentFeatherMode)) // Add break point
                {
                    lastCommand[activeMainWindow->getUUID()] = utils::datatypes::Command::BREAKPOINT_ADD;
                add_breakpoint_command:
                    printingOrchestrator->unhighlightSelection(activeMainWindow->getUUID());
                    addBreakpoint(activeMainWindow, bottomBarWindow);
                }
                else if (ctrl(U'p') == utf8Character && (utils::FEATHER_MODE::READ_MODE == currentFeatherMode)) // Goto previous breakpoint
                {
                    lastCommand[activeMainWindow->getUUID()] = utils::datatypes::Command::BREAKPOINT_PREVIOUS;
                previous_breakpoint_command:
                    printingOrchestrator->unhighlightSelection(activeMainWindow->getUUID());
                    previousBreakpoint(activeMainWindow, bottomBarWindow);
                }
                else if (ctrl(U'n') == utf8Character && (utils::FEATHER_MODE::READ_MODE == currentFeatherMode)) // Goto next breakpoint
                {
                    lastCommand[activeMainWindow->getUUID()] = utils::datatypes::Command::BREAKPOINT_NEXT;
                next_breakpoint_command:
                    printingOrchestrator->unhighlightSelection(activeMainWindow->getUUID());
                    nextBreakpoint(activeMainWindow, bottomBarWindow);
                }
                else if (U'i' == utf8Character && (utils::FEATHER_MODE::READ_MODE == currentFeatherMode)) // Switch to WRITE MODE
                {
                    lastCommand[activeMainWindow->getUUID()] = utils::datatypes::Command::NONE;
                    unhiglightSelectedText(activeMainWindow);
                    pair cursorPos = activeMainWindow->getCursorRealPosition(printingOrchestrator.get());
                    activeMainWindow->disableCursor();
                    setModeToWriteMode(bottomBarWindow);
                    refreshInfoWindow(currFrameVisitor);
                    utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, cursorPos);
                    refreshProgressWindow();
                    break;
                }
                else if (U'O' == utf8Character && (utils::FEATHER_MODE::READ_MODE == currentFeatherMode))
                {
                    unhiglightSelectedText(activeMainWindow);
                    auto cursorPos = activeMainWindow->getCursorRealPosition(printingOrchestrator.get());
                    try
                    {
                        auto result = actionHandler->handle(std::make_unique<action::EditLineAboveAction>(action::EditLineAboveActionInput(activeMainWindow->getUUID(), activeMainWindow->getCursorRealPosition(printingOrchestrator.get()), activeMainWindow->getStorage(), printingOrchestrator, this, windowsManager)));
                        activeMainWindow->disableCursor();
                        setModeToWriteMode(bottomBarWindow);
                        activeMainWindow->getBufferFiller()->getScreenBuffer().unhighlightBrackets();
                        utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, result.getResult());
                        windowsManager->refreshAllWindows(currFrameVisitor);
                        utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, result.getResult());
                        refreshProgressWindow();
                        emptySpacesToDiscard = std::make_pair(result.positionToSkip.first, std::make_pair(result.positionToSkip.first.first, result.positionToSkip.first.second + result.positionToSkip.second - 1UL));
                    }
                    catch (utils::exception::FeatherInterruptedException &e)
                    {
                        utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, cursorPos);
                        refreshProgressWindow();
                    }
                    break;
                }
                else if (U'o' == utf8Character && (utils::FEATHER_MODE::READ_MODE == currentFeatherMode))
                {
                    unhiglightSelectedText(activeMainWindow);
                    auto cursorPos = activeMainWindow->getCursorRealPosition(printingOrchestrator.get());
                    try
                    {
                        auto result = actionHandler->handle(std::make_unique<action::EditLineBelowAction>(action::EditLineBelowActionInput(activeMainWindow->getUUID(), activeMainWindow->getCursorRealPosition(printingOrchestrator.get()), activeMainWindow->getStorage(), printingOrchestrator, this, windowsManager)));
                        activeMainWindow->disableCursor();
                        setModeToWriteMode(bottomBarWindow);
                        activeMainWindow->getBufferFiller()->getScreenBuffer().unhighlightBrackets();
                        utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, result.getResult());
                        windowsManager->refreshAllWindows(currFrameVisitor);
                        utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, result.getResult());
                        refreshProgressWindow();
                        emptySpacesToDiscard = std::make_pair(result.positionToSkip.first, std::make_pair(result.positionToSkip.first.first, result.positionToSkip.first.second + result.positionToSkip.second - 1UL));
                    }
                    catch (utils::exception::FeatherInterruptedException &e)
                    {
                        utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, cursorPos);
                        refreshProgressWindow();
                    }
                    break;
                }
                else if (U'v' == utf8Character) // select text
                {
                    if (utils::FEATHER_MODE::READ_MODE_SELECT_TEXT != currentFeatherMode)
                    {
                        selectedText = std::make_pair(activeMainWindow->getCursorRealPosition(printingOrchestrator.get()), activeMainWindow->getCursorRealPosition(printingOrchestrator.get()));
                        currentFeatherMode = utils::FEATHER_MODE::READ_MODE_SELECT_TEXT;
                    }
                    else
                    {
                        setModeToReadMode(bottomBarWindow);
                    }
                    break;
                }
                else if (U'w' == utf8Character) // next word
                {
                    lastCommand[activeMainWindow->getUUID()] = utils::datatypes::Command::GOTO_NEXT_WORD;
                next_word_command:
                    auto cursorPos = activeMainWindow->getCursorRealPosition(printingOrchestrator.get());
                    try
                    {
                        auto cursorPos = actionHandler->handle(std::make_unique<action::MoveToNextWordAction>(action::MoveToNextWordActionInput(activeMainWindow->getUUID(), activeMainWindow->getCursorRealPosition(printingOrchestrator.get()), activeMainWindow->getStorage(), printingOrchestrator, windowsManager))).getResult();
                        activeMainWindow->disableCursor();
                        activeMainWindow->setCursorFromRealPosition(cursorPos, printingOrchestrator, halfFrameUpVisitor);
                        activeMainWindow->moveCursor(utils::Direction::ALIGN_TO_READ_MODE, linesLengthInformationVisitor, std::nullopt);
                        utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, activeMainWindow->getCursorRealPosition(printingOrchestrator.get()));
                        highlightTextIfNeeded(activeMainWindow);
                        refreshProgressWindow();
                    }
                    catch (utils::exception::FeatherInterruptedException &e)
                    {
                        utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, cursorPos);
                        refreshProgressWindow();
                    }
                    break;
                }
                else if (U'b' == utf8Character) // prev word
                {
                    lastCommand[activeMainWindow->getUUID()] = utils::datatypes::Command::GOTO_PREVIOUS_WORD;
                previous_word_command:
                    auto cursorPos = activeMainWindow->getCursorRealPosition(printingOrchestrator.get());
                    try
                    {
                        auto cursorPos = actionHandler->handle(std::make_unique<action::MoveToPreviousWordAction>(action::MoveToPreviousWordActionInput(activeMainWindow, printingOrchestrator, windowsManager))).getResult();
                        activeMainWindow->disableCursor();
                        activeMainWindow->setCursorFromRealPosition(cursorPos, printingOrchestrator, halfFrameUpVisitor);
                        activeMainWindow->moveCursor(utils::Direction::ALIGN_TO_READ_MODE, linesLengthInformationVisitor, std::nullopt);
                        utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, activeMainWindow->getCursorRealPosition(printingOrchestrator.get()));
                        highlightTextIfNeeded(activeMainWindow);
                        refreshProgressWindow();
                    }
                    catch (utils::exception::FeatherInterruptedException &e)
                    {
                        utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, cursorPos);
                        refreshProgressWindow();
                    }
                }
                else if (U'0' == utf8Character) // line begin
                {
                    lastCommand[activeMainWindow->getUUID()] = utils::datatypes::Command::GOTO_LINE_BEGIN;
                line_begin_action:
                    auto cursorPos = activeMainWindow->getCursorRealPosition(printingOrchestrator.get());
                    try
                    {
                        auto cursorPos = actionHandler->handle(std::make_unique<action::MoveToLineBeginAction>(action::MoveToLineBeginActionInput(activeMainWindow->getUUID(), activeMainWindow->getCursorRealPosition(printingOrchestrator.get()), activeMainWindow->getStorage(), printingOrchestrator, windowsManager))).getResult();
                        activeMainWindow->disableCursor();
                        activeMainWindow->setCursorFromRealPosition(cursorPos, printingOrchestrator, halfFrameUpVisitor);
                        utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, activeMainWindow->getCursorRealPosition(printingOrchestrator.get()));
                        highlightTextIfNeeded(activeMainWindow);
                        refreshProgressWindow();
                    }
                    catch (utils::exception::FeatherInterruptedException &e)
                    {
                        utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, cursorPos);
                        refreshProgressWindow();
                    }
                }
                else if (U'$' == utf8Character) // line end
                {
                    lastCommand[activeMainWindow->getUUID()] = utils::datatypes::Command::GOTO_LINE_END;
                line_end_action:
                    auto cursorPos = activeMainWindow->getCursorRealPosition(printingOrchestrator.get());
                    try
                    {
                        auto cursorPos = actionHandler->handle(std::make_unique<action::MoveToLineEndAction>(action::MoveToLineEndActionInput(activeMainWindow->getUUID(), activeMainWindow->getCursorRealPosition(printingOrchestrator.get()), activeMainWindow->getStorage(), printingOrchestrator, windowsManager))).getResult();
                        activeMainWindow->disableCursor();
                        utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, cursorPos);
                        highlightTextIfNeeded(activeMainWindow);
                        refreshProgressWindow();
                    }
                    catch (utils::exception::FeatherInterruptedException &e)
                    {
                        utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, cursorPos);
                        refreshProgressWindow();
                    }
                }
                else if (U'A' == utf8Character && (utils::FEATHER_MODE::READ_MODE == currentFeatherMode)) // line end with edit
                {
                    auto cursorPos = activeMainWindow->getCursorRealPosition(printingOrchestrator.get());
                    try
                    {
                        auto cursorPos = actionHandler->handle(std::make_unique<action::MoveToLineEndWithEditAction>(action::MoveToLineEndWithEditActionInput(activeMainWindow->getUUID(), activeMainWindow->getCursorRealPosition(printingOrchestrator.get()), activeMainWindow->getStorage(), printingOrchestrator, windowsManager))).getResult();
                        unhiglightSelectedText(activeMainWindow);
                        activeMainWindow->disableCursor();
                        utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, cursorPos);
                        setModeToWriteMode(bottomBarWindow);
                        refreshInfoWindow(currFrameVisitor);
                        activeMainWindow->setCursorFromRealPosition(cursorPos, printingOrchestrator, halfFrameUpVisitor);
                        refreshProgressWindow();
                    }
                    catch (utils::exception::FeatherInterruptedException &e)
                    {
                        utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, cursorPos);
                        refreshProgressWindow();
                    }
                    break;
                }
                else if (U'a' == utf8Character && (utils::FEATHER_MODE::READ_MODE == currentFeatherMode)) // edit at next character
                {
                    unhiglightSelectedText(activeMainWindow);
                    auto cursorPos = actionHandler->handle(std::make_unique<action::EditNextCharacterAction>(action::EditNextCharacterActionInput(activeMainWindow->getUUID(), activeMainWindow->getCursorRealPosition(printingOrchestrator.get()), activeMainWindow->getStorage(), printingOrchestrator))).getResult();
                    activeMainWindow->disableCursor();
                    utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, cursorPos);
                    setModeToWriteMode(bottomBarWindow);
                    refreshInfoWindow(currFrameVisitor);
                    activeMainWindow->setCursorFromRealPosition(cursorPos, printingOrchestrator, halfFrameUpVisitor);
                    refreshProgressWindow();
                }
                else if (U'D' == utf8Character && (utils::FEATHER_MODE::READ_MODE == currentFeatherMode)) // remove until line end
                {
                    lastCommand[activeMainWindow->getUUID()] = utils::datatypes::Command::REMOVE_UNTIL_LINE_END;
                remove_until_line_end_action:
                    unhiglightSelectedText(activeMainWindow);
                    if (isCursorAtFirstLine())
                    {
                        pair cursorPos = activeMainWindow->getCursorRealPosition(printingOrchestrator.get());
                        activeMainWindow->disableCursor();
                        windowsManager->refreshAllWindows(halfFrameUpVisitor);
                        utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, cursorPos);
                        activeMainWindow->enableCursor();
                    }
                    auto cursorPos = activeMainWindow->getCursorRealPosition(printingOrchestrator.get());
                    try
                    {
                        activeMainWindow->disableCursor();
                        auto newCursorPos = actionHandler->handle(std::make_unique<action::RemoveUntilLineEndAction>(action::RemoveUntilLineEndActionInput(activeMainWindow->getUUID(), windowsManager, cursorPos, activeMainWindow->getStorage(), printingOrchestrator))).getResult();
                        windowsManager->refreshMainWindow(currFrameVisitor, activeMainWindow->getUUID());
                        activeMainWindow->setCursorFromRealPosition(newCursorPos, printingOrchestrator, halfFrameUpVisitor);
                        activeMainWindow->enableCursor();
                        activeMainWindow->moveCursor(utils::Direction::ALIGN_TO_READ_MODE, linesLengthInformationVisitor, std::nullopt);
                        utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, activeMainWindow->getCursorRealPosition(printingOrchestrator.get()));
                        refreshProgressWindow();
                    }
                    catch (utils::exception::FeatherInterruptedException &e)
                    {
                        auto firstValidPos = printingOrchestrator->getNextVirtualPosition(activeMainWindow->getUUID(), activeMainWindow->getStorage(), cursorPos);
                        utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, firstValidPos);
                        refreshProgressWindow();
                    }
                }
                else if (U'g' == utf8Character) // begining of file
                {
                    lastCommand[activeMainWindow->getUUID()] = utils::datatypes::Command::GOTO_FILE_BEGIN;
                file_begin_action:
                    auto cursorPos = actionHandler->handle(std::make_unique<action::MoveToFileBeginAction>(action::MoveToFileBeginActionInput(activeMainWindow, printingOrchestrator))).getResult();
                    activeMainWindow->disableCursor();
                    utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, cursorPos);
                    highlightTextIfNeeded(activeMainWindow);
                    refreshProgressWindow();
                    break;
                }
                else if (U'G' == utf8Character) // end of file
                {
                    lastCommand[activeMainWindow->getUUID()] = utils::datatypes::Command::GOTO_FILE_END;
                file_end_action:
                    auto cursorPos = actionHandler->handle(std::make_unique<action::MoveToFileEndAction>(action::MoveToFileEndActionInput(activeMainWindow, printingOrchestrator))).getResult();
                    activeMainWindow->disableCursor();
                    utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, cursorPos);
                    highlightTextIfNeeded(activeMainWindow);
                    refreshProgressWindow();
                    break;
                }
                else if (U'x' == utf8Character && (utils::FEATHER_MODE::READ_MODE == currentFeatherMode)) // remove current character
                {
                    lastCommand[activeMainWindow->getUUID()] = utils::datatypes::Command::REMOVE_SINGLE_CHAR;
                remove_single_char_action:
                    unhiglightSelectedText(activeMainWindow);
                    pair cursorPos = activeMainWindow->getCursorRealPosition(printingOrchestrator.get());
                    rewindFrameIfNeeded(cursorPos, activeMainWindow);
                    auto result = actionHandler->handle(std::make_unique<action::RemoveCharacterUnderCursorAction>(action::RemoveCharacterUnderCursorActionInput(activeMainWindow->getUUID(), activeMainWindow->getCursorRealPosition(printingOrchestrator.get()), activeMainWindow->getStorage(), printingOrchestrator, windowsManager))).getResult();
                    // activeMainWindow->disableCursor();
                    refresh(activeMainWindow, true, result);
                    break;
                }
                else if (U'z' == utf8Character) // align to middle
                {
                    actionType = waitForSecondKey(activeMainWindow, bottomBarWindow, utils::datatypes::ChangeType::OTHER);
                    if (action::ACTION_TYPE::ALIGN_TO_MIDDLE == actionType.first)
                    {
                        auto cursorPos = actionHandler->handle(std::make_unique<action::AlignToMiddleOfScreenAction>(action::AlignToMiddleOfScreenActionInput(activeMainWindow, printingOrchestrator, windowsManager, lineUpVisitor, lineDownVisitor))).getResult();
                        utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, cursorPos);
                        activeMainWindow->enableCursor();
                        highlightTextIfNeeded(activeMainWindow);
                        refreshProgressWindow();
                    }
                    break;
                }
                else if (U'h' == utf8Character) // Highlight
                {
                    unhiglightSelectedText(activeMainWindow);
                    actionType = waitForSecondKey(activeMainWindow, bottomBarWindow, utils::datatypes::ChangeType::HIGHLIGHT);
                    if (action::ACTION_TYPE::HIGHLIGHT_LINE == actionType.first)
                    {
                        lastCommand[activeMainWindow->getUUID()] = utils::datatypes::Command::HIGHLIGHT_LINE;
                    highlight_line_command:
                        auto currCursorPos = activeMainWindow->getCursorRealPosition(printingOrchestrator.get());
                        activeMainWindow->disableCursor();
                        selectedText = std::nullopt;
                        try
                        {
                            selectedText = actionHandler->handle(std::make_unique<action::CopyLineAction>(action::CopyLineActionInput(activeMainWindow->getUUID(), currCursorPos, activeMainWindow->getStorage(), printingOrchestrator, windowsManager))).getResult();
                            auto highlightColor = printingOrchestrator->highlightSelection(selectedText.value(), activeMainWindow->getUUID());
                            auto highlightedText = printingOrchestrator->highlightTextChunk(selectedText.value(), activeMainWindow->getUUID());
                            this->changesStack->addChange(
                                utils::datatypes::ChangeType::HIGHLIGHT,
                                activeMainWindow->getUUID(),
                                currCursorPos,
                                currCursorPos,
                                highlightedText);
                            setModeToReadMode(bottomBarWindow);
                            bottomBarWindow->getParentWindowHandler()->moveCursor(utils::Direction::ALIGN_TO_READ_MODE, linesLengthInformationVisitor, std::nullopt);
                            refreshProgressWindow();
                        }
                        catch (utils::exception::FeatherInterruptedException &e)
                        {
                            utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, currCursorPos);
                            refreshProgressWindow();
                        }
                        break;
                    }
                    else if (action::ACTION_TYPE::HIGHLIGHT_BETWEEN_BRACKETS == actionType.first)
                    {
                        lastCommand[activeMainWindow->getUUID()] = utils::datatypes::Command::HIGHLIGHT_BETWEEN_BRACKETS;
                    highlight_between_brackets_command:
                        auto currCursorPos = activeMainWindow->getCursorRealPosition(printingOrchestrator.get());
                        try
                        {
                            selectedText = std::nullopt;
                            selectedText = actionHandler->handle(std::make_unique<action::HighlightBetweenBracketsAction>(action::HighlightBetweenBracketsActionInput(activeMainWindow->getUUID(), windowsManager, currCursorPos, actionType.second, activeMainWindow->getStorage(), printingOrchestrator))).cursorPos;
                            if (selectedText.has_value())
                            {
                                activeMainWindow->disableCursor();
                                auto highlightColor = printingOrchestrator->highlightSelection(selectedText.value(), activeMainWindow->getUUID());
                                auto highlightedText = printingOrchestrator->highlightTextChunk(selectedText.value(), activeMainWindow->getUUID());
                                this->changesStack->addChange(
                                    utils::datatypes::ChangeType::HIGHLIGHT,
                                    activeMainWindow->getUUID(),
                                    currCursorPos,
                                    currCursorPos,
                                    highlightedText);
                                setModeToReadMode(bottomBarWindow);
                                bottomBarWindow->getParentWindowHandler()->moveCursor(utils::Direction::ALIGN_TO_READ_MODE, linesLengthInformationVisitor, std::nullopt);
                                refreshProgressWindow();
                            }
                        }
                        catch (utils::exception::FeatherInterruptedException &e)
                        {
                            utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, currCursorPos);
                            refreshProgressWindow();
                        }
                        break;
                    }
                }
                else if (U'y' == utf8Character) // Copy
                {
                    if (utils::FEATHER_MODE::READ_MODE_SELECT_TEXT == currentFeatherMode)
                    {
                        if (selectedText->first > selectedText->second)
                        {
                            std::swap(selectedText->first, selectedText->second);
                        }
                        printingOrchestrator->setLineToCopy(std::nullopt, std::nullopt);
                        printingOrchestrator->setTextToCut(std::nullopt, std::nullopt);
                        printingOrchestrator->setTextToCopy(std::make_pair(std::make_pair(activeMainWindow->getUUID(), activeMainWindow->getStorage()->getAbsolutePath()), selectedText.value()), std::make_pair(std::make_pair(activeMainWindow->getUUID(), utils::helpers::Lambda::getAbsolutePath(activeMainWindow->getStorage()->getName(), U"", filesystem)), utils::datatypes::DeletionsSnapshot{printingOrchestrator->getTotalBytesOfDeletions(activeMainWindow->getUUID()), printingOrchestrator->getDeletionsPositions(activeMainWindow->getUUID()), printingOrchestrator->getReverseDeletionsPositions(activeMainWindow->getUUID())}));
                        selectedText = std::nullopt;
                        auto currCursorPos = activeMainWindow->getCursorRealPosition(printingOrchestrator.get());
                        activeMainWindow->disableCursor();
                        windowsManager->refreshAllWindows(currFrameVisitor);
                        activeMainWindow->setCursorFromRealPosition(
                            currCursorPos,
                            printingOrchestrator,
                            currFrameVisitor);
                        activeMainWindow->moveCursor(utils::Direction::ALIGN_TO_READ_MODE, linesLengthInformationVisitor, std::nullopt);
                        setModeToReadMode(bottomBarWindow);
                        utils::helpers::Lambda::subWindowPrint(utils::datatypes::Strings::COPIED_TEXT, std::nullopt, bottomBarWindow, windowsManager->getSubWindow(windowsManager->getProgressBarWindow(bottomBarWindow->getParentWindowHandler()->getUUID()).value()), currFrameVisitor);
                        utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, activeMainWindow->getCursorRealPosition(printingOrchestrator.get()));
                        activeMainWindow->enableCursor();
                        refreshProgressWindow();
                    }
                    else
                    {
                        lastCommand[activeMainWindow->getUUID()] = utils::datatypes::Command::COPY_LINE;
                    copy_line_command:
                        unhiglightSelectedText(activeMainWindow);
                        actionType = waitForSecondKey(activeMainWindow, bottomBarWindow, utils::datatypes::ChangeType::COPY);
                        if (action::ACTION_TYPE::COPY_LINE_ACTION == actionType.first)
                        {
                            auto cursorPos = activeMainWindow->getCursorRealPosition(printingOrchestrator.get());
                            activeMainWindow->disableCursor();
                            selectedText = std::nullopt;
                            printingOrchestrator->setTextToCopy(std::nullopt, std::nullopt);
                            printingOrchestrator->setTextToCut(std::nullopt, std::nullopt);
                            try
                            {
                                auto result = actionHandler->handle(std::make_unique<action::CopyLineAction>(action::CopyLineActionInput(activeMainWindow->getUUID(), cursorPos, activeMainWindow->getStorage(), printingOrchestrator, windowsManager))).getResult();
                                if (result.has_value())
                                {
                                    printingOrchestrator->setLineToCopy(std::make_pair(std::make_pair(activeMainWindow->getUUID(), activeMainWindow->getStorage()->getName()), result.value()), std::make_pair(std::make_pair(activeMainWindow->getUUID(), utils::helpers::Lambda::getAbsolutePath(activeMainWindow->getStorage()->getName(), U"", filesystem)), utils::datatypes::DeletionsSnapshot{printingOrchestrator->getTotalBytesOfDeletions(activeMainWindow->getUUID()), printingOrchestrator->getDeletionsPositions(activeMainWindow->getUUID()), printingOrchestrator->getReverseDeletionsPositions(activeMainWindow->getUUID())}));
                                }
                                else
                                {
                                    printingOrchestrator->setLineToCopy(std::nullopt, std::nullopt);
                                }
                                activeMainWindow->enableCursor();
                                windowsManager->refreshAllWindows(currFrameVisitor);
                                if (result.has_value())
                                {
                                    utils::helpers::Lambda::subWindowPrint(utils::datatypes::Strings::COPIED_LINE, std::nullopt, bottomBarWindow, windowsManager->getSubWindow(windowsManager->getProgressBarWindow(bottomBarWindow->getParentWindowHandler()->getUUID()).value()), currFrameVisitor);
                                }
                                utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, cursorPos);
                                refreshProgressWindow();
                            }
                            catch (utils::exception::FeatherInterruptedException &e)
                            {
                                utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, cursorPos);
                                refreshProgressWindow();
                            }
                        }
                    }
                    printingOrchestrator->setTextToCopyPositionsToSkip(std::make_pair(std::make_pair(activeMainWindow->getUUID(), activeMainWindow->getStorage()->getName()), std::map<pair, pair>{}));
                    break;
                }
                else if (U'x' == utf8Character && utils::FEATHER_MODE::READ_MODE_SELECT_TEXT == currentFeatherMode) // Cut
                {
                    if (selectedText->first > selectedText->second)
                    {
                        std::swap(selectedText->first, selectedText->second);
                    }
                    printingOrchestrator->setLineToCopy(std::nullopt, std::nullopt);
                    printingOrchestrator->setTextToCopy(std::nullopt, std::nullopt);
                    printingOrchestrator->setTextToCut(std::make_pair(std::make_pair(activeMainWindow->getUUID(), utils::helpers::Lambda::getAbsolutePath(activeMainWindow->getStorage()->getName(), U"", filesystem)), selectedText.value()), std::make_pair(std::make_pair(activeMainWindow->getUUID(), utils::helpers::Lambda::getAbsolutePath(activeMainWindow->getStorage()->getName(), U"", filesystem)), utils::datatypes::DeletionsSnapshot{printingOrchestrator->getTotalBytesOfDeletions(activeMainWindow->getUUID()), printingOrchestrator->getDeletionsPositions(activeMainWindow->getUUID()), printingOrchestrator->getReverseDeletionsPositions(activeMainWindow->getUUID())}));
                    pair newCursorPos = printingOrchestrator->removeCharacters(activeMainWindow->getUUID(), activeMainWindow->getStorage(), printingOrchestrator->getTextToCut()->second.first, printingOrchestrator->getTextToCut()->second.second);
                    selectedText = std::nullopt;
                    activeMainWindow->disableCursor();
                    windowsManager->refreshAllWindows(currFrameVisitor);
                    utils::helpers::Lambda::subWindowPrint(utils::datatypes::Strings::CUT_TEXT, std::nullopt, bottomBarWindow, windowsManager->getSubWindow(windowsManager->getProgressBarWindow(bottomBarWindow->getParentWindowHandler()->getUUID()).value()), currFrameVisitor);
                    activeMainWindow->setCursorFromRealPosition(
                        newCursorPos,
                        printingOrchestrator,
                        currFrameVisitor);
                    setModeToReadMode(bottomBarWindow);
                    activeMainWindow->moveCursor(utils::Direction::ALIGN_TO_READ_MODE, linesLengthInformationVisitor, std::nullopt);
                    utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, activeMainWindow->getCursorRealPosition(printingOrchestrator.get()));
                    activeMainWindow->enableCursor();
                    refreshProgressWindow();
                    printingOrchestrator->setTextToCopyPositionsToSkip(std::make_pair(std::make_pair(activeMainWindow->getUUID(), activeMainWindow->getStorage()->getName()), std::map<pair, pair>{}));
                    break;
                }
                else if (U'p' == utf8Character && (utils::FEATHER_MODE::READ_MODE == currentFeatherMode)) // Paste
                {
                    lastCommand[activeMainWindow->getUUID()] = utils::datatypes::Command::PASTE;
                paste_command:
                    if (printingOrchestrator->isLineCopied())
                    {
                        auto savedFeatherMode = currentFeatherMode;
                        auto nextNewLine = utils::helpers::Lambda::findNext(activeMainWindow->getCursorRealPosition(printingOrchestrator.get()), utils::helpers::Lambda::isNewLineChar, printingOrchestrator, activeMainWindow->getUUID(), activeMainWindow->getStorage(), windowsManager, false, true);
                        utils::helpers::Lambda::restoreFeatherMode(windowsManager, currFrameVisitor, savedFeatherMode);
                        pair currCursorPos = nextNewLine.first;
                        // Only move to next char if did not reached last character
                        if (nextNewLine.second.has_value() && utils::helpers::Lambda::isNewLineChar(nextNewLine.second.value()))
                        {
                            currCursorPos = printingOrchestrator->getNextVirtualPosition(activeMainWindow->getUUID(), activeMainWindow->getStorage(), currCursorPos);
                        }
                        activeMainWindow->disableCursor();
                        auto result = actionHandler->handle(std::make_unique<action::PasteAction>(action::PasteActionInput(activeMainWindow->getUUID(), printingOrchestrator->getLineToCopy().value(), storageFactory, currCursorPos, activeMainWindow->getStorage(), printingOrchestrator, printingOrchestrator->getTextToCopyPositionsToSkip().value(), breakPoints[activeMainWindow->getUUID()], currentBreakPoint[activeMainWindow->getUUID()], printingOrchestrator->getTextToCutSnapshot(), breakPointsAdded[activeMainWindow->getUUID()], windowsManager, currFrameVisitor, true)));
                        printingOrchestrator->setLineToCopy(printingOrchestrator->getLineToCopy(), std::make_pair(printingOrchestrator->getLineToCopy()->first, result.deletionsSnapshot));
                        windowsManager->refreshAllWindows(currFrameVisitor);
                        activeMainWindow->setCursorFromRealPosition(
                            result.result,
                            printingOrchestrator,
                            halfFrameUpVisitor);
                        activeMainWindow->moveCursor(utils::Direction::ALIGN_TO_READ_MODE, linesLengthInformationVisitor, std::nullopt);
                        utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, activeMainWindow->getCursorRealPosition(printingOrchestrator.get()));
                        refresh(activeMainWindow, true, result.result);
                        printingOrchestrator->setTextToCopyPositionsToSkip(result.positionsToSkipUpdated);
                        breakPointsAdded[activeMainWindow->getUUID()] = result.addedBreakPointsUpdated;
                        activeMainWindow->enableCursor();
                    }
                    else if (printingOrchestrator->isTextCopied())
                    {
                        activeMainWindow->disableCursor();
                        utils::helpers::Lambda::subWindowPrint(utils::datatypes::Strings::WORKING_STRING, std::nullopt, bottomBarWindow, windowsManager->getSubWindow(windowsManager->getProgressBarWindow(activeMainWindow->getUUID()).value()), currFrameVisitor);
                        auto result = actionHandler->handle(std::make_unique<action::PasteAction>(action::PasteActionInput(activeMainWindow->getUUID(), printingOrchestrator->getTextToCopy().value(), storageFactory, activeMainWindow->getCursorRealPosition(printingOrchestrator.get()), activeMainWindow->getStorage(), printingOrchestrator, printingOrchestrator->getTextToCopyPositionsToSkip().value(), breakPoints[activeMainWindow->getUUID()], currentBreakPoint[activeMainWindow->getUUID()], printingOrchestrator->getTextToCutSnapshot(), breakPointsAdded[activeMainWindow->getUUID()], windowsManager, currFrameVisitor, false)));
                        printingOrchestrator->setTextToCopy(printingOrchestrator->getTextToCopy(), std::make_pair(printingOrchestrator->getTextToCopy()->first, result.deletionsSnapshot));
                        windowsManager->refreshAllWindows(currFrameVisitor);
                        activeMainWindow->setCursorFromRealPosition(
                            result.result,
                            printingOrchestrator,
                            halfFrameUpVisitor);
                        activeMainWindow->moveCursor(utils::Direction::ALIGN_TO_READ_MODE, linesLengthInformationVisitor, std::nullopt);
                        utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, activeMainWindow->getCursorRealPosition(printingOrchestrator.get()));
                        refresh(activeMainWindow, true, result.result);
                        printingOrchestrator->setTextToCopyPositionsToSkip(result.positionsToSkipUpdated);
                        breakPointsAdded[activeMainWindow->getUUID()] = result.addedBreakPointsUpdated;
                    }
                    else if (printingOrchestrator->isTextCut())
                    {
                        activeMainWindow->disableCursor();
                        utils::helpers::Lambda::subWindowPrint(utils::datatypes::Strings::WORKING_STRING, std::nullopt, bottomBarWindow, windowsManager->getSubWindow(windowsManager->getProgressBarWindow(activeMainWindow->getUUID()).value()), currFrameVisitor);
                        auto result = actionHandler->handle(std::make_unique<action::PasteAction>(action::PasteActionInput(activeMainWindow->getUUID(), printingOrchestrator->getTextToCut().value(), storageFactory, activeMainWindow->getCursorRealPosition(printingOrchestrator.get()), activeMainWindow->getStorage(), printingOrchestrator, printingOrchestrator->getTextToCopyPositionsToSkip().value(), breakPoints[activeMainWindow->getUUID()], currentBreakPoint[activeMainWindow->getUUID()], printingOrchestrator->getTextToCutSnapshot(), breakPointsAdded[activeMainWindow->getUUID()], windowsManager, currFrameVisitor, false)));
                        printingOrchestrator->setTextToCut(printingOrchestrator->getTextToCut(), std::make_pair(printingOrchestrator->getTextToCut()->first, result.deletionsSnapshot));
                        windowsManager->refreshAllWindows(currFrameVisitor);
                        activeMainWindow->setCursorFromRealPosition(
                            result.result,
                            printingOrchestrator,
                            halfFrameUpVisitor);
                        activeMainWindow->moveCursor(utils::Direction::ALIGN_TO_READ_MODE, linesLengthInformationVisitor, std::nullopt);
                        utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, activeMainWindow->getCursorRealPosition(printingOrchestrator.get()));
                        refresh(activeMainWindow, true, result.result);
                        printingOrchestrator->setTextToCopyPositionsToSkip(result.positionsToSkipUpdated);
                        breakPointsAdded[activeMainWindow->getUUID()] = result.addedBreakPointsUpdated;
                    }
                    else
                    {
                        pair const cursorPos = activeMainWindow->getCursorRealPosition(printingOrchestrator.get());
                        activeMainWindow->disableCursor();
                        if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastUndoRedoPressed) > std::chrono::milliseconds(1000))
                        {
                            utils::helpers::Lambda::blinkPrint(utils::datatypes::Strings::NOTHING_TO_PASTE, utils::datatypes::Strings::READ_MODE, ONE_SECOND, bottomBarWindow);
                        }
                        lastUndoRedoPressed = std::chrono::steady_clock::now();
                        utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, cursorPos);
                        activeMainWindow->enableCursor();
                        refreshProgressWindow();
                    }
                    break;
                }
                else if (U'd' == utf8Character && (utils::FEATHER_MODE::READ_MODE == currentFeatherMode)) // Remove shortcuts
                {
                    unhiglightSelectedText(activeMainWindow);
                    actionType = waitForSecondKey(activeMainWindow, bottomBarWindow, utils::datatypes::ChangeType::DELETE);
                    if (action::ACTION_TYPE::REMOVE_BETWEEN_BRACKETS == actionType.first)
                    {
                        lastCommand[activeMainWindow->getUUID()] = utils::datatypes::Command::REMOVE_BETWEEN_BRACKETS;
                    remove_between_brackets_command:
                        pair cursorPos = activeMainWindow->getCursorRealPosition(printingOrchestrator.get());
                        try
                        {
                            rewindFrameIfNeeded(cursorPos, activeMainWindow);
                            activeMainWindow->disableCursor();
                            auto newCursorRealPositions = actionHandler->handle(std::make_unique<action::RemoveBetweenBracketsAction>(action::RemoveBetweenBracketsActionInput(activeMainWindow->getUUID(), windowsManager, cursorPos, actionType.second, activeMainWindow->getStorage(), printingOrchestrator))).newCursorPosition;
                            if (newCursorRealPositions.has_value())
                            {
                                windowsManager->refreshAllWindows(currFrameVisitor);
                                utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, activeMainWindow->getCursorRealPosition(printingOrchestrator.get()));
                                activeMainWindow->setCursorFromRealPosition(
                                    newCursorRealPositions.value(),
                                    printingOrchestrator,
                                    halfFrameUpVisitor);
                                activeMainWindow->moveCursor(utils::Direction::ALIGN_TO_READ_MODE, linesLengthInformationVisitor, std::nullopt);
                                utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, activeMainWindow->getCursorRealPosition(printingOrchestrator.get()));
                                activeMainWindow->enableCursor();
                                refreshProgressWindow();
                            }
                            else
                            {
                                activeMainWindow->setCursorFromRealPosition(
                                    cursorPos,
                                    printingOrchestrator,
                                    halfFrameUpVisitor);
                                activeMainWindow->enableCursor();
                            }
                        }
                        catch (utils::exception::FeatherInterruptedException &e)
                        {
                            auto firstValidPos = printingOrchestrator->getNextVirtualPosition(activeMainWindow->getUUID(), activeMainWindow->getStorage(), cursorPos);
                            utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, firstValidPos);
                            refreshProgressWindow();
                        }
                    }
                    else if (action::ACTION_TYPE::REMOVE_BETWEEN_WHITE_CHARACTERS == actionType.first)
                    {
                        lastCommand[activeMainWindow->getUUID()] = utils::datatypes::Command::REMOVE_BETWEEN_WHITE_CHARACTERS;
                    remove_between_white_characters_command:
                        auto cursorPos = activeMainWindow->getCursorRealPosition(printingOrchestrator.get());
                        try
                        {
                            activeMainWindow->disableCursor();
                            auto newCursorRealPositions = actionHandler->handle(std::make_unique<action::RemoveBetweenWhiteCharactersAction>(action::RemoveBetweenWhiteCharactersActionInput(activeMainWindow->getUUID(), windowsManager, cursorPos, activeMainWindow->getStorage(), printingOrchestrator))).getResult();
                            utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, newCursorRealPositions);
                            if (isCursorAtFirstLine())
                            {
                                activeMainWindow->disableCursor();
                                windowsManager->refreshAllWindows(halfFrameUpVisitor);
                            }
                            else
                            {
                                activeMainWindow->disableCursor();
                                windowsManager->refreshAllWindows(currFrameVisitor);
                            }
                            utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, newCursorRealPositions);
                            activeMainWindow->enableCursor();
                            activeMainWindow->moveCursor(utils::Direction::ALIGN_TO_READ_MODE, linesLengthInformationVisitor, std::nullopt);
                            refreshProgressWindow();
                        }
                        catch (utils::exception::FeatherInterruptedException &e)
                        {
                            auto firstValidPos = printingOrchestrator->getNextVirtualPosition(activeMainWindow->getUUID(), activeMainWindow->getStorage(), cursorPos);
                            utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, firstValidPos);
                            refreshProgressWindow();
                        }
                        break;
                    }
                    else if (action::ACTION_TYPE::REMOVE_WORD_FORWARD_ACTION == actionType.first)
                    {
                        lastCommand[activeMainWindow->getUUID()] = utils::datatypes::Command::REMOVE_WORD_FORWARD;
                    remove_word_forward_command:
                        pair cursorPos = activeMainWindow->getCursorRealPosition(printingOrchestrator.get());
                        try
                        {
                            rewindFrameIfNeeded(cursorPos, activeMainWindow);
                            activeMainWindow->disableCursor();
                            auto newCursorRealPositions = actionHandler->handle(std::make_unique<action::RemoveWordForwardAction>(action::RemoveWordForwardActionInput(activeMainWindow->getUUID(), windowsManager, cursorPos, activeMainWindow->getStorage(), printingOrchestrator))).getResult();
                            windowsManager->refreshAllWindows(currFrameVisitor);
                            utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, activeMainWindow->getCursorRealPosition(printingOrchestrator.get()));
                            activeMainWindow->setCursorFromRealPosition(
                                newCursorRealPositions,
                                printingOrchestrator,
                                halfFrameUpVisitor);
                            activeMainWindow->moveCursor(utils::Direction::ALIGN_TO_READ_MODE, linesLengthInformationVisitor, std::nullopt);
                            utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, activeMainWindow->getCursorRealPosition(printingOrchestrator.get()));
                            activeMainWindow->enableCursor();
                            refreshProgressWindow();
                        }
                        catch (utils::exception::FeatherInterruptedException &e)
                        {
                            auto firstValidPos = printingOrchestrator->getNextVirtualPosition(activeMainWindow->getUUID(), activeMainWindow->getStorage(), cursorPos);
                            utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, firstValidPos);
                            refreshProgressWindow();
                        }
                    }
                    else if (action::ACTION_TYPE::REMOVE_LINE_ACTION == actionType.first)
                    {
                        lastCommand[activeMainWindow->getUUID()] = utils::datatypes::Command::REMOVE_LINE;
                    remove_line_command:
                        auto cursorPos = activeMainWindow->getCursorRealPosition(printingOrchestrator.get());
                        try
                        {
                            auto newCursorRealPositions = actionHandler->handle(std::make_unique<action::RemoveLineAction>(action::RemoveLineActionInput(activeMainWindow->getUUID(), windowsManager, cursorPos, 1UL, activeMainWindow->getStorage(), printingOrchestrator))).getResult();
                            utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, newCursorRealPositions);
                            if (isCursorAtFirstLine())
                            {
                                activeMainWindow->disableCursor();
                                windowsManager->refreshAllWindows(halfFrameUpVisitor);
                            }
                            else
                            {
                                windowsManager->refreshAllWindows(currFrameVisitor);
                            }
                            utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, newCursorRealPositions);
                            activeMainWindow->setCursorFromRealPosition(
                                actionHandler->handle(std::make_unique<action::SkipEmptyCharactersAction>(action::SkipEmptyCharactersActionInput(activeMainWindow, printingOrchestrator, windowsManager))).getResult(),
                                printingOrchestrator,
                                halfFrameUpVisitor);
                            utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, activeMainWindow->getCursorRealPosition(printingOrchestrator.get()));
                            activeMainWindow->enableCursor();
                            activeMainWindow->moveCursor(utils::Direction::ALIGN_TO_READ_MODE, linesLengthInformationVisitor, std::nullopt);
                            refreshProgressWindow();
                        }
                        catch (utils::exception::FeatherInterruptedException &e)
                        {
                            auto firstValidPos = printingOrchestrator->getNextVirtualPosition(activeMainWindow->getUUID(), activeMainWindow->getStorage(), cursorPos);
                            utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, firstValidPos);
                            refreshProgressWindow();
                        }
                        break;
                    }
                    else if (action::ACTION_TYPE::REMOVE_MULTIPLE_LINES_ACTION == actionType.first)
                    {
                        lastCommand[activeMainWindow->getUUID()] = utils::datatypes::Command::REMOVE_MULTIPLE_LINES;
                    remove_multiple_lines_command:
                        if (0 == actionType.second)
                        {
                            break;
                        }
                        auto cursorPos = activeMainWindow->getCursorRealPosition(printingOrchestrator.get());
                        try
                        {
                            auto newCursorRealPosition = actionHandler->handle(std::make_unique<action::RemoveLineAction>(action::RemoveLineActionInput(activeMainWindow->getUUID(), windowsManager, cursorPos, actionType.second, activeMainWindow->getStorage(), printingOrchestrator))).getResult();
                            std::this_thread::sleep_for(std::chrono::milliseconds(250));
                            utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, newCursorRealPosition);
                            if (isCursorAtFirstLine())
                            {
                                windowsManager->refreshAllWindows(halfFrameUpVisitor);
                            }
                            else
                            {
                                windowsManager->refreshAllWindows(currFrameVisitor);
                            }
                            utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, newCursorRealPosition);
                            activeMainWindow->setCursorFromRealPosition(
                                actionHandler->handle(std::make_unique<action::SkipEmptyCharactersAction>(action::SkipEmptyCharactersActionInput(activeMainWindow, printingOrchestrator, windowsManager))).getResult(),
                                printingOrchestrator,
                                halfFrameUpVisitor);
                            utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, activeMainWindow->getCursorRealPosition(printingOrchestrator.get()));
                            activeMainWindow->enableCursor();
                            activeMainWindow->moveCursor(utils::Direction::ALIGN_TO_READ_MODE, linesLengthInformationVisitor, std::nullopt);
                            refreshProgressWindow();
                        }
                        catch (utils::exception::FeatherInterruptedException &e)
                        {
                            auto firstValidPos = printingOrchestrator->getNextVirtualPosition(activeMainWindow->getUUID(), activeMainWindow->getStorage(), cursorPos);
                            utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, firstValidPos);
                            refreshProgressWindow();
                        }
                        break;
                    }
                    else if (action::ACTION_TYPE::REMOVE_UNTIL_FILE_BEGIN_ACTION == actionType.first)
                    {
                        lastCommand[activeMainWindow->getUUID()] = utils::datatypes::Command::REMOVE_UNTIL_FILE_BEGIN;
                    remove_until_file_begin_command:
                        auto cursorPos = actionHandler->handle(std::make_unique<action::RemoveUntilFileBeginAction>(action::RemoveUntilFileBeginActionInput(activeMainWindow->getUUID(), windowsManager, activeMainWindow->getCursorRealPosition(printingOrchestrator.get()), activeMainWindow->getStorage(), printingOrchestrator))).getResult();
                        activeMainWindow->disableCursor();
                        utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, cursorPos);
                        windowsManager->refreshAllWindows(currFrameVisitor);
                        activeMainWindow->enableCursor();
                        activeMainWindow->moveCursor(utils::Direction::ALIGN_TO_READ_MODE, linesLengthInformationVisitor, std::nullopt);
                        utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, activeMainWindow->getCursorRealPosition(printingOrchestrator.get()));
                        refreshProgressWindow();
                    }
                    else if (action::ACTION_TYPE::REMOVE_UNTIL_FILE_END_ACTION == actionType.first)
                    {
                        lastCommand[activeMainWindow->getUUID()] = utils::datatypes::Command::REMOVE_UNTIL_FILE_END;
                    remove_until_file_end_command:
                        auto newCursorRealPositions = actionHandler->handle(std::make_unique<action::RemoveUntilFileEndAction>(action::RemoveUntilFileEndActionInput(activeMainWindow->getUUID(), windowsManager, activeMainWindow->getCursorRealPosition(printingOrchestrator.get()), activeMainWindow->getStorage(), printingOrchestrator))).getResult();
                        if (isCursorAtFirstLine())
                        {
                            activeMainWindow->disableCursor();
                            windowsManager->refreshAllWindows(halfFrameUpVisitor);
                        }
                        else
                        {
                            activeMainWindow->disableCursor();
                            windowsManager->refreshAllWindows(currFrameVisitor);
                        }
                        activeMainWindow->setCursorFromRealPosition(
                            newCursorRealPositions,
                            printingOrchestrator,
                            halfFrameUpVisitor);
                        activeMainWindow->enableCursor();
                        activeMainWindow->moveCursor(utils::Direction::ALIGN_TO_READ_MODE, linesLengthInformationVisitor, std::nullopt);
                        utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, activeMainWindow->getCursorRealPosition(printingOrchestrator.get()));
                        refreshProgressWindow();
                    }
                    break;
                }
                else if (U':' == utf8Character) // Action mode
                {
                    unhiglightSelectedText(activeMainWindow);
                    currentFeatherMode = feather::utils::FEATHER_MODE::ACTION_MODE;
                    auto lastCursorPos = activeMainWindow->getCursorPosition();
                    auto lastCursorRealPos = activeMainWindow->getCursorRealPosition(printingOrchestrator.get());
                    refreshInfoWindow(currFrameVisitor);
                    bool cursorWasMoved = ActionReader(bottomBarWindow, actionHandler, printingOrchestrator, storageFactory, windowsManager, changesStack, keyReader, std::make_pair(activeMainWindow->getFramePosition(), activeMainWindow->getCursorRealPosition(printingOrchestrator.get())), filesystem, breakPoints[activeMainWindow->getUUID()], breakPointsAdded[activeMainWindow->getUUID()], currentBreakPoint[activeMainWindow->getUUID()])();
                    feather::utils::NCursesWrapper::flushInputBuffer();
                    if (!cursorWasMoved)
                    {
                        updateCursorPosIfFileModified(activeMainWindow, lastCursorPos, lastCursorRealPos);
                        activeMainWindow->setCursorPosition(lastCursorPos);
                        activeMainWindow->moveCursor(utils::Direction::ALIGN_TO_READ_MODE, linesLengthInformationVisitor, std::nullopt);
                    }
                    currentFeatherMode = feather::utils::FEATHER_MODE::READ_MODE;
                    bottomBarWindow->disableCursor();
                    refreshProgressWindow();
                    setModeToReadMode(bottomBarWindow);
                    if (breakPoints[windowsManager->getActiveMainWindow()].empty())
                    {
                        currentBreakPoint[windowsManager->getActiveMainWindow()] = breakPoints[windowsManager->getActiveMainWindow()].end();
                    }
                }
                else if (U'/' == utf8Character) // Search mode
                {
                    unhiglightSelectedText(activeMainWindow);
                    currentFeatherMode = feather::utils::FEATHER_MODE::SEARCH_MODE;
                    refreshInfoWindow(currFrameVisitor);
                    pair cursorPos = SearchReader(bottomBarWindow, actionHandler, printingOrchestrator, storageFactory, windowsManager, keyReader, breakPoints[activeMainWindow->getUUID()], breakPointsAdded[activeMainWindow->getUUID()], currentBreakPoint[activeMainWindow->getUUID()], std::make_pair(activeMainWindow->getFramePosition(), activeMainWindow->getCursorRealPosition(printingOrchestrator.get())), activeMainWindow->getCursorRealPosition(printingOrchestrator.get()), utils::Direction::DOWN)();
                    feather::utils::NCursesWrapper::flushInputBuffer();
                    currentFeatherMode = feather::utils::FEATHER_MODE::READ_MODE;
                    bottomBarWindow->disableCursor();
                    refresh(activeMainWindow, true, cursorPos);
                }
                else if (U'?' == utf8Character) // Search mode
                {
                    unhiglightSelectedText(activeMainWindow);
                    currentFeatherMode = feather::utils::FEATHER_MODE::SEARCH_MODE;
                    refreshInfoWindow(currFrameVisitor);
                    pair cursorPos = SearchReader(bottomBarWindow, actionHandler, printingOrchestrator, storageFactory, windowsManager, keyReader, breakPoints[activeMainWindow->getUUID()], breakPointsAdded[activeMainWindow->getUUID()], currentBreakPoint[activeMainWindow->getUUID()], std::make_pair(activeMainWindow->getFramePosition(), activeMainWindow->getCursorRealPosition(printingOrchestrator.get())), activeMainWindow->getCursorRealPosition(printingOrchestrator.get()), utils::Direction::UP)();
                    feather::utils::NCursesWrapper::flushInputBuffer();
                    currentFeatherMode = feather::utils::FEATHER_MODE::READ_MODE;
                    bottomBarWindow->disableCursor();
                    refresh(activeMainWindow, true, cursorPos);
                }
                else if (U'\\' == utf8Character && configuration->isHexMode()) // Hex search down mode
                {
                    unhiglightSelectedText(activeMainWindow);
                    currentFeatherMode = feather::utils::FEATHER_MODE::HEX_SEARCH_MODE;
                    refreshInfoWindow(currFrameVisitor);
                    pair cursorPos = SearchReader(bottomBarWindow, actionHandler, printingOrchestrator, storageFactory, windowsManager, keyReader, breakPoints[activeMainWindow->getUUID()], breakPointsAdded[activeMainWindow->getUUID()], currentBreakPoint[activeMainWindow->getUUID()], std::make_pair(activeMainWindow->getFramePosition(), activeMainWindow->getCursorRealPosition(printingOrchestrator.get())), activeMainWindow->getCursorRealPosition(printingOrchestrator.get()), utils::Direction::DOWN)();
                    feather::utils::NCursesWrapper::flushInputBuffer();
                    currentFeatherMode = feather::utils::FEATHER_MODE::READ_MODE;
                    bottomBarWindow->disableCursor();
                    refresh(activeMainWindow, true, cursorPos);
                }
                else if (U'|' == utf8Character && configuration->isHexMode()) // Hex search up mode
                {
                    unhiglightSelectedText(activeMainWindow);
                    currentFeatherMode = feather::utils::FEATHER_MODE::HEX_SEARCH_MODE;
                    refreshInfoWindow(currFrameVisitor);
                    pair cursorPos = SearchReader(bottomBarWindow, actionHandler, printingOrchestrator, storageFactory, windowsManager, keyReader, breakPoints[activeMainWindow->getUUID()], breakPointsAdded[activeMainWindow->getUUID()], currentBreakPoint[activeMainWindow->getUUID()], std::make_pair(activeMainWindow->getFramePosition(), activeMainWindow->getCursorRealPosition(printingOrchestrator.get())), activeMainWindow->getCursorRealPosition(printingOrchestrator.get()), utils::Direction::UP)();
                    feather::utils::NCursesWrapper::flushInputBuffer();
                    currentFeatherMode = feather::utils::FEATHER_MODE::READ_MODE;
                    bottomBarWindow->disableCursor();
                    refresh(activeMainWindow, true, cursorPos);
                }
                break;
            }
            }
            auto lastReloadTimestamp = activeMainWindow->getStorage()->getLastReloadTimestamp();
            if (activeMainWindow->getStorage()->getOpenTimestamp() != lastReloadTimestamp && lastReloadTimestamp > timestamp)
            {
                unselectTextToCopy();
                auto cursorPos = activeMainWindow->getCursorPosition();
                utils::helpers::Lambda::subWindowPrint(utils::datatypes::Strings::FILE_CONTENT_CHANGED, std::nullopt, bottomBarWindow, windowsManager->getSubWindow(windowsManager->getProgressBarWindow(bottomBarWindow->getParentWindowHandler()->getUUID()).value()), currFrameVisitor);
                activeMainWindow->setCursorPosition(cursorPos);
                activeMainWindow->enableCursor();
            }
        }
    }

    void KeyStroke::addNewCharacter(char32_t utf8Character, std::shared_ptr<windows::MainWindowInterface> activeMainWindow, std::unique_ptr<windows::SubWindowInterface> &bottomBarWindow)
    {
        emptySpacesToDiscard = std::nullopt;
        static std::chrono::steady_clock::time_point addingEndTime = std::chrono::steady_clock::now();
        static size_t noChunksReadLastTime = 0;
        auto initialCursorPos = activeMainWindow->getCursorRealPosition(printingOrchestrator.get());
        static pair lastPasteEndCursorPos = initialCursorPos;
        auto lastCursorPos = activeMainWindow->getCursorPosition();
        auto lastCursorRealPos = initialCursorPos;
        bool wasNewLineCharacterPressed = utils::helpers::Lambda::isNewLineChar(utf8Character);
        bool appendChunk = (noChunksReadLastTime > 1UL) && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - addingEndTime) < PASTE_GRACE_PERIOD;
        if (appendChunk)
        {
            lastCursorRealPos = lastPasteEndCursorPos;
        }
        noChunksReadLastTime = 0;
        changesStack->clearRedoChanges(activeMainWindow->getUUID());
        activeMainWindow->getLibraryWindowHandler()->setNonBlockingRead();
        size_t charactersPasted = 0UL;
        bool cursorDisabled = false;
        while (!utils::helpers::Lambda::isBackspace(utf8Character) && utf8Character && 27 != utf8Character && ERR != static_cast<int>(utf8Character))
        {
            ++charactersPasted;
            if (KEY_RESIZE == static_cast<int>(utf8Character))
            {
                defaultRedrawWindows(activeMainWindow);
                utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, initialCursorPos);
                utf8Character = keyReader->getNextCharacterFromInputBuffer(activeMainWindow->getLibraryWindowHandler());
                continue;
            }
            else if (ERR == static_cast<int>(utf8Character) || KEY_LEFT == static_cast<int>(utf8Character) || KEY_RIGHT == static_cast<int>(utf8Character) || KEY_UP == static_cast<int>(utf8Character) || KEY_DOWN == static_cast<int>(utf8Character))
            {
                utf8Character = keyReader->getNextCharacterFromInputBuffer(activeMainWindow->getLibraryWindowHandler());
                continue;
            }
            else if (!inputBuffer.add((char)utf8Character))
            {
                // Input buffer full
                printingOrchestrator->addChanges(activeMainWindow->getUUID(), activeMainWindow->getStorage(), initialCursorPos, inputBuffer.getBuffer(), inputBuffer.getBufferSize(), appendChunk);
                updateBreakPoints(initialCursorPos, inputBuffer.getBufferSize(), activeMainWindow->getUUID());
                inputBuffer.clear();
                inputBuffer.add((char)utf8Character);
                initialCursorPos = lastCursorRealPos;
                appendChunk = true;
            }
            // That mean user is pastig a text
            if (charactersPasted > 32UL && !cursorDisabled)
            {
                activeMainWindow->disableCursor();
                cursorDisabled = true;
            }
            lastCursorRealPos.second += utils::Utf8Util::getCharacterBytesTakenLight(utf8Character);
            utf8Character = keyReader->getNextCharacterFromInputBuffer(activeMainWindow->getLibraryWindowHandler());
            wasNewLineCharacterPressed = (ERR == static_cast<int>(utf8Character) && wasNewLineCharacterPressed) ? true : utils::helpers::Lambda::isNewLineChar(utf8Character);
            ++noChunksReadLastTime;
        }
        if (utils::helpers::Lambda::isBackspace(utf8Character))
        {
            backspaceHandler(activeMainWindow);
        }
        else if (27 == utf8Character)
        {
            setModeToReadMode(bottomBarWindow);
            // clear rest of the buffer
        }
        else if (inputBuffer.getBufferSize())
        {
            printingOrchestrator->addChanges(activeMainWindow->getUUID(), activeMainWindow->getStorage(), initialCursorPos, inputBuffer.getBuffer(), inputBuffer.getBufferSize(), appendChunk);
            updateBreakPoints(initialCursorPos, inputBuffer.getBufferSize(), activeMainWindow->getUUID());
        }
        inputBuffer.clear();
        activeMainWindow->getLibraryWindowHandler()->setBlockingRead();
        windowsManager->refreshAllWindows(currFrameVisitor);
        lastPasteEndCursorPos = lastCursorRealPos;
        updateCursorPosIfFileModified(activeMainWindow, lastCursorPos, lastCursorRealPos);
        lastCursorRealPos = utils::helpers::Lambda::alignToProperPosition(lastCursorRealPos, activeMainWindow->getStorage(), printingOrchestrator, activeMainWindow->getUUID());
        utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, lastCursorRealPos);
        refreshProgressWindow();
        addingEndTime = std::chrono::steady_clock::now();
    }

    void KeyStroke::backspaceHandler(std::shared_ptr<windows::MainWindowInterface> activeMainWindow)
    {
        if (printingOrchestrator->isCharAtFirstPosition(activeMainWindow->getCursorRealPosition(printingOrchestrator.get()), activeMainWindow->getUUID()))
        {
            return;
        }
        else if (isCursorAtTopLeftCorner())
        {
            activeMainWindow->disableCursor();
            pair cursorPos = activeMainWindow->getCursorRealPosition(printingOrchestrator.get());
            windowsManager->refreshAllWindows(halfFrameUpVisitor);
            activeMainWindow->setCursorFromRealPosition(cursorPos, printingOrchestrator, halfFrameUpVisitor);
            activeMainWindow->moveCursor(utils::Direction::DOWN, linesLengthInformationVisitor, std::nullopt);
        }
        std::optional<size_t> oldLineUpSize = std::get<0>(activeMainWindow->accept(linesLengthInformationVisitor)->getInformation()).first;
        activeMainWindow->moveCursor(utils::Direction::LEFT, linesLengthInformationVisitor, oldLineUpSize);
        pair deletionStart = activeMainWindow->getCursorRealPosition(printingOrchestrator.get());
        pair deletionEnd = printingOrchestrator->getNextVirtualPosition(activeMainWindow->getUUID(), activeMainWindow->getStorage(), deletionStart, true);
        pair newPos = printingOrchestrator->removeCharacters(activeMainWindow->getUUID(), activeMainWindow->getStorage(), deletionStart, deletionEnd);
        windowsManager->refreshAllWindows(currFrameVisitor);
        utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, newPos);
        refreshProgressWindow();
    }

    void KeyStroke::moveCursorLeft(std::shared_ptr<windows::MainWindowInterface> activeMainWindow)
    {
        if (printingOrchestrator->isCharAtFirstPosition(activeMainWindow->getCursorRealPosition(printingOrchestrator.get()), activeMainWindow->getUUID()))
        {
            return;
        }
        else if (isCursorAtTopLeftCorner())
        {
            windowsManager->refreshAllWindows(lineUpVisitor);
            activeMainWindow->moveCursor(utils::Direction::DOWN, linesLengthInformationVisitor, std::nullopt);
        }
        activeMainWindow->moveCursor(utils::Direction::LEFT, linesLengthInformationVisitor, std::nullopt);
        refreshProgressWindow();
    }

    void KeyStroke::moveCursorRight(std::shared_ptr<windows::MainWindowInterface> activeMainWindow)
    {
        if (isCursorAtLowerRightCorner())
        {
            windowsManager->refreshAllWindows(lineDownVisitor);
            activeMainWindow->moveCursor(utils::Direction::UP, linesLengthInformationVisitor, std::nullopt);
        }
        activeMainWindow->moveCursor(utils::Direction::RIGHT, linesLengthInformationVisitor, std::nullopt);
        refreshProgressWindow();
    }

    void KeyStroke::refresh(std::shared_ptr<windows::MainWindowInterface> activeMainWindow, bool shouldEnableCursor, std::optional<pair> newCursorPos)
    {
        windowsManager->refreshAllWindows(currFrameVisitor);
        if (isCursorAtFirstLine() && newCursorPos.has_value())
        {
            pair savedCursorPos = activeMainWindow->getCursorRealPosition(printingOrchestrator.get());
            windowsManager->refreshAllWindows(halfFrameUpVisitor);
            utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, savedCursorPos);
        }
        if (newCursorPos.has_value())
        {
            utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, newCursorPos.value());
        }
        activeMainWindow->moveCursor(utils::Direction::ALIGN_TO_READ_MODE, linesLengthInformationVisitor, std::nullopt);
        if (shouldEnableCursor)
        {
            activeMainWindow->enableCursor();
        }
        utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, activeMainWindow->getCursorRealPosition(printingOrchestrator.get()));
        windowsManager->refreshAllWindows(currFrameVisitor);
        utils::NCursesWrapper::applyRefresh();
    }

    void KeyStroke::refreshInfoWindow(std::unique_ptr<printer::PrintingVisitorInterface> &visitor)
    {
        auto activeMainWindow = windowsManager->getMainWindow(windowsManager->getActiveMainWindow());
        windowsManager->refreshInfoWindow(visitor, activeMainWindow->getUUID());
        refreshProgressWindow();
    }

    void KeyStroke::refreshProgressWindow()
    {
        auto activeMainWindow = windowsManager->getMainWindow(windowsManager->getActiveMainWindow());
        windowsManager->refreshProgresWindow(currFrameVisitor, activeMainWindow->getUUID());
        activeMainWindow->enableCursor();
    }

    void KeyStroke::refreshShortcutsWindow(std::unique_ptr<printer::PrintingVisitorInterface> &visitor)
    {
        auto activeMainWindow = windowsManager->getMainWindow(windowsManager->getActiveMainWindow());
        windowsManager->refreshShortcutsWindow(visitor, activeMainWindow->getUUID());
    }

    void KeyStroke::rewindFrameIfNeeded(pair cursorPos, std::shared_ptr<windows::MainWindowInterface> activeMainWindow)
    {
        if (shouldFrameBeRewindedBeforeDelete(cursorPos))
        {
            activeMainWindow->disableCursor();
            windowsManager->refreshAllWindows(halfFrameUpVisitor);
            activeMainWindow->setCursorFromRealPosition(utils::helpers::Lambda::alignToProperPosition(cursorPos, activeMainWindow->getStorage(), printingOrchestrator, activeMainWindow->getUUID()), printingOrchestrator, halfFrameUpVisitor);
            activeMainWindow->enableCursor();
        }
    }
    void KeyStroke::addBreakpoint(std::shared_ptr<windows::MainWindowInterface> activeMainWindow, std::unique_ptr<windows::SubWindowInterface> &bottomBarWindow)
    {
        pair cursorPos = activeMainWindow->getCursorRealPosition(printingOrchestrator.get());
        if (!breakPointsAdded[activeMainWindow->getUUID()].insert(cursorPos).second)
        {
            return;
        }
        breakPoints[activeMainWindow->getUUID()].push_back(cursorPos);
        if (breakPoints[activeMainWindow->getUUID()].end() == currentBreakPoint[activeMainWindow->getUUID()])
        {
            currentBreakPoint[activeMainWindow->getUUID()] = breakPoints[activeMainWindow->getUUID()].begin();
        }
        activeMainWindow->disableCursor();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastAddBreakpointPressed) > std::chrono::milliseconds(1000))
        {
            utils::helpers::Lambda::subWindowPrint(utils::datatypes::Strings::ADDED_BREAKPOINT, ONE_SECOND, bottomBarWindow, windowsManager->getSubWindow(windowsManager->getProgressBarWindow(activeMainWindow->getUUID()).value()), currFrameVisitor);
        }
        lastAddBreakpointPressed = std::chrono::steady_clock::now();
        refresh(activeMainWindow, true, cursorPos);
    }

    void KeyStroke::highlightTextIfNeeded(std::shared_ptr<windows::MainWindowInterface> activeMainWindow)
    {
        //-1 is UP, 0 neutral, 1 DOWN
        static int direction = 0;
        if (utils::FEATHER_MODE::READ_MODE_SELECT_TEXT == currentFeatherMode)
        {
            if (selectedText->first == selectedText->second)
            {
                direction = 0;
            }
            const pair cursorRealPos = activeMainWindow->getCursorRealPosition(printingOrchestrator.get());
            if (cursorRealPos < selectedText->second && ((0 == direction) || (-1 == direction)))
            {
                selectedText->first = cursorRealPos;
                direction = -1;
            }
            else if (cursorRealPos > selectedText->first && ((0 == direction) || (1 == direction)))
            {
                selectedText->second = cursorRealPos;
                direction = 1;
            }
            else if (-1 == direction)
            {
                selectedText->first = selectedText->second;
                selectedText->second = cursorRealPos;
                direction = 1;
            }
            else
            {
                selectedText->second = selectedText->first;
                selectedText->first = cursorRealPos;
                direction = -1;
            }
            auto cursorPos = activeMainWindow->getCursorPosition();
            activeMainWindow->disableCursor();
            auto highlightColor = printingOrchestrator->highlightSelection(selectedText.value(), activeMainWindow->getUUID());
            windowsManager->refreshAllWindows(currFrameVisitor);
            activeMainWindow->setCursorPosition(cursorPos);
            activeMainWindow->enableCursor();
        }
        else
        {
            unhiglightSelectedText(activeMainWindow);
        }
    }

    void KeyStroke::defaultRedrawWindows(std::shared_ptr<windows::MainWindowInterface> activeMainWindow)
    {
        pair cursorPos = activeMainWindow->getCursorRealPosition(printingOrchestrator.get());
        windowsManager->recreateWindows();
        windowsManager->refreshAllWindows(currFrameVisitor);
        if (utils::FEATHER_MODE::READ_MODE == currentFeatherMode || utils::FEATHER_MODE::READ_MODE_SELECT_TEXT == currentFeatherMode || utils::FEATHER_MODE::WRITE_MODE == currentFeatherMode)
        {
            highlightTextIfNeeded(activeMainWindow);
        }
        utils::NCursesWrapper::applyRefresh();
        refresh(activeMainWindow, false, cursorPos);
    }

    void KeyStroke::nextBreakpoint(std::shared_ptr<windows::MainWindowInterface> activeMainWindow, std::unique_ptr<windows::SubWindowInterface> &bottomBarWindow)
    {
        pair cursorPos = activeMainWindow->getCursorRealPosition(printingOrchestrator.get());
        activeMainWindow->disableCursor();
        if (breakPoints[activeMainWindow->getUUID()].cend() == currentBreakPoint[activeMainWindow->getUUID()])
        {
            utils::helpers::Lambda::subWindowPrint(utils::datatypes::Strings::NO_BREAKPOINTS_SET, ONE_SECOND, bottomBarWindow, windowsManager->getSubWindow(windowsManager->getProgressBarWindow(activeMainWindow->getUUID()).value()), currFrameVisitor);
        }
        else
        {
            auto localCurrentBreakPoint = currentBreakPoint[activeMainWindow->getUUID()]++;
            while (currentBreakPoint[activeMainWindow->getUUID()] == breakPoints[activeMainWindow->getUUID()].cend() || printingOrchestrator->isCharAtPosDeleted(*currentBreakPoint[activeMainWindow->getUUID()], activeMainWindow->getUUID()))
            {
                if (currentBreakPoint[activeMainWindow->getUUID()] == breakPoints[activeMainWindow->getUUID()].cend())
                {
                    currentBreakPoint[activeMainWindow->getUUID()] = breakPoints[activeMainWindow->getUUID()].begin();
                    continue;
                }
                else if (currentBreakPoint[activeMainWindow->getUUID()] == localCurrentBreakPoint)
                {
                    utils::helpers::Lambda::subWindowPrint(utils::datatypes::Strings::NO_BREAKPOINTS_SET, ONE_SECOND, bottomBarWindow, windowsManager->getSubWindow(windowsManager->getProgressBarWindow(activeMainWindow->getUUID()).value()), currFrameVisitor);
                    goto refresh_windows;
                }
                ++currentBreakPoint[activeMainWindow->getUUID()];
            }
            cursorPos = *currentBreakPoint[activeMainWindow->getUUID()];
        }
    refresh_windows:
        refresh(activeMainWindow, true, cursorPos);
    }

    void KeyStroke::previousBreakpoint(std::shared_ptr<windows::MainWindowInterface> activeMainWindow, std::unique_ptr<windows::SubWindowInterface> &bottomBarWindow)
    {
        pair cursorPos = activeMainWindow->getCursorRealPosition(printingOrchestrator.get());
        activeMainWindow->disableCursor();
        if (breakPoints[activeMainWindow->getUUID()].end() == currentBreakPoint[activeMainWindow->getUUID()])
        {
            utils::helpers::Lambda::blinkPrint(utils::datatypes::Strings::NO_BREAKPOINTS_SET, utils::datatypes::Strings::READ_MODE, ONE_SECOND, bottomBarWindow);
        }
        else
        {
            auto localCurrentBreakPoint = currentBreakPoint[activeMainWindow->getUUID()];
            currentBreakPoint[activeMainWindow->getUUID()] = (currentBreakPoint[activeMainWindow->getUUID()] == breakPoints[activeMainWindow->getUUID()].begin()) ? std::prev(breakPoints[activeMainWindow->getUUID()].end()) : std::prev(currentBreakPoint[activeMainWindow->getUUID()]);
            while (printingOrchestrator->isCharAtPosDeleted(*currentBreakPoint[activeMainWindow->getUUID()], activeMainWindow->getUUID()))
            {
                if (currentBreakPoint[activeMainWindow->getUUID()] == localCurrentBreakPoint)
                {
                    utils::helpers::Lambda::blinkPrint(utils::datatypes::Strings::NO_BREAKPOINTS_SET, utils::datatypes::Strings::READ_MODE, ONE_SECOND, bottomBarWindow);
                    goto refresh_windows;
                }
                --currentBreakPoint[activeMainWindow->getUUID()];
            }
            cursorPos = *currentBreakPoint[activeMainWindow->getUUID()];
        }
    refresh_windows:
        refresh(activeMainWindow, true, cursorPos);
    }

    void KeyStroke::normalizeAfterSave(std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator, utils::datatypes::Uuid const &windowUUID)
    {
        if (breakPoints.count(windowUUID))
        {
            std::list<pair> updatedBreakPoints;
            std::for_each(breakPoints[windowUUID].begin(), breakPoints[windowUUID].end(), [&](auto &e)
                          {
                if (!printingOrchestrator->isCharAtPosDeleted(e, windowUUID))
                {
                    updatedBreakPoints.push_back(pair(printingOrchestrator->convertVirtualPositionToByteOffset(e, windowUUID), 0));
                } });
            std::swap(breakPoints[windowUUID], updatedBreakPoints);
            currentBreakPoint[windowUUID] = breakPoints[windowUUID].begin();
        }
    }

    void KeyStroke::updateBreakPoints(pair pos, size_t size, utils::datatypes::Uuid const &windowUUID)
    {
        if (!breakPoints.count(windowUUID))
        {
            return;
        }
        utils::helpers::Lambda::shiftPairs(breakPoints[windowUUID], pos, size, currentBreakPoint[windowUUID]);
    }

    void KeyStroke::cleanBreakPoints(pair lastValidPos, utils::datatypes::Uuid const &windowUUID)
    {
        if (!breakPoints.count(windowUUID))
        {
            return;
        }

        std::list<pair> updatedBreakPoints;
        for (auto e : breakPoints[windowUUID])
        {
            if (e <= lastValidPos)
            {
                updatedBreakPoints.push_back(e);
            }
        }

        if (updatedBreakPoints.empty())
        {
            breakPoints.erase(windowUUID);
            currentBreakPoint.erase(windowUUID);
        }
        else if (currentBreakPoint.count(windowUUID) && currentBreakPoint[windowUUID] != breakPoints[windowUUID].end())
        {
            size_t currBreakPointOffset = std::distance(breakPoints[windowUUID].begin(), currentBreakPoint[windowUUID]);
            std::swap(breakPoints[windowUUID], updatedBreakPoints);
            if (*currentBreakPoint[windowUUID] > lastValidPos || currBreakPointOffset > breakPoints.size())
            {
                currentBreakPoint[windowUUID] = breakPoints[windowUUID].end();
            }
            else
            {
                currentBreakPoint[windowUUID] = std::next(breakPoints[windowUUID].begin(), currBreakPointOffset);
            }
        }
    }

    bool KeyStroke::isCursorAtFirstLine() const
    {
        auto activeMainWindow = windowsManager->getMainWindow(windowsManager->getActiveMainWindow());
        if (configuration->isHexMode())
        {
            auto hexModeWindow = windowsManager->getMainWindow(windowsManager->getSecondaryActiveMainWindowForHexMode());
            return (0 == activeMainWindow->getCursorPosition().first) || (0 == hexModeWindow->getCursorPosition().first);
        }
        return 0 == activeMainWindow->getCursorPosition().first;
    }

    bool KeyStroke::isCursorAtLastLine() const
    {
        auto activeMainWindow = windowsManager->getMainWindow(windowsManager->getActiveMainWindow());
        bool isScreenBufferLastLine = activeMainWindow->getCursorPosition().first == activeMainWindow->getBufferFiller()->getScreenBuffer().getFilledRows() - 1UL;
        auto lastValidPosition = printingOrchestrator->getLastValidPosition(activeMainWindow->getUUID(), activeMainWindow->getBufferFiller()->getStorage());
        pair frameEndPos = activeMainWindow->getBufferFiller()->getFramePositions().endFramePosition;
        return isScreenBufferLastLine && (lastValidPosition == frameEndPos);
    }

    bool KeyStroke::isCursorAtLowerRightCorner() const
    {
        auto activeMainWindow = windowsManager->getMainWindow(windowsManager->getActiveMainWindow());
        return activeMainWindow->getCursorPosition().second == activeMainWindow->getWindowDimensions().second - 1 &&
               activeMainWindow->getCursorPosition().first == activeMainWindow->getWindowDimensions().first - 1;
    }

    bool KeyStroke::isCursorAtTopLeftCorner() const
    {
        auto activeMainWindow = windowsManager->getMainWindow(windowsManager->getActiveMainWindow());
        auto firstScreenChar = activeMainWindow->getBufferFiller()->getScreenBuffer().getCharAt(pair(0, 0));
        auto cursorPos = activeMainWindow->getCursorPosition();
        return (0 == activeMainWindow->getCursorPosition().first && 0 == activeMainWindow->getCursorPosition().second) ||
               (!printingOrchestrator->isCharAtFirstPosition(activeMainWindow->getBufferFiller()->getFramePositions().startFramePosition, activeMainWindow->getUUID()) && (firstScreenChar.has_value() && (pair(0, utils::helpers::Lambda::getCharacterWidth(firstScreenChar.value(), true, activeMainWindow->isSecondaryHexModeMainWindow()))) == cursorPos));
    }

    bool KeyStroke::isLastValidPosInFrame() const
    {
        auto activeMainWindow = windowsManager->getMainWindow(windowsManager->getActiveMainWindow());
        auto lastValidPosition = printingOrchestrator->getLastValidPosition(activeMainWindow->getUUID(), activeMainWindow->getBufferFiller()->getStorage());
        pair frameEndPos = activeMainWindow->getBufferFiller()->getFramePositions().endFramePosition;
        return (lastValidPosition == frameEndPos);
    }

    bool KeyStroke::shouldFrameBeRewindedBeforeDelete(pair deletionStart) const
    {
        return deletionStart <= windowsManager->getMainWindow(windowsManager->getActiveMainWindow())->getFramePosition().startFramePosition;
    }

    std::pair<action::ACTION_TYPE, size_t> KeyStroke::waitForSecondKey(std::shared_ptr<windows::MainWindowInterface> activeMainWindow, std::unique_ptr<windows::SubWindowInterface> &bottomBarWindow, utils::datatypes::ChangeType changeType)
    {
        typedef std::chrono::milliseconds ms;
        char32_t keyPressed = 0;
        activeMainWindow->getLibraryWindowHandler()->setNonBlockingRead();
        auto waitForSecondaryKeyStartTime = std::chrono::steady_clock::now();
        while (true)
        {
            keyPressed = keyReader->getNextCharacterFromInputBuffer(activeMainWindow->getLibraryWindowHandler());
            if (1UL == utils::Utf8Util::getCharacterBytesTakenLight(keyPressed) && std::isdigit(static_cast<char>(keyPressed)))
            {
                activeMainWindow->getLibraryWindowHandler()->setBlockingRead();
                return std::make_pair(action::ACTION_TYPE::REMOVE_MULTIPLE_LINES_ACTION, static_cast<char>(keyPressed) - '0');
            }
            else if (U'[' == keyPressed || U'{' == keyPressed || U'<' == keyPressed || U'(' == keyPressed)
            {
                activeMainWindow->getLibraryWindowHandler()->setBlockingRead();
                return utils::datatypes::ChangeType::HIGHLIGHT == changeType ? std::make_pair(action::ACTION_TYPE::HIGHLIGHT_BETWEEN_BRACKETS, keyPressed) : std::make_pair(action::ACTION_TYPE::REMOVE_BETWEEN_BRACKETS, keyPressed);
            }
            else if (U'w' == keyPressed)
            {
                activeMainWindow->getLibraryWindowHandler()->setBlockingRead();
                return std::make_pair(action::ACTION_TYPE::REMOVE_WORD_FORWARD_ACTION, 0);
            }
            else if (U'd' == keyPressed)
            {
                activeMainWindow->getLibraryWindowHandler()->setBlockingRead();
                return std::make_pair(action::ACTION_TYPE::REMOVE_LINE_ACTION, 0);
            }
            else if (U's' == keyPressed)
            {
                activeMainWindow->getLibraryWindowHandler()->setBlockingRead();
                return std::make_pair(action::ACTION_TYPE::REMOVE_BETWEEN_WHITE_CHARACTERS, 0);
            }
            else if (U'z' == keyPressed)
            {
                activeMainWindow->getLibraryWindowHandler()->setBlockingRead();
                return std::make_pair(action::ACTION_TYPE::ALIGN_TO_MIDDLE, 0);
            }
            else if (U'g' == keyPressed)
            {
                activeMainWindow->getLibraryWindowHandler()->setBlockingRead();
                return std::make_pair(action::ACTION_TYPE::REMOVE_UNTIL_FILE_BEGIN_ACTION, 0);
            }
            else if (U'G' == keyPressed)
            {
                activeMainWindow->getLibraryWindowHandler()->setBlockingRead();
                return std::make_pair(action::ACTION_TYPE::REMOVE_UNTIL_FILE_END_ACTION, 0);
            }
            else if (U'y' == keyPressed)
            {
                activeMainWindow->getLibraryWindowHandler()->setBlockingRead();
                return std::make_pair(action::ACTION_TYPE::COPY_LINE_ACTION, 0);
            }
            else if (U'h' == keyPressed)
            {
                activeMainWindow->getLibraryWindowHandler()->setBlockingRead();
                return std::make_pair(action::ACTION_TYPE::HIGHLIGHT_LINE, 0);
            }
            else if (KEY_RESIZE == keyPressed)
            {
                defaultRedrawWindows(activeMainWindow);
                activeMainWindow->getLibraryWindowHandler()->setBlockingRead();
                return std::make_pair(action::ACTION_TYPE::UNKNOWN, 0);
            }
            else if (keyPressed != static_cast<char32_t>(ERR))
            {
                activeMainWindow->getLibraryWindowHandler()->setBlockingRead();
                return std::make_pair(action::ACTION_TYPE::UNKNOWN, 0);
            }

            if (std::chrono::duration_cast<ms>(std::chrono::steady_clock::now() - waitForSecondaryKeyStartTime) > ms(1500))
            {
                activeMainWindow->getLibraryWindowHandler()->setBlockingRead();
                return std::make_pair(action::ACTION_TYPE::UNKNOWN, 0);
            }
            std::this_thread::sleep_for(ms(100));
        }
        activeMainWindow->getLibraryWindowHandler()->setBlockingRead();
        return std::make_pair(action::ACTION_TYPE::UNKNOWN, 0);
    }

    void KeyStroke::setModeToReadMode(std::unique_ptr<windows::SubWindowInterface> &bottomBarWindow)
    {
        unhiglightSelectedText(bottomBarWindow->getParentWindowHandler());
        auto activeMainWindow = bottomBarWindow->getParentWindowHandler();
        if (emptySpacesToDiscard.has_value())
        {
            auto cursorPos = printingOrchestrator->removeCharacters(activeMainWindow->getUUID(), activeMainWindow->getStorage(), emptySpacesToDiscard->first, emptySpacesToDiscard->second, false);
            activeMainWindow->setCursorFromRealPosition(utils::helpers::Lambda::alignToProperPosition(cursorPos, activeMainWindow->getStorage(), printingOrchestrator, activeMainWindow->getUUID()), printingOrchestrator, halfFrameUpVisitor);
            emptySpacesToDiscard = std::nullopt;
        }
        auto lastCursorPos = activeMainWindow->getCursorPosition();
        auto lastCursorRealPos = activeMainWindow->getCursorRealPosition(printingOrchestrator.get());
        activeMainWindow->disableCursor();
        currentFeatherMode = feather::utils::FEATHER_MODE::READ_MODE;
        windowsManager->refreshAllWindows(currFrameVisitor);
        updateCursorPosIfFileModified(activeMainWindow, lastCursorPos, lastCursorRealPos);
        activeMainWindow->setCursorPosition(lastCursorPos);
        activeMainWindow->moveCursor(utils::Direction::ALIGN_TO_READ_MODE, linesLengthInformationVisitor, std::nullopt);
        activeMainWindow->setCursorPosition(lastCursorPos);
        activeMainWindow->moveCursor(utils::Direction::ALIGN_TO_READ_MODE, linesLengthInformationVisitor, std::nullopt);
        utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, activeMainWindow->getCursorRealPosition(printingOrchestrator.get()));
        activeMainWindow->enableCursor();
    }

    void KeyStroke::setModeToWriteMode(std::unique_ptr<windows::SubWindowInterface> &bottomBarWindow)
    {
        pair cursorPos = bottomBarWindow->getParentWindowHandler()->getCursorPosition();
        bottomBarWindow->getParentWindowHandler()->disableCursor();
        auto currentMainWindow = bottomBarWindow->getParentWindowHandler();
        currentFeatherMode = feather::utils::FEATHER_MODE::WRITE_MODE;
        windowsManager->refreshAllWindows(currFrameVisitor);
        currentMainWindow->setCursorPosition(cursorPos);
        currentMainWindow->moveCursor(utils::Direction::ALIGN_TO_READ_MODE, linesLengthInformationVisitor, std::nullopt);
        windowsManager->refreshProgresWindow(currFrameVisitor, currentMainWindow->getUUID());
        currentMainWindow->enableCursor();
    }

    void KeyStroke::unhiglightSelectedText(std::shared_ptr<windows::MainWindowInterface> activeMainWindow)
    {
        printingOrchestrator->unhighlightSelection(activeMainWindow->getUUID());
        selectedText = std::nullopt;
        if (currentFeatherMode != utils::FEATHER_MODE::WORKING_MODE)
        {
            currentFeatherMode = currentFeatherMode != utils::FEATHER_MODE::WRITE_MODE ? utils::FEATHER_MODE::READ_MODE : utils::FEATHER_MODE::WRITE_MODE;
        }
    }

    void KeyStroke::unselectTextToCopy()
    {
        printingOrchestrator->setLineToCopy(std::nullopt, std::nullopt);
        printingOrchestrator->setTextToCopy(std::nullopt, std::nullopt);
        printingOrchestrator->setTextToCut(std::nullopt, std::nullopt);
    	printingOrchestrator->setTextToCopyPositionsToSkip(std::nullopt);
    }

    void KeyStroke::updateCursorPosIfFileModified(std::shared_ptr<windows::MainWindowInterface> activeMainWindow, pair &lastCursorPos, pair &lastCursorRealPos)
    {
        auto lastValidPos = printingOrchestrator->getLastValidPosition(activeMainWindow->getUUID(), activeMainWindow->getStorage());
        if (activeMainWindow->getStorage()->getLastReloadTimestamp() > timestamp && lastCursorRealPos > lastValidPos)
        {
            if (currentFeatherMode == utils::FEATHER_MODE::READ_MODE_SELECT_TEXT && selectedText->first > lastValidPos)
            {
                unhiglightSelectedText(activeMainWindow);
            }
            activeMainWindow->setCursorFromRealPosition(lastValidPos, printingOrchestrator, halfFrameUpVisitor);
            lastCursorRealPos = lastValidPos; // activeMainWindow->getCursorRealPosition(printingOrchestrator.get());
            lastCursorPos = activeMainWindow->getCursorPosition();
            changesStack->updatePositionsAfterFileReload(printingOrchestrator, lastValidPos, activeMainWindow->getUUID());
        }
    }

    bool KeyStroke::yesNoPrompt(std::shared_ptr<windows::MainWindowInterface> activeMainWindow)
    {
        char32_t scannedCharacter = EOF;
    _begin:
        switch (scannedCharacter = keyReader->getNextCharacterFromInputBuffer(activeMainWindow->getLibraryWindowHandler()))
        {
        case static_cast<char32_t>(ERR):
            goto _begin;
        case KEY_RESIZE:
        {
            defaultRedrawWindows(activeMainWindow);
            goto _begin;
        }
        case U'Y':
        case U'y':
        {
            if (activeMainWindow->getBufferFiller()->getStorage()->wasFileCreated() &&
                !activeMainWindow->getBufferFiller()->getStorage()->getSize())
            {
                storageFactory->removeStorage(activeMainWindow->getBufferFiller()->getStorage()->getName());
            }
            return true;
        }
        case U'N':
        case U'n':
        case 27:
        {
            return false;
        }
        default:
            goto _begin;
        }
    }

    KeyStroke::~KeyStroke()
    {
    }

    std::unordered_map<utils::datatypes::Uuid, std::list<pair>, utils::datatypes::UuidHasher> KeyStroke::breakPoints;
    std::unordered_map<utils::datatypes::Uuid, std::set<pair>, utils::datatypes::UuidHasher> KeyStroke::breakPointsAdded;
    std::unordered_map<utils::datatypes::Uuid, std::list<pair>::iterator, utils::datatypes::UuidHasher> KeyStroke::currentBreakPoint;
    std::unordered_map<utils::datatypes::Uuid, utils::datatypes::Command, utils::datatypes::UuidHasher> KeyStroke::lastCommand;

} // namespace feather::reader
