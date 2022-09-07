#include "action/input/GotoLineActionInput.hpp"
#include "action/input/MoveToByteActionInput.hpp"
#include "action/input/ReplaceAllActionInput.hpp"
#include "action/input/SaveChangesActionInput.hpp"
#include "action/input/SearchSubstringActionInput.hpp"
#include "printer/CurrFrameVisitor.hpp"
#include "reader/ActionReader.hpp"
#include "utils/Filesystem.hpp"
#include "utils/algorithm/RegexSearch.hpp"
#include "utils/datatypes/Strings.hpp"
#include "utils/exception/FeatherInterruptedException.hpp"
#include "utils/helpers/Conversion.hpp"
#include "utils/helpers/Lambda.hpp"

#include <filesystem>

extern feather::utils::FEATHER_MODE currentFeatherMode;

namespace feather::reader
{
    ActionReader::ActionReader(std::unique_ptr<windows::SubWindowInterface> &currBottomBarWindow, std::shared_ptr<action::ActionHandler> actionHandler, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator, std::shared_ptr<utils::storage::AbstractStorageFactory> storageFactory, std::shared_ptr<windows::WindowsManager> windowsManager, std::shared_ptr<printer::ChangesStack> changesStackArg, std::shared_ptr<utils::helpers::KeyReader> keyReader, std::pair<utils::windows::FramePositions, pair> framePositions, std::shared_ptr<utils::FilesystemInterface> filesystemArg, std::list<pair> &breakPointsArg, std::set<pair> &breakPointsAddedArg, std::list<pair>::iterator &currentBreakPointArg)
        : Reader(currBottomBarWindow, actionHandler, printingOrchestrator, storageFactory, windowsManager, keyReader, framePositions, breakPointsArg, breakPointsAddedArg, currentBreakPointArg), changesStack(changesStackArg), filesystem(filesystemArg)
    {
        commandBuffer.push_back(U':');
        currBottomBarWindow->enableCursor();
        dynamic_cast<windows::NCursesBottomBarWindowDecorator *>(currBottomBarWindow.get())->setReader(this);
    }

    bool ActionReader::isAtHistoryBegin() const
    {
        return dynamic_cast<windows::NCursesBottomBarWindowDecorator *>(currBottomBarWindow.get())->isActionHistoryIteratorAtBegining();
    }

    bool ActionReader::isAtHistoryEnd() const
    {
        return dynamic_cast<windows::NCursesBottomBarWindowDecorator *>(currBottomBarWindow.get())->isActionHistoryIteratorAtLastPos();
    }

    std::optional<std::u32string> ActionReader::getCurrFromHistory() const
    {
        auto currFromHistory = dynamic_cast<windows::NCursesBottomBarWindowDecorator *>(currBottomBarWindow.get())->getCurrFromActionHistory();
        if (!currFromHistory.has_value())
        {
            return commandBuffer;
        }
        return currFromHistory;
    }

    std::optional<std::u32string> ActionReader::getNextFromHistory() const
    {
        return dynamic_cast<windows::NCursesBottomBarWindowDecorator *>(currBottomBarWindow.get())->getNextFromActionHistory();
    }

    std::optional<std::u32string> ActionReader::getPreviousFromHistory() const
    {
        return dynamic_cast<windows::NCursesBottomBarWindowDecorator *>(currBottomBarWindow.get())->getPreviousFromActionHistory();
    }

    bool ActionReader::operator()()
    {
        static std::set<std::u32string> filesInDir;
        static std::set<std::u32string>::const_iterator lastFileInDir = filesInDir.cend();
        std::u32string pathStem, pathStemSecondPart;
        std::vector<std::u32string> candidateCommands;
        std::vector<std::u32string>::iterator candidateCommand = candidateCommands.end();
        std::vector<std::u32string> candidateColors;
        std::vector<std::u32string>::iterator candidateColor = candidateColors.end();

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
            {
                terminalDimensionsChangeHandler(commandBuffer, cursorIdx);
                break;
            }
            case KEY_LEFT:
            {
                if (currBottomBarWindow->getCursorPosition().second <= 1)
                {
                    shiftCursorHalfWindowLeft(cursorIdx);
                }
                std::vector<std::u32string> splittedCommand = utils::helpers::Conversion::splitString(commandBuffer);
                if (splittedCommand.size() > 1UL && (U":w" == splittedCommand[0] || U":e" == splittedCommand[0]) && !pathStemSecondPart.empty())
                {
                    std::copy(pathStemSecondPart.cbegin(), pathStemSecondPart.cend(), std::back_inserter(commandBuffer));
                    pathStemSecondPart.clear();
                    pathStem.clear();
                    filesInDir.clear();
                    while (utils::helpers::Lambda::getStringWidth(commandBuffer, commandBufferFirstIdx, commandBuffer.length() - 1UL, false, false) > bottomBarWindowWidth)
                    {
                        ++cursorIdx;
                        commandBufferFirstIdx = cursorIdx;
                    }
                    setCursor(commandBuffer.length());
                    printWithoutDelay(commandBuffer, false);
                }
                auto cursorIdx = utils::helpers::Lambda::getIndexFromOffset(commandBuffer, commandBufferFirstIdx, currBottomBarWindow->getCursorPosition().second, bottomBarWindowWidth);
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
                std::vector<std::u32string> splittedCommand = utils::helpers::Conversion::splitString(commandBuffer);
                if (splittedCommand.size() > 1UL && (U":w" == splittedCommand[0] || U":e" == splittedCommand[0]) && !pathStemSecondPart.empty())
                {
                    auto tmpCommandBuffer = splittedCommand[1UL];
                    std::copy(pathStemSecondPart.cbegin(), pathStemSecondPart.cend(), std::back_inserter(tmpCommandBuffer));
                    std::string tmpCommandBufferString = utils::helpers::Conversion::squeezeu32String(tmpCommandBuffer);
                    std::copy(pathStemSecondPart.cbegin(), pathStemSecondPart.cend(), std::back_inserter(commandBuffer));
                    pathStemSecondPart.clear();
                    pathStem.clear();
                    filesInDir.clear();
                    setCursorAtTheEnd();
                    printWithoutDelay(commandBuffer, false);
                }
                else
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
                }
                break;
            }
            case 10:
            case KEY_ENTER:
            {
                std::vector<std::u32string> splittedCommand = utils::helpers::Conversion::splitString(commandBuffer);
                if (splittedCommand.size() > 1UL && (U":w" == splittedCommand[0] || U":e" == splittedCommand[0]))
                {
                    std::copy(pathStemSecondPart.cbegin(), pathStemSecondPart.cend(), std::back_inserter(commandBuffer));
                    size_t lastSlashPos = splittedCommand[1UL].rfind(U"/") + 1UL;
                    pathStem = splittedCommand[1UL].substr(lastSlashPos);
                    filesInDir.clear();
                    lastFileInDir = filesInDir.cend();
                }
                dynamic_cast<windows::NCursesBottomBarWindowDecorator *>(currBottomBarWindow.get())->addToActionHistory({commandBuffer.begin(), commandBuffer.end()});
                return handleCommand();
            }
            case KEY_DOWN:
            {
                if (anyChangeMade)
                {
                    setCursorAtTheEnd();
                    printWithoutDelay(commandBuffer, false);
                    break;
                }
                currBottomBarWindow->refreshBuffer(lineDownVisitor.get());
                if (dynamic_cast<windows::NCursesBottomBarWindowDecorator *>(currBottomBarWindow.get())->isActionHistoryIteratorAtLastPos())
                {
                    commandBuffer = ':';
                    setCursorAtTheEnd();
                    printWithoutDelay(commandBuffer, false);
                    break;
                }
                commandBuffer = *dynamic_cast<windows::NCursesBottomBarWindowDecorator *>(currBottomBarWindow.get())->getCurrFromActionHistory();
                handleModeChange();
                setCursorAtTheEnd();
                printWithoutDelay(commandBuffer, false);
                break;
            }
            case KEY_UP:
            {
                if (anyChangeMade || dynamic_cast<windows::NCursesBottomBarWindowDecorator *>(currBottomBarWindow.get())->isActionHistoryEmpty())
                {
                    break;
                }
                currBottomBarWindow->refreshBuffer(lineUpVisitor.get());
                commandBuffer = *dynamic_cast<windows::NCursesBottomBarWindowDecorator *>(currBottomBarWindow.get())->getCurrFromActionHistory();
                handleModeChange();
                setCursorAtTheEnd();
                printWithoutDelay(commandBuffer, false);
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
                if (commandBuffer.size() > 1UL)
                {
                    if (!pathStemSecondPart.empty())
                    {
                        std::copy(pathStemSecondPart.cbegin(), pathStemSecondPart.cend(), std::back_inserter(commandBuffer));
                        setCursorAtTheEnd();
                        cursorIdx = commandBuffer.size();
                    }
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
                    handleModeChange();
                    printWithoutDelay(commandBuffer, false);
                    pathStemSecondPart.clear();
                    std::vector<std::u32string> splittedCommand = utils::helpers::Conversion::splitString(commandBuffer);
                    if (splittedCommand.size() > 1UL && (U":w" == splittedCommand[0] || U":e" == splittedCommand[0]))
                    {
                        size_t lastSlashPos = splittedCommand[1UL].rfind(U"/") + 1UL;
                        pathStem = splittedCommand[1UL].substr(lastSlashPos);
                    }
                    filesInDir.clear();
                    lastFileInDir = filesInDir.cend();
                }
                if (1UL == commandBuffer.size())
                {
                    anyChangeMade = false;
                }
                candidateCommands.clear();
                candidateCommand = candidateCommands.end();
                candidateColors.clear();
                candidateColor = candidateColors.end();
                break;
            }
            case ctrl(U'w'):
                anyChangeMade = true;
                if (commandBuffer.size() > 1UL)
                {
                    std::vector<std::u32string> splittedCommand = utils::helpers::Conversion::splitString(commandBuffer);
                    if (!pathStemSecondPart.empty())
                    {
                        std::copy(pathStemSecondPart.cbegin(), pathStemSecondPart.cend(), std::back_inserter(commandBuffer));
                        pathStemSecondPart.clear();
                        setCursorAtTheEnd();
                        cursorIdx = commandBuffer.size();
                    }
                    size_t newCursorIdx = removeWordBackward(cursorIdx);
                    handleModeChange();
                    setCursor(newCursorIdx);
                    printWithoutDelay(commandBuffer, false);
                    splittedCommand = utils::helpers::Conversion::splitString(commandBuffer);
                    if (splittedCommand.size() > 1UL && (U":w" == splittedCommand[0] || U":e" == splittedCommand[0]))
                    {
                        size_t lastSlashPos = splittedCommand[1UL].rfind(U"/") + 1UL;
                        pathStem = splittedCommand[1UL].substr(lastSlashPos);
                    }
                    filesInDir.clear();
                    lastFileInDir = filesInDir.cend();
                    currBottomBarWindow->enableCursor();
                }
                if (1UL == commandBuffer.size())
                {
                    anyChangeMade = false;
                }
                candidateCommands.clear();
                candidateCommand = candidateCommands.end();
                break;
            case '\t':
            {
                if (commandBuffer.size() != utils::helpers::Lambda::getIndexFromOffset(commandBuffer, commandBufferFirstIdx, currBottomBarWindow->getCursorPosition().second, bottomBarWindowWidth))
                {
                    break;
                }
                std::vector<std::u32string> splittedCommand = utils::helpers::Conversion::splitString(commandBuffer);
                if (1UL == commandBuffer.size())
                {
                    break;
                }
                else if (splittedCommand[0] == U":w" || splittedCommand[0] == U":e")
                {
                    std::u32string path;
                    if (splittedCommand.size() > 1UL)
                    {
                        path = splittedCommand[1UL];
                        for (size_t i = 2UL; i < splittedCommand.size(); ++i)
                        {
                            path += U" " + splittedCommand[i];
                        }
                    }
                    if (fixUserInputPath(splittedCommand.size() > 1UL ? path : utils::datatypes::Strings::EMPTY_STRING, splittedCommand[0]))
                    {
                        splittedCommand = utils::helpers::Conversion::splitString(commandBuffer);
                        path = splittedCommand[1UL];
                        filesInDir = utils::helpers::Lambda::getAllFilesInDirectory((U'/' != path.back()) ? path.substr(0, path.rfind(U'/') + 1UL) : path);
                        lastFileInDir = filesInDir.cbegin();
                        pathStem = splittedCommand[1UL].substr(path.rfind(U"/") + 1UL);
                        break;
                    }
                    if (filesInDir.empty())
                    {
                        filesInDir = utils::helpers::Lambda::getAllFilesInDirectory((U'/' != path.back()) ? path.substr(0, path.rfind(U'/') + 1UL) : path);
                        lastFileInDir = filesInDir.cbegin();
                        if (filesInDir.empty())
                        {
                            break;
                        }
                    }
                    if (lastFileInDir == filesInDir.cend())
                    {
                        pathStemSecondPart.clear();
                        setCursorAtTheEnd();
                        printWithoutDelay(commandBuffer, false);
                        lastFileInDir = filesInDir.cbegin();
                    }
                    else
                    {
                        pathStem = path.substr(path.rfind(U"/") + 1UL);
                        for (; lastFileInDir != filesInDir.end(); ++lastFileInDir)
                        {
                            if (!lastFileInDir->find(pathStem) || pathStem.empty())
                            {
                                pathStemSecondPart.clear();
                                size_t oldCommandBufferSize = commandBuffer.size();
                                std::copy(std::next(lastFileInDir->begin(), pathStem.size()), lastFileInDir->end(), std::back_inserter(pathStemSecondPart));
                                std::copy(pathStemSecondPart.begin(), pathStemSecondPart.end(), std::back_inserter(commandBuffer));
                                setCursorAtTheEnd();
                                printWithoutDelay(commandBuffer, false);
                                ++lastFileInDir;
                                commandBuffer.erase(oldCommandBufferSize);
                                break;
                            }
                        }
                    }
                    break;
                }
                else if (1UL == splittedCommand.size())
                {
                    if (candidateCommands.empty())
                    {
                        for (auto &command : actionCommands)
                        {
                            if (std::u32string::npos != command.find(splittedCommand[0]))
                            {
                                candidateCommands.push_back(command);
                            }
                        }
                        candidateCommand = candidateCommands.begin();
                    }

                    if (!candidateCommands.empty())
                    {
                        if (*candidateCommand != U":stash" && *candidateCommand != U":drop" && *candidateCommand != U":reload" && candidateCommand->back() != U' ')
                        {
                            *candidateCommand += U" ";
                        }
                        commandBuffer = *candidateCommand;
                        printWithoutDelay(commandBuffer, false);
                        handleModeChange();
                        setCursorAtTheEnd();
                        printWithoutDelay(commandBuffer, false);
                        candidateCommand = (candidateCommand == std::prev(candidateCommands.end())) ? candidateCommands.begin() : std::next(candidateCommand, 1);
                    }
                }
                else if (2UL == splittedCommand.size() && splittedCommand[0] == U":color" && commandBuffer.back() != U' ')
                {
                    std::u32string lastColor = (splittedCommand[1].rfind(U'|') == std::u32string::npos) ? splittedCommand[1] : splittedCommand[1].substr(splittedCommand[1].rfind(U'|') + 1UL);
                    if (!candidateColors.empty())
                    {
                        candidateColor = std::next(candidateColor) == candidateColors.end() ? candidateColors.begin() : std::next(candidateColor);
                    }
                    else
                    {
                        for (auto &color : colorMapper)
                        {
                            if (!color.first.find(lastColor))
                            {
                                candidateColors.push_back(color.first);
                            }
                        }
                        if (!candidateColors.empty())
                        {
                            candidateColor = candidateColors.begin();
                        }
                    }

                    if (!candidateColors.empty())
                    {
                        if (lastColor == splittedCommand[1])
                        {
                            commandBuffer = U":color ";
                        }
                        else
                        {
                            commandBuffer.erase(commandBuffer.size() - lastColor.size());
                        }
                        std::copy(candidateColor->begin(), candidateColor->end(), std::back_inserter(commandBuffer));
                        if (1UL == candidateColors.size())
                        {
                            commandBuffer.push_back(U' ');
                        }
                        printWithoutDelay(commandBuffer, false);
                        handleModeChange();
                        setCursorAtTheEnd();
                        printWithoutDelay(commandBuffer, false);
                    }
                }
                break;
            }
            case 27: // QUIT
                printWithoutDelay(utils::datatypes::Strings::EMPTY_STRING, false);
                return false;
            default:
                anyChangeMade = true;
                filesInDir.clear();
                candidateColors.clear();
                candidateColor = candidateColors.end();
                lastFileInDir = filesInDir.cend();
                if (!pathStemSecondPart.empty())
                {
                    std::copy(pathStemSecondPart.cbegin(), pathStemSecondPart.cend(), std::back_inserter(commandBuffer));
                    setCursor(commandBuffer.size());
                    cursorIdx = commandBuffer.size();
                }
                if (cursorIdx > 0 && !utils::helpers::Lambda::isNewLineChar(scannedCharacter))
                {
                    commandBuffer.insert(std::next(commandBuffer.begin(), cursorIdx++), scannedCharacter);
                    while (utils::helpers::Lambda::getStringWidth(commandBuffer, commandBufferFirstIdx, cursorIdx, false, false) > bottomBarWindowWidth)
                    {
                        ++commandBufferFirstIdx;
                    }
                }
                timeout(0);
                while (static_cast<char32_t>(ERR) != (scannedCharacter = getNextCharacter(currBottomBarWindow->getLibraryWindowHandler())))
                {
                    if (utils::helpers::Lambda::isTabChar(scannedCharacter) || utils::helpers::Lambda::isNewLineChar(scannedCharacter))
                    {
                        continue;
                    }
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
                std::vector<std::u32string> splittedCommand = utils::helpers::Conversion::splitString(commandBuffer);
                if (splittedCommand.size() > 1UL && (U":w" == splittedCommand[0] || U":e" == splittedCommand[0]))
                {
                    size_t lastSlashPos = splittedCommand[1UL].rfind(U"/") + 1UL;
                    pathStem = splittedCommand[1UL].substr(lastSlashPos);
                }
                handleModeChange();
                setCursor(cursorIdx);
                printWithoutDelay(commandBuffer, false);
                pathStemSecondPart.clear();
                candidateCommands.clear();
                candidateCommand = candidateCommands.end();
                break;
            }
        }
    }

    bool ActionReader::fixUserInputPath(std::u32string const &path, std::u32string const &prefix)
    {
        std::u32string fixedPath = utils::helpers::Lambda::fixPath(path);
        if (path == fixedPath)
        {
            return false;
        }
        commandBuffer = prefix + U' ' + fixedPath;
        printWithoutDelay(commandBuffer, false);
        handleModeChange();
        setCursorAtTheEnd();
        printWithoutDelay(commandBuffer, false);
        return true;
    }

    bool ActionReader::handleCommand()
    {
        currBottomBarWindow->disableCursor();
        currBottomBarWindow->getParentWindowHandler()->disableCursor();
        std::vector<std::u32string> splittedCommand = utils::helpers::Conversion::splitString(commandBuffer);

        if (commandBuffer.size() == 1)
        {
            return false;
        }
        else if (splittedCommand[0] == U":e")
        {
            auto fileName = utils::helpers::Conversion::squeezeu32String(utils::helpers::Conversion::removeWhiteCharactersOnBothEnds(commandBuffer.substr(2UL)));
            try
            {
                fileName = utils::helpers::Lambda::getAbsolutePath(fileName, commandBuffer, filesystem);
            }
            catch (utils::exception::FeatherRuntimeException const &)
            {
                printError(utils::datatypes::Strings::UNKNOWN_PROBLEM);
                return false;
            }
            if (splittedCommand.size() == 1UL)
            {
                currBottomBarWindow->getParentWindowHandler()->getStorage()->reload();
                return false;
            }
            else if (!filesystem->isFileExist(fileName))
            {
                printError(utils::datatypes::Strings::FILE_DOES_NOT_EXIST);
                return false;
            }
            else if (filesystem->isFileDirectory(fileName))
            {
                printError(utils::datatypes::Strings::FILE_IS_FOLDER);
                return false;
            }
            auto newCursorPos = windowsManager->reloadWindowContent(utils::helpers::Lambda::getAbsolutePath(fileName, commandBuffer, filesystem), storageFactory);
            windowsManager->refreshAllWindows(currFrameVisitor);
            if (newCursorPos.has_value())
            {
                utils::helpers::Lambda::setCursor(windowsManager, printingOrchestrator, currFrameVisitor, halfFrameUpVisitor, newCursorPos.value());
            }
        }
        else if (splittedCommand[0] == U":w")
        {
            std::string fileName = currBottomBarWindow->getParentWindowHandler()->getStorage()->getName();
            std::string absolutePath;
            try
            {
                absolutePath = utils::helpers::Lambda::getAbsolutePath(fileName, commandBuffer, filesystem);
            }
            catch (utils::exception::FeatherRuntimeException const &)
            {
                printError(utils::datatypes::Strings::UNKNOWN_PROBLEM);
                return false;
            }
            std::string savedFileName = (1UL == splittedCommand.size()) ? fileName : absolutePath;
            std::u32string savedFileNameWideString = (1UL == splittedCommand.size()) ? U"Saving to current file" : U"Saving to : " + utils::helpers::Lambda::mapStringToWideString(absolutePath);
            auto lastModificationTimestamp = storageFactory->getStorage(currBottomBarWindow->getParentWindowHandler()->getStorage()->getName(), currBottomBarWindow->getParentWindowHandler()->getUUID())->getLastModificationTimestamp();
            std::string folderName = utils::helpers::Lambda::getFolderName(savedFileName);
            if (!filesystem->isFolderExist(folderName))
            {
                printError(utils::datatypes::Strings::FOLDER_DOES_NOT_EXIST);
                return false;
            }
            else if (!filesystem->hasFolderWritePermissions(folderName))
            {
                printError(utils::datatypes::Strings::FOLDER_NO_PERMISSIONS_TO_WRITE);
                return false;
            }
            else if ((1UL != splittedCommand.size()) && filesystem->isFileExist(savedFileName) && filesystem->isFileDirectory(savedFileName))
            {
                std::string currFileFullName = currBottomBarWindow->getParentWindowHandler()->getStorage()->getName();
                if (U'/' != savedFileName.back())
                {
                    savedFileName += U'/';
                }
                savedFileName = savedFileName + currFileFullName.substr(currFileFullName.rfind('/') + 1UL);
                if (filesystem->isFileExist(savedFileName))
                {
                    goto _fileExist;
                }
            }
            else if (!filesystem->hasDiskEnoughSpace(savedFileName, currBottomBarWindow->getParentWindowHandler()->getStorage()->getSize() + printingOrchestrator->getTotalBytesOfNewChanges(currBottomBarWindow->getParentWindowHandler()->getUUID()) - printingOrchestrator->getTotalBytesOfDeletions(currBottomBarWindow->getParentWindowHandler()->getUUID())))
            {
                printError(utils::datatypes::Strings::DISK_NO_SPACE_LEFT);
                return false;
            }
            else if (lastModificationTimestamp > currBottomBarWindow->getParentWindowHandler()->getStorage()->getLastModificationTimestamp())
            {
                currentFeatherMode = utils::FEATHER_MODE::READ_MODE_WAITING_FOR_INPUT;
                printWithoutDelay(utils::datatypes::Strings::FILE_WAS_MODIFIED);
                char32_t scannedCharacter = 0;
            _begin_1:
                switch (scannedCharacter = getNextCharacter(currBottomBarWindow->getLibraryWindowHandler()))
                {
                case static_cast<char32_t>(ERR):
                    goto _begin_1;
                case KEY_RESIZE:
                {
                    terminalDimensionsChangeHandler(utils::datatypes::Strings::FILE_WAS_MODIFIED, 0);
                    goto _begin_1;
                }
                case U'n':
                case U'N':
                    return false;
                case U'y':
                case U'Y':
                    break;
                default:
                    goto _begin_1;
                }
            }

            if (1UL == splittedCommand.size())
            {
                if (!printingOrchestrator->isAnyChangesMade(currBottomBarWindow->getParentWindowHandler()->getUUID()))
                {
                    printError(utils::datatypes::Strings::NO_CHANGES_MADE);
                    return false;
                }
            }
            else
            {
                if (filesystem->isFileExist(savedFileName))
                {
                _fileExist:
                    currentFeatherMode = utils::FEATHER_MODE::READ_MODE_WAITING_FOR_INPUT;
                    printWithoutDelay(utils::datatypes::Strings::FILE_ALREADY_EXIST);
                _begin:
                    char32_t scannedCharacter = 0;
                    switch (scannedCharacter = getNextCharacter(currBottomBarWindow->getLibraryWindowHandler()))
                    {
                    case static_cast<char32_t>(ERR):
                        goto _begin;
                    case KEY_RESIZE:
                    {
                        terminalDimensionsChangeHandler(utils::datatypes::Strings::FILE_ALREADY_EXIST, 0);
                        goto _begin;
                    }
                    case U'n':
                    case U'N':
                        return false;
                    case U'y':
                    case U'Y':
                        break;
                    default:
                        goto _begin;
                    }
                }
            }
            //blockSignals();
            auto actionResult = actionHandler->handle(std::make_unique<action::SaveChangesAction>(action::SaveChangesActionInput(printingOrchestrator, storageFactory, currBottomBarWindow->getParentWindowHandler()->getUUID(), currBottomBarWindow->getParentWindowHandler()->getBufferFiller(), currBottomBarWindow->getParentWindowHandler()->getStorage(), windowsManager, savedFileName, currFrameVisitor, changesStack, savedFileNameWideString)));
            try
            {
                unblockSignals();
                if ((utils::datatypes::ERROR::INTERRUPTED == actionResult.result) || (utils::datatypes::ERROR::FILE_NO_LONGER_AVAILABLE == actionResult.result))
                {
                    if (utils::datatypes::ERROR::INTERRUPTED == actionResult.result)
                    {
                        utils::helpers::Lambda::subWindowPrint(utils::datatypes::Strings::SAVE_INTERRUPTED, std::nullopt, currBottomBarWindow, windowsManager->getSubWindow(windowsManager->getProgressBarWindow(currBottomBarWindow->getParentWindowHandler()->getUUID()).value()), currFrameVisitor);
                    _begin_2:
                        char32_t scannedCharacter = ERR;
                        switch (scannedCharacter = getNextCharacter(currBottomBarWindow->getLibraryWindowHandler()))
                        {
                        case static_cast<char32_t>(ERR):
                            goto _begin_2;
                        case KEY_RESIZE:
                        {
                            terminalDimensionsChangeHandler(utils::datatypes::Strings::SAVE_INTERRUPTED, 0);
                            goto _begin_2;
                        }
                        case U'n':
                        case U'N':
                            break;
                        case U'y':
                        case U'Y':
                        {
                            std::string junkFile = actionResult.junkFile.value();
                            storageFactory->removeStorage(std::move(junkFile));
                            break;
                        }
                        default:
                            goto _begin_2;
                        }
                    }
                    else
                    {
                        printError(utils::datatypes::Strings::FILE_DELETED_DURING_SAVING);
                    }
                    printWithoutDelay(utils::datatypes::Strings::EMPTY_STRING, false);
                    currentFeatherMode = utils::FEATHER_MODE::ACTION_MODE;
                    windowsManager->refreshAllWindows(currFrameVisitor);
                    return true;
                }
                else
                {
                    print(utils::datatypes::Strings::SAVE_SUCCESS, true);
                    return true;
                }
            }
            catch (std::exception const &e)
            {
                throw e;
            }
            return false;
        }
        else if (isInputAllDigits())
        {
            size_t lineNumber = utils::helpers::Conversion::u32StringTosize_t({std::next(commandBuffer.begin(), 1UL), commandBuffer.end()});
            try
            {
                auto result = actionHandler->handle(std::make_unique<action::GotoLineAction>(action::GotoLineActionInput(lineNumber, keyReader, currBottomBarWindow->getParentWindowHandler()->getUUID(), currBottomBarWindow->getParentWindowHandler()->getStorage(), printingOrchestrator, windowsManager, currFrameVisitor))).result;
                if (!result.has_value())
                {
                    printError(utils::datatypes::Strings::LINE_DOES_NOT_EXIST);
                }
                else
                {
                    currBottomBarWindow->getParentWindowHandler()->setCursorFromRealPosition(
                        result.value(),
                        printingOrchestrator,
                        halfFrameUpVisitor);
                    currBottomBarWindow->getParentWindowHandler()->setCursorFromRealPosition(
                        actionHandler->handle(std::make_unique<action::SkipEmptyCharactersAction>(action::SkipEmptyCharactersActionInput(currBottomBarWindow->getParentWindowHandler(), printingOrchestrator, windowsManager))).getResult(),
                        printingOrchestrator,
                        halfFrameUpVisitor);
                    pair newCursorPos = currBottomBarWindow->getParentWindowHandler()->getCursorRealPosition(printingOrchestrator.get());
                    windowsManager->refreshProgresWindow(currFrameVisitor, currBottomBarWindow->getParentWindowHandler()->getUUID(), std::make_pair(utils::windows::FramePositions{newCursorPos, currBottomBarWindow->getParentWindowHandler()->getFramePosition().endFramePosition}, newCursorPos));
                }
            }
            catch (utils::exception::FeatherInterruptedException &)
            {
                // Do nothing
            }
            return true;
        }
        else if (U":jump" == splittedCommand[0])
        {
            if (splittedCommand.size() < 2)
            {
                printError(utils::datatypes::Strings::NOT_ENOUGH_ARGUMENTS);
            }
            if (splittedCommand[1].back() == U'B' || splittedCommand[1].back() == U'b')
            {
                splittedCommand[1].pop_back();
            }
            if (!std::all_of(splittedCommand[1].begin(), splittedCommand[1].end(), [](char32_t c)
                             { return utils::Utf8Util::isDigit(c); }))
            {
                printError(utils::datatypes::Strings::BYTE_DOES_NOT_EXIST);
            }
            else
            {
                size_t byteNumber = utils::helpers::Conversion::u32StringTosize_t(splittedCommand[1]);
                currBottomBarWindow->getParentWindowHandler()->setCursorFromRealPosition(
                    actionHandler->handle(std::make_unique<action::MoveToByteAction>(action::MoveToByteActionInput(byteNumber, currBottomBarWindow->getParentWindowHandler()->getUUID(), currBottomBarWindow->getParentWindowHandler()->getStorage(), printingOrchestrator))).getResult(),
                    printingOrchestrator,
                    halfFrameUpVisitor);
                pair newCursorPos = currBottomBarWindow->getParentWindowHandler()->getCursorRealPosition(printingOrchestrator.get());
                windowsManager->refreshProgresWindow(currFrameVisitor, currBottomBarWindow->getParentWindowHandler()->getUUID(), std::make_pair(utils::windows::FramePositions{newCursorPos, currBottomBarWindow->getParentWindowHandler()->getFramePosition().endFramePosition}, newCursorPos));
            }
            return true;
        }
        else if (U":color" == splittedCommand[0])
        {
            std::vector<std::u32string> splittedCommand = utils::helpers::Conversion::splitStringWithQuote(commandBuffer);
            if (splittedCommand.size() == 2)
            {
                printError(utils::datatypes::Strings::INVALID_REGEX);
                return false;
            }
            else if (splittedCommand.size() < 3)
            {
                printError(utils::datatypes::Strings::NOT_ENOUGH_OR_WRONG_ARGUMENTS);
                return false;
            }
            else if (!utils::algorithm::RegexSearch::isValidRegex(splittedCommand[2]))
            {
                printError(utils::datatypes::Strings::INVALID_REGEX);
                return false;
            }
            std::set<utils::datatypes::COLOR> colors = parseColors(splittedCommand[1UL]);
            std::set<utils::datatypes::COLOR> disjointSet = colors;
            if (disjointSet.count(utils::datatypes::COLOR::FEATHER_COLOR_WHITE))
            {
                disjointSet.erase(utils::datatypes::COLOR::FEATHER_COLOR_WHITE);
            }
            if (disjointSet.count(utils::datatypes::COLOR::FEATHER_COLOR_BLACK))
            {
                disjointSet.erase(utils::datatypes::COLOR::FEATHER_COLOR_BLACK);
            }
            if (!configuration->isTerminalSupportColors())
            {
                printError(utils::datatypes::Strings::TERMINAL_NOT_SUPPORT_COLORS);
                return false;
            }
            if (colors.empty())
            {
                printError(utils::datatypes::Strings::UNSUPPORTED_COLOR);
            }
            else
            {
                for (size_t wordIdx = 2UL; wordIdx < 3UL; ++wordIdx)
                {
                    if (configuration->isHexMode())
                    {
                        printingOrchestrator->highlightText(utils::helpers::Conversion::getColoredStringHexMode(splittedCommand[wordIdx]), colors, currBottomBarWindow->getParentWindowHandler()->getUUID(true));
                    }
                    else
                    {
                        printingOrchestrator->highlightText(splittedCommand[wordIdx], colors, currBottomBarWindow->getParentWindowHandler()->getUUID());
                    }
                }
                // changesStack->addChange(utils::datatypes::ChangeType::HIGHLIGHT_TEXT, currBottomBarWindow->getParentWindowHandler()->getUUID(), colorMapper[splittedCommand[1]], strings);
            }
        }
        else if (U":uncolor" == splittedCommand[0])
        {
            std::vector<std::u32string> splittedCommand = utils::helpers::Conversion::splitStringWithQuote(commandBuffer);
            if (splittedCommand.size() == 1)
            {
                printError(utils::datatypes::Strings::INVALID_REGEX);
                return false;
            }
            else if (splittedCommand.size() < 2)
            {
                printError(utils::datatypes::Strings::NOT_ENOUGH_ARGUMENTS);
            }
            else
            {
                for (size_t wordIdx = 1; wordIdx < splittedCommand.size(); ++wordIdx)
                {
                    if (configuration->isHexMode())
                    {
                        printingOrchestrator->unhighlightText(utils::helpers::Conversion::getColoredStringHexMode(splittedCommand[wordIdx]), currBottomBarWindow->getParentWindowHandler()->getUUID(true));
                    }
                    else
                    {
                        printingOrchestrator->unhighlightText(splittedCommand[wordIdx], currBottomBarWindow->getParentWindowHandler()->getUUID());
                    }
                }
            }
        }
        else if (U":stash" == splittedCommand[0])
        {
            if (!configuration->isCacheOn())
            {
                printError(utils::datatypes::Strings::CACHE_TURNED_OFF);
            }
            else
            {
                size_t dumpedBytes = printingOrchestrator->dumpCache(currBottomBarWindow->getParentWindowHandler()->getUUID());
                if (0 == dumpedBytes)
                {
                    printError(utils::datatypes::Strings::CACHE_NOTHING_TO_STASH);
                }
                else
                {
                    print(utils::datatypes::Strings::CACHE_STASHED, true);
                }
            }
        }
        else if (U":reload" == splittedCommand[0])
        {
            currBottomBarWindow->getParentWindowHandler()->getStorage()->reload();
        }
        else if (U":drop" == splittedCommand[0])
        {
            if (breakPoints.empty())
            {
                printError(utils::datatypes::Strings::NO_BREAKPOINTS_SET);
            }
            else
            {
                breakPoints.clear();
                breakPointsAdded.clear();
                currentBreakPoint = breakPoints.end();
                print(utils::datatypes::Strings::DROPPPED_ALL_BREAKPOINTS, true);
            }
        }
        /*
    else if (U":replaceall" == splittedCommand[0])
    {
        if (splittedCommand.size() < 2)
        {
            printError(utils::datatypes::Strings::NOT_ENOUGH_ARGUMENTS);
        }
        else
        {
            std::u32string replaceString(splittedCommand[1]);
            if (replaceString.size() < 3UL)
            {
                printError(utils::datatypes::Strings::INVALID_REPLACE_STRING);
            }
            size_t noBareSlashes = 0;
            size_t middleSlashPos = 0;
            for (int idx = 0UL; idx < replaceString.size(); ++idx)
            {
                if (replaceString[idx] == '/' && (0 == idx || replaceString[idx - 1] != '\\'))
                {
                    ++noBareSlashes;
                    if (2 == noBareSlashes)
                    {
                        middleSlashPos = idx;
                    }
                }
            }
            if (noBareSlashes != 3UL)
            {
                printError(utils::datatypes::Strings::INVALID_REPLACE_STRING);
                return false;
            }
            replaceString = replaceString.substr(1, replaceString.size() - 2UL);
            std::u32string replaceFrom = utils::helpers::Conversion::escapeSlashCharacters(replaceString.substr(0, middleSlashPos - 1UL));
            std::u32string replaceTo = utils::helpers::Conversion::escapeSlashCharacters(replaceString.substr(middleSlashPos));
            replaceFrom = utils::helpers::Conversion::escapeSpecialCharacters(replaceFrom);
            replaceTo = utils::helpers::Conversion::escapeSpecialCharacters(replaceTo);
            std::u32string replaceFromTmp(replaceFrom);
            action::SearchSubstringActionInput searchSubstringActionInput(currBottomBarWindow->getParentWindowHandler()->getUUID(), std::move(replaceFromTmp), storageFactory, printingOrchestrator, currBottomBarWindow->getParentWindowHandler()->getBufferFiller(), windowsManager, currFrameVisitor, utils::Direction::DOWN);
            auto removeAllInput = action::RemoveAllActionInput(searchSubstringActionInput, currBottomBarWindow->getParentWindowHandler()->getUUID(), windowsManager, currBottomBarWindow->getParentWindowHandler()->getCursorRealPosition(printingOrchestrator.get()), currBottomBarWindow->getParentWindowHandler()->getStorage(), printingOrchestrator, utils::helpers::Conversion::squeezeu32String(splittedCommand[1]).length());
            actionHandler->handle(std::make_unique<action::ReplaceAllAction>(action::ReplaceAllActionInput(removeAllInput, currBottomBarWindow->getParentWindowHandler()->getUUID(), windowsManager, storageFactory, currBottomBarWindow->getParentWindowHandler()->getBufferFiller(), currFrameVisitor, pair(0, 0), currBottomBarWindow->getParentWindowHandler()->getStorage(), printingOrchestrator, replaceFrom, replaceTo)));
        }
    }
    */
        else if (U":removeuntil" == splittedCommand[0])
        {
            if (splittedCommand.size() < 2)
            {
                printError(utils::datatypes::Strings::NOT_ENOUGH_ARGUMENTS);
            }
            else if (splittedCommand[1].length() > 1UL && splittedCommand[1][0] != U'!' && splittedCommand[1] != U"\\n" && splittedCommand[1] != U"\\t" && splittedCommand[1] != U"\\s")
            {
                printError(utils::datatypes::Strings::REMOVE_UNTIL_SINGLE_CHARACTER_ALLOWED);
            }
            else if (splittedCommand[1].length() > 2UL && splittedCommand[1][0] == U'!' && splittedCommand[1].substr(1UL) != U"\\n" && splittedCommand[1].substr(1UL) != U"\\t" && splittedCommand[1].substr(1UL) != U"\\s")
            {
                printError(utils::datatypes::Strings::REMOVE_UNTIL_SINGLE_CHARACTER_ALLOWED);
            }
            else
            {
                bool notEqual = false;
                char32_t searchedCharacter = 0;
                if ((splittedCommand[1][0] == U'!') && (splittedCommand[1].length() > 1))
                {
                    notEqual = true;
                    searchedCharacter = splittedCommand[1].substr(1UL) == U"\\n" ? U'\n' : splittedCommand[1].substr(1UL) == U"\\t" ? U'\t'
                                                                                       : splittedCommand[1].substr(1UL) == U"\\s"   ? U' '
                                                                                                                                    : splittedCommand[1][1];
                }
                else
                {
                    searchedCharacter = splittedCommand[1] == U"\\n" ? U'\n' : splittedCommand[1] == U"\\t" ? U'\t'
                                                                           : splittedCommand[1] == U"\\s"   ? U' '
                                                                                                            : splittedCommand[1][0];
                }
                auto const deletionStart = currBottomBarWindow->getParentWindowHandler()->getCursorRealPosition(printingOrchestrator.get());
                try
                {
                    auto savedFeatherMode = currentFeatherMode;
                    auto const deletionEnd = utils::helpers::Lambda::findNext(
                                             deletionStart, [&](char32_t c)
                                             { return notEqual ? (c != searchedCharacter) : (c == searchedCharacter); },
                                             printingOrchestrator, currBottomBarWindow->getParentWindowHandler()->getUUID(), currBottomBarWindow->getParentWindowHandler()->getStorage(), windowsManager, false, true)
                                             .first;
                    utils::helpers::Lambda::restoreFeatherMode(windowsManager, currFrameVisitor, savedFeatherMode);
                    auto lastValidPosition = printingOrchestrator->getLastValidPosition(currBottomBarWindow->getParentWindowHandler()->getUUID(), currBottomBarWindow->getParentWindowHandler()->getStorage());
                    if (lastValidPosition < deletionEnd)
                    {
                        printError(utils::datatypes::Strings::NOT_FOUND);
                    }
                    else
                    {
                        printingOrchestrator->removeCharacters(currBottomBarWindow->getParentWindowHandler()->getUUID(), currBottomBarWindow->getParentWindowHandler()->getStorage(), deletionStart, deletionEnd);
                        if (configuration->isHexMode())
                        {
                            auto hexWindow = windowsManager->getMainWindow(windowsManager->getSecondaryActiveMainWindowForHexMode());
                            printingOrchestrator->removeCharacters(hexWindow->getUUID(), hexWindow->getStorage(), deletionStart, deletionEnd);
                        }
                    }
                }
                catch (utils::exception::FeatherInterruptedException &e)
                {
                    return false;
                }
            }
        }
        else if (U":copyuntil" == splittedCommand[0])
        {
            if (splittedCommand.size() < 2)
            {
                printError(utils::datatypes::Strings::NOT_ENOUGH_ARGUMENTS);
            }
            else
            {
                bool notEqual = false;
                auto searchedCharacter = splittedCommand[1][0];
                if ((splittedCommand[1][0] == U'!') && (splittedCommand[1].length() > 1))
                {
                    notEqual = true;
                    searchedCharacter = splittedCommand[1][1];
                }
                auto const copyStart = currBottomBarWindow->getParentWindowHandler()->getCursorRealPosition(printingOrchestrator.get());
                try
                {
                    auto savedFeatherMode = currentFeatherMode;
                    auto const copyEnd = utils::helpers::Lambda::findNext(
                                         copyStart, [&](char32_t c)
                                         { return notEqual ? (c != searchedCharacter) : (c == searchedCharacter); },
                                         printingOrchestrator, currBottomBarWindow->getParentWindowHandler()->getUUID(), currBottomBarWindow->getParentWindowHandler()->getStorage(), windowsManager, false, true)
                                         .first;
                    utils::helpers::Lambda::restoreFeatherMode(windowsManager, currFrameVisitor, savedFeatherMode);
                    auto lastValidPosition = printingOrchestrator->getLastValidPosition(currBottomBarWindow->getParentWindowHandler()->getUUID(), currBottomBarWindow->getParentWindowHandler()->getStorage());
                    if (lastValidPosition < copyEnd)
                    {
                        printError(utils::datatypes::Strings::NOT_FOUND);
                    }
                    else
                    {
                        printingOrchestrator->setTextToCut(std::nullopt, std::nullopt);
                        printingOrchestrator->setLineToCopy(std::nullopt, std::nullopt);
                        printingOrchestrator->setTextToCopy(std::make_pair(std::make_pair(currBottomBarWindow->getParentWindowHandler()->getUUID(), currBottomBarWindow->getParentWindowHandler()->getStorage()->getAbsolutePath()), std::make_pair(copyStart, copyEnd)), std::make_pair(std::make_pair(currBottomBarWindow->getParentWindowHandler()->getUUID(), utils::helpers::Lambda::getAbsolutePath(currBottomBarWindow->getParentWindowHandler()->getStorage()->getName(), U"", filesystem)), utils::datatypes::DeletionsSnapshot{printingOrchestrator->getTotalBytesOfDeletions(currBottomBarWindow->getParentWindowHandler()->getUUID()), printingOrchestrator->getDeletionsPositions(currBottomBarWindow->getParentWindowHandler()->getUUID()), printingOrchestrator->getReverseDeletionsPositions(currBottomBarWindow->getParentWindowHandler()->getUUID())}));
                        printingOrchestrator->setTextToCopyPositionsToSkip(std::make_pair(std::make_pair(currBottomBarWindow->getParentWindowHandler()->getUUID(), currBottomBarWindow->getParentWindowHandler()->getStorage()->getName()), std::map<pair, pair>{}));
                    }
                }
                catch (utils::exception::FeatherInterruptedException &e)
                {
                    return false;
                }
            }
        }
        else
        {
            printError(utils::datatypes::Strings::UNRECOGNIZED_COMMAND);
        }
        return false;
    }

    bool ActionReader::isInputAllDigits() const
    {
        return std::all_of(std::next(commandBuffer.begin(), 1), commandBuffer.end(), [](char32_t c)
                           { return utils::Utf8Util::isDigit(c); });
    }

    void ActionReader::handleModeChange()
    {
        std::vector<std::u32string> splittedCommand = utils::helpers::Conversion::splitString(commandBuffer);
        if (splittedCommand[0] == U":color")
        {
            currentFeatherMode = utils::FEATHER_MODE::ACTION_MODE_COLOR;
        }
        else if (splittedCommand[0] == U":drop")
        {
            currentFeatherMode = utils::FEATHER_MODE::ACTION_MODE_DROP_BREAKPOINTS;
        }
        /*else if (splittedCommand[0] == U":replaceall")
    {
        currentFeatherMode = utils::FEATHER_MODE::ACTION_MODE_REPLACE_ALL;
    }
    */
        else if (splittedCommand[0] == U":removeuntil")
        {
            currentFeatherMode = utils::FEATHER_MODE::ACTION_MODE_REMOVE_UNTIL;
        }
        else if (splittedCommand[0] == U":reload")
        {
            currentFeatherMode = utils::FEATHER_MODE::ACTION_MODE_RELOAD;
        }
        else if (splittedCommand[0] == U":stash")
        {
            currentFeatherMode = utils::FEATHER_MODE::ACTION_MODE_STASH_CHANGES;
        }
        else if (splittedCommand[0] == U":copyuntil")
        {
            currentFeatherMode = utils::FEATHER_MODE::ACTION_MODE_COPY_UNTIL;
        }
        else if (splittedCommand[0] == U":uncolor")
        {
            currentFeatherMode = utils::FEATHER_MODE::ACTION_MODE_UNCOLOR;
        }
        else if (splittedCommand[0] == U":w" || splittedCommand[0] == U":e")
        {
            currentFeatherMode = utils::FEATHER_MODE::ACTION_MODE_SAVE;
        }
        else
        {
            currentFeatherMode = utils::FEATHER_MODE::ACTION_MODE;
        }
        windowsManager->refreshInfoWindow(currFrameVisitor, currBottomBarWindow->getParentWindowHandler()->getUUID());
    }

    std::set<utils::datatypes::COLOR> ActionReader::parseColors(std::u32string const &input)
    {
        std::set<utils::datatypes::COLOR> colors;
        size_t nextPos = 0, lastPos = 0;
        while ((nextPos = input.find(U'|', lastPos)) != std::u32string::npos)
        {
            auto color = input.substr(lastPos, nextPos - lastPos);
            color.erase(std::remove_if(color.begin(), color.end(), utils::helpers::Lambda::isSpace), color.end());
            if (!colorMapper.count(color))
            {
                return std::set<utils::datatypes::COLOR>();
            }
            colors.insert(colorMapper[color]);
            lastPos = nextPos + 1UL;
        }
        auto color = input.substr(lastPos);
        if (!colorMapper.count(color))
        {
            return std::set<utils::datatypes::COLOR>();
        }
        color.erase(std::remove_if(color.begin(), color.end(), utils::helpers::Lambda::isSpace), color.end());
        colors.insert(colorMapper[color]);
        return colors;
    }

    ActionReader::~ActionReader()
    {
        dynamic_cast<windows::NCursesBottomBarWindowDecorator *>(currBottomBarWindow.get())->setActionHistoryIteratorAtEnd();
        dynamic_cast<windows::NCursesBottomBarWindowDecorator *>(currBottomBarWindow.get())->setReader(nullptr);
    }

    std::set<std::u32string> ActionReader::actionCommands = {
        U":color",
        U":drop",
        U":stash",
        U":jump",
        U":reload",
        U":uncolor",
        // U":replaceall",
        U":removeuntil",
        U":copyuntil"};

    std::map<std::u32string, utils::datatypes::COLOR> ActionReader::colorMapper = {
        {U"black", utils::datatypes::COLOR::FEATHER_COLOR_BLACK},
        {U"red", utils::datatypes::COLOR::FEATHER_COLOR_RED},
        {U"green", utils::datatypes::COLOR::FEATHER_COLOR_GREEN},
        {U"yellow", utils::datatypes::COLOR::FEATHER_COLOR_YELLOW},
        {U"blue", utils::datatypes::COLOR::FEATHER_COLOR_BLUE},
        {U"magenta", utils::datatypes::COLOR::FEATHER_COLOR_MAGENTA},
        {U"cyan", utils::datatypes::COLOR::FEATHER_COLOR_CYAN},
        {U"white", utils::datatypes::COLOR::FEATHER_COLOR_WHITE}};

} // namespace feather::reader