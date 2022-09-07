#include "utils/Utf8Util.hpp"
#include "utils/MoveCursorOnScreenStateMachine.hpp"
#include "windows/NCursesWindow.hpp"

extern std::unique_ptr<const feather::config::Configuration> configuration;

namespace feather::windows
{
    NCursesWindow::NCursesWindow()
    {
        this->cursorIsEnabled = false;
        this->windowIsVisible = false;
        this->uuid = utils::datatypes::Uuid();
        this->cursorPosition = {0, 0};
    }

    NCursesWindow::NCursesWindow(utils::NCursesWrapper *nCursesWrapper, std::optional<utils::datatypes::Uuid> existingWindowUUID)
        : cursorIsEnabled(false), windowIsVisible(false), cursorPosition{0, 0}, ncurses(nCursesWrapper)
    {
        if (existingWindowUUID.has_value())
        {
            uuid = existingWindowUUID.value();
        }
        else
        {
            uuid = utils::datatypes::Uuid();
        }
    }

    NCursesWindow::NCursesWindow(utils::NCursesWrapper *nCursesWrapper)
        : cursorIsEnabled(false), windowIsVisible(false), cursorPosition{0, 0}, ncurses(nCursesWrapper)
    {
        uuid = utils::datatypes::Uuid();
    }

    void NCursesWindow::destroyDisplayedWindow() const
    {
        ncurses.reset(nullptr);
    }

    void NCursesWindow::enableCursor()
    {
        if (!utils::helpers::Lambda::isScreenBigEnough())
        {
            return;
        }
        cursorIsEnabled = true;
        ncurses->enableCursor();
    }

    void NCursesWindow::disableCursor()
    {
        cursorIsEnabled = false;
        ncurses->disableCursor();
    }

    void NCursesWindow::hide()
    {
        windowIsVisible = false;
        ncurses->hidePanel();
    }

    std::pair<pair, bool> NCursesWindow::moveCursor(feather::utils::Direction direction, std::shared_ptr<utils::windows::WindowInformationInterface> windowInfo, std::optional<size_t> lastLineUpSize, utils::ScreenBuffer const &internalBuffer, bool isSecondaryHexModeWindow)
    {
        if (!utils::helpers::Lambda::isScreenBigEnough())
        {
            return std::make_pair(pair(0, 0), false);
        }
        pair currCursorPos = getCursorPosition();
        pair newCursorPosition = utils::MoveCursorOnScreenStateMachine::getCursorNewPosition(currCursorPos.first, currCursorPos.second, getNoRows(), getNoColumns(), windowInfo, direction, lastLineUpSize, internalBuffer, isSecondaryHexModeWindow);
        setCursorAtPos(newCursorPosition);
        return std::make_pair(newCursorPosition, internalBuffer.highlightBrackets(pair(newCursorPosition.first, utils::MoveCursorOnScreenStateMachine::mapCursorColumnToScreenBufferColumn(newCursorPosition, internalBuffer, isSecondaryHexModeWindow)), isSecondaryHexModeWindow));
    }

    pair NCursesWindow::getCursorPosition(bool callerIsMainWindow) const
    {
        if (callerIsMainWindow && !utils::helpers::Lambda::isScreenBigEnough())
        {
            return pair(0, 0);
        }
        return cursorPosition;
    }

    pair NCursesWindow::getWindowDimensions() const
    {
        if (!utils::helpers::Lambda::isScreenBigEnough())
        {
            return pair(0, 0);
        }
        return std::make_pair(ncurses->getNoRows(), ncurses->getNoColumns());
    }

    utils::datatypes::Uuid const &NCursesWindow::getUUID() const
    {
        return uuid;
    }

    void NCursesWindow::drawWindowFrame()
    {
        if (!utils::helpers::Lambda::isScreenBigEnough())
        {
            return;
        }
        if (windowIsVisible)
        {
            ncurses->drawWindowFrame();
        }
    }

    bool NCursesWindow::isCursorEnabled() const
    {
        return cursorIsEnabled;
    }

    bool NCursesWindow::isWindowVisible() const
    {
        return windowIsVisible;
    }

    void NCursesWindow::print(utils::ScreenBuffer const &newBuffer, bool callerIsMainWindow)
    {
#ifndef _FEATHER_TEST_
        if (!utils::helpers::Lambda::isScreenBigEnough())
        {
            return;
        }
#endif
        if (windowIsVisible)
        {
            ncurses->clearWindow();
            std::vector<wchar_t> theSameColorChunk;
            std::set<utils::datatypes::COLOR> theSameChunkColor;
            pair theSameChunkStartPos = {0, 0};
            bool theSameColorHighlighted = false;
            std::unordered_map<std::set<utils::datatypes::COLOR>, std::map<pair, std::pair<std::vector<wchar_t>, bool>>, utils::datatypes::ColorHash> colorMap;
            for (size_t row = 0; row < newBuffer.getFilledRows(); ++row)
            {
                size_t columnOnScreen = 0;
                for (size_t col = 0; col < newBuffer[row].size(); ++col)
                {
                    auto currentColor = (!configuration->isHexMode() && newBuffer.isPosMappedToHex(pair(row, col)) && newBuffer.getColor(row, col).empty()) ? std::set<utils::datatypes::COLOR>{utils::datatypes::COLOR::FEATHER_COLOR_GREY} : newBuffer.getColor(row, col);
                    bool isCurrCharHighlighted = (newBuffer.isSearchedTextPos(pair(row, col)) || newBuffer.isBracket(pair(row, col)));
                    if (theSameColorChunk.empty())
                    {
                        if (!utils::helpers::Lambda::isTabChar(newBuffer[row][col]))
                        {
                            theSameColorChunk.push_back(utils::Utf8Util::getCodePoint(newBuffer[row][col], callerIsMainWindow));
                            theSameChunkColor = currentColor;
                            theSameChunkStartPos = pair(row, columnOnScreen);
                            theSameColorHighlighted = isCurrCharHighlighted;
                        }
                        else
                        {
                            for (size_t i = 0; i < configuration->getTabWidth(); ++i)
                            {
                                theSameColorChunk.push_back(utils::Utf8Util::getCodePoint(U' ', callerIsMainWindow));
                            }
                            theSameChunkColor = currentColor;
                            theSameChunkStartPos = pair(row, columnOnScreen);
                            theSameColorHighlighted = isCurrCharHighlighted;
                        }
                    }
                    else if (theSameColorHighlighted != isCurrCharHighlighted)
                    {
                        colorMap[theSameChunkColor][theSameChunkStartPos] = {theSameColorChunk, theSameColorHighlighted};
                        theSameColorChunk.clear();
                        if (!utils::helpers::Lambda::isTabChar(newBuffer[row][col]))
                        {
                            --col;
                            continue;
                        }
                    }
                    else if (theSameChunkColor == currentColor)
                    {
                        if (utils::helpers::Lambda::isTabChar(newBuffer[row][col]))
                        {
                            for (size_t i = 0; i < configuration->getTabWidth(); ++i)
                            {
                                theSameColorChunk.push_back(utils::Utf8Util::getCodePoint(U' ', callerIsMainWindow));
                            }
                        }
                        else
                        {
                            theSameColorChunk.push_back(utils::Utf8Util::getCodePoint(newBuffer[row][col], callerIsMainWindow));
                        }
                    }
                    else
                    {
                        colorMap[theSameChunkColor][theSameChunkStartPos] = {theSameColorChunk, theSameColorHighlighted};
                        theSameColorChunk.clear();
                        --col;
                        continue;
                    }
                    columnOnScreen += (callerIsMainWindow && configuration->isHexMode()) ? 1UL : utils::helpers::Lambda::getCharacterWidth(newBuffer[row][col], callerIsMainWindow, false);
                }
                if (!theSameColorChunk.empty())
                {
                    colorMap[theSameChunkColor][theSameChunkStartPos] = {theSameColorChunk, theSameColorHighlighted};
                }
                theSameColorChunk.clear();
            }

            /* Check if cursor for second main windows is not at very end */
            if (0 == newBuffer.getFilledRows())
            {
                if (newBuffer.isSearchedTextPos(pair(0, 0)))
                {
                    colorMap[newBuffer.getColor(0, 0)][pair(0, 0)] = {std::vector<wchar_t>{' '}, true};
                }
            }
            else
            {
                size_t lastRow = newBuffer.getFilledRows() - 1UL;
                if (newBuffer.isSearchedTextPos(pair(lastRow, newBuffer[lastRow].size())))
                {
                    colorMap[newBuffer.getColor(lastRow, newBuffer[lastRow].size())][pair(lastRow, newBuffer[lastRow].size())] = {std::vector<wchar_t>{' '}, true};
                }
            }
            for (auto &t : colorMap)
            {
                ncurses->print(t);
            }
            ncurses->refreshWindow();
        }
    }

    void NCursesWindow::setCursorAtPos(pair pos, bool isMainWindow) const
    {
        if (isMainWindow && !utils::helpers::Lambda::isScreenBigEnough())
        {
            return;
        }
        cursorPosition = pos;
        ncurses->setCursorPosition(pos);
    }

    void NCursesWindow::show()
    {
        this->windowIsVisible = true;
        this->ncurses->showPanel();
    }

    void NCursesWindow::switchWrapper(utils::NCursesWrapper *newNCursesWindow) const
    {
        ncurses.reset(newNCursesWindow);
        if (cursorIsEnabled)
        {
            ncurses->setCursorPosition(cursorPosition);
            ncurses->enableCursor();
        }
        else
        {
            ncurses->disableCursor();
        }
        if (windowIsVisible)
        {
            ncurses->showPanel();
        }
        else
        {
            ncurses->hidePanel();
        }
    }

    size_t NCursesWindow::getNoRows() const
    {
        if (!utils::helpers::Lambda::isScreenBigEnough())
        {
            return 0;
        }
        return ncurses->getNoRows();
    }

    size_t NCursesWindow::getNoColumns() const
    {
        return ncurses->getNoColumns();
    }

    int NCursesWindow::getCharacter() const
    {
        return ncurses->getCharacter();
    }

    void NCursesWindow::resizeWindow(size_t newWindowStartRow, size_t newWindowStartColumn, size_t newWindowNoRows, size_t newWindowNoColumns)
    {
        if (!utils::helpers::Lambda::isScreenBigEnough())
        {
            return;
        }
        ncurses->resizeWindow(newWindowStartRow, newWindowStartColumn, newWindowNoRows, newWindowNoColumns);
    }

    void NCursesWindow::moveWindow(size_t newWindowStartRow, size_t newWindowStartColumn)
    {
        if (!utils::helpers::Lambda::isScreenBigEnough())
        {
            return;
        }
        ncurses->moveWindow(newWindowStartRow, newWindowStartColumn);
    }

    void NCursesWindow::refreshWindow() const
    {
        ncurses->refreshWindow();
    }

    void NCursesWindow::setBlockingRead() const
    {
        ncurses->setBlockingRead();
    }

    void NCursesWindow::setNonBlockingRead() const
    {
        ncurses->setNonBlockingRead();
    }

    NCursesWindow::~NCursesWindow()
    {
    }
} // namespace feather::windows
