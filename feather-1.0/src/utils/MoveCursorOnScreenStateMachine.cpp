#include "config/Config.hpp"
#include "utils/FeatherMode.hpp"
#include "utils/helpers/Lambda.hpp"
#include "utils/MoveCursorOnScreenStateMachine.hpp"
#include "utils/windows/LinesAroundPositionInformation.hpp"

extern feather::utils::FEATHER_MODE currentFeatherMode;
extern std::unique_ptr<const feather::config::Configuration> configuration;

namespace feather::utils
{
    pair MoveCursorOnScreenStateMachine::getCursorNewPosition(size_t screenCursorRow, size_t screenCursorColumn, size_t screenNoRows, size_t screenNoColumns, std::shared_ptr<utils::windows::WindowInformationInterface> const windowInformation, Direction const &direction, std::optional<size_t> lastLineUpSize, utils::ScreenBuffer const &screenBuffer, bool isCallerSecondaryHexModeWindow) noexcept
    {
        auto linesAroundInformation = dynamic_cast<utils::windows::LinesAroundPositionInformation *>(windowInformation.get())->getInformation();
        /*Each pair contains info about line len, and if line ends with new line character */
        std::pair<size_t, bool> lineUpInfo = std::get<0>(linesAroundInformation);
        std::pair<size_t, bool> lineCurrInfo = std::get<1>(linesAroundInformation);
        std::pair<size_t, bool> lineDownInfo = std::get<2>(linesAroundInformation);

        switch (direction)
        {
        case Direction::ALIGN_TO_READ_MODE:
        {
            size_t screenBufferLineSize = screenBuffer[screenCursorRow].size();
            bool isLastLineCharacterNewLine = screenBuffer[screenCursorRow].empty() ? false : utils::helpers::Lambda::isNewLineChar(screenBuffer[screenCursorRow][screenBufferLineSize - 1UL]);
            if (screenBuffer[screenCursorRow].empty() && screenCursorRow > 0)
            {
                return pair(screenCursorRow - 1UL, mapScreenBufferColumnToCursorColumn(screenCursorRow - 1UL, isLastLineCharacterNewLine ? screenBuffer[screenCursorRow -1UL].size() - 2UL : screenBuffer[screenCursorRow - 1UL].size() - 1UL, screenBuffer));
            }
            else if (1UL == screenBuffer[screenCursorRow].size() || screenBuffer[screenCursorRow].empty())
            {
                return pair(screenCursorRow, 0UL);
            }
            size_t lastLineReadModeProperColumn = mapScreenBufferColumnToCursorColumn(screenCursorRow, (isLastLineCharacterNewLine && !configuration->isHexMode()) ? screenBuffer[screenCursorRow].size() - 2UL : screenBuffer[screenCursorRow].size() - 1UL, screenBuffer);
            if (lastLineReadModeProperColumn < screenCursorColumn)
            {
                return pair(screenCursorRow, lastLineReadModeProperColumn);
            }
            break;
        }
        case Direction::UP:
        {
            if (0 == screenCursorRow)
            {
                return pair(screenCursorRow, screenCursorColumn);
            }
            size_t lastLineUpColumn = mapScreenBufferColumnToCursorColumn(screenCursorRow - 1UL, lineUpInfo.first - 1UL, screenBuffer);
            if (lastLineUpColumn <= screenCursorColumn)
            {
                if (1UL == lineUpInfo.first)
                {
                    return pair(screenCursorRow - 1UL, 0UL);
                }
                else if (utils::FEATHER_MODE::READ_MODE == currentFeatherMode)
                {
                    return pair(screenCursorRow - 1, mapScreenBufferColumnToCursorColumn(screenCursorRow - 1, lineUpInfo.first - 2UL, screenBuffer));
                }
                else if (utils::FEATHER_MODE::WRITE_MODE == currentFeatherMode || utils::FEATHER_MODE::READ_MODE_SELECT_TEXT == currentFeatherMode)
                {
                    return pair(screenCursorRow - 1, mapScreenBufferColumnToCursorColumn(screenCursorRow - 1, lineUpInfo.first - 1UL, screenBuffer));
                }
            }
            else
            {
                if (configuration->isHexMode())
                {
                    return pair{screenCursorRow - 1UL, screenCursorColumn};
                }
                return pair(screenCursorRow - 1UL, mapCursorColumnToFirstNotLessColumn(pair(screenCursorRow - 1UL, screenCursorColumn), screenBuffer));
            }
            break;
        }
        case Direction::RIGHT:
        {
            if (utils::FEATHER_MODE::READ_MODE_SELECT_TEXT == currentFeatherMode && !doLineExist(lineDownInfo) && isCursorAtLastScreenBufferColumn(pair(screenCursorRow, screenCursorColumn), lineCurrInfo, lineDownInfo, screenBuffer, isCallerSecondaryHexModeWindow))
            {
                break;
            }
            else if (utils::FEATHER_MODE::READ_MODE_SELECT_TEXT != currentFeatherMode && ((!doLineExist(lineCurrInfo) || (isCursorAtLastScreenBufferColumn(pair(screenCursorRow, screenCursorColumn), lineCurrInfo, lineDownInfo, screenBuffer, isCallerSecondaryHexModeWindow) && hasNewLine(lineCurrInfo))) ||
                (!doLineExist(lineDownInfo) && (isCursorAtLastScreenBufferColumn(pair(screenCursorRow, screenCursorColumn), lineCurrInfo, lineDownInfo, screenBuffer, isCallerSecondaryHexModeWindow) && !hasNewLine(lineCurrInfo)))))
            {
                break;
            }
            else if (doLineExist(lineDownInfo) && isCursorAtLastScreenBufferColumn(pair(screenCursorRow, screenCursorColumn), lineCurrInfo, lineDownInfo, screenBuffer, isCallerSecondaryHexModeWindow) && (!hasNewLine(lineCurrInfo) || utils::FEATHER_MODE::READ_MODE_SELECT_TEXT == currentFeatherMode))
            {
                return pair(screenCursorRow + 1UL, 0UL);
            }
            char32_t character = screenBuffer[screenCursorRow][mapCursorColumnToScreenBufferColumn(pair(screenCursorRow, screenCursorColumn), screenBuffer, isCallerSecondaryHexModeWindow)];
            if (!configuration->isHexMode() && utils::helpers::Lambda::isNewLineChar(character))
            {
                break;
            }
            screenCursorColumn += utils::helpers::Lambda::getCharacterWidth(character, true, isCallerSecondaryHexModeWindow);
            if (screenCursorColumn == screenNoColumns)
            {
                return pair(screenCursorRow + 1UL, 0UL);
            }
            break;
        }
        case Direction::DOWN:
        {
            size_t lastLineDownColumn = mapScreenBufferColumnToCursorColumn(screenCursorRow + 1UL, lineDownInfo.second ? lineDownInfo.first - 1 : lineDownInfo.first, screenBuffer);
            if (0 == lineDownInfo.first)
            {
                return pair{screenCursorRow, screenCursorColumn};
            }
            else if (lastLineDownColumn <= screenCursorColumn)
            {
                if (1UL == lineDownInfo.first && lineDownInfo.second)
                {
                    return pair(screenCursorRow + 1UL, 0UL);
                }
                else if (utils::FEATHER_MODE::READ_MODE == currentFeatherMode)
                {
                    return pair(screenCursorRow + 1, mapScreenBufferColumnToCursorColumn(screenCursorRow + 1, lineDownInfo.second ? lineDownInfo.first - 2UL : lineDownInfo.first - 1UL, screenBuffer));
                }
                else if (utils::FEATHER_MODE::WRITE_MODE == currentFeatherMode || utils::FEATHER_MODE::READ_MODE_SELECT_TEXT == currentFeatherMode)
                {
                    return pair(screenCursorRow + 1, mapScreenBufferColumnToCursorColumn(screenCursorRow + 1, lineDownInfo.second ? lineDownInfo.first - 1UL : lineDownInfo.first, screenBuffer));
                }
            }
            else
            {
                if (configuration->isHexMode())
                {
                    return pair{screenCursorRow + 1UL, screenCursorColumn};
                }
                return pair(screenCursorRow + 1, mapCursorColumnToFirstNotLessColumn(pair(screenCursorRow + 1UL, screenCursorColumn), screenBuffer));
            }
            break;
        }
        case Direction::LEFT:
        {
            if (screenCursorColumn > 0)
            {
                screenCursorColumn -= utils::helpers::Lambda::getCharacterWidth(screenBuffer[screenCursorRow][mapCursorColumnToScreenBufferColumn(pair(screenCursorRow, screenCursorColumn), screenBuffer, isCallerSecondaryHexModeWindow) - 1UL], true, isCallerSecondaryHexModeWindow);
            }
            else if (lastLineUpSize)
            {
                --screenCursorRow;
                screenCursorColumn = mapScreenBufferColumnToCursorColumn(screenCursorRow, *lastLineUpSize - 1, screenBuffer);
            }
            else if (!hasNewLine(lineUpInfo) || (configuration->isHexMode() && doLineExist(lineUpInfo)) || utils::FEATHER_MODE::READ_MODE_SELECT_TEXT == currentFeatherMode)
            {
                --screenCursorRow;
                screenCursorColumn = mapScreenBufferColumnToCursorColumn(screenCursorRow, screenBuffer[screenCursorRow].size() - 1, screenBuffer);
            }
        }
        }
        return pair(screenCursorRow, screenCursorColumn);
    }

    size_t MoveCursorOnScreenStateMachine::mapCursorColumnToScreenBufferColumn(pair cursorOnScreenPosition, utils::ScreenBuffer const &screenBuffer, bool isSecondaryHexModeWindow)
    {
        size_t localCursorOnScreenPosition = 0UL, screenColumn = 0UL;
        for (; screenColumn < screenBuffer[cursorOnScreenPosition.first].size(); ++screenColumn)
        {
            if (localCursorOnScreenPosition == cursorOnScreenPosition.second)
            {
                return screenColumn;
            }
            localCursorOnScreenPosition += utils::helpers::Lambda::getCharacterWidth(screenBuffer[cursorOnScreenPosition.first][screenColumn], true, isSecondaryHexModeWindow);
        }
        return screenColumn;
    }

    bool MoveCursorOnScreenStateMachine::doLineExist(std::pair<size_t, bool> const &lineInformation)
    {
        return lineInformation.first;
    }

    bool MoveCursorOnScreenStateMachine::hasNewLine(std::pair<size_t, bool> const &lineInformation)
    {
        return lineInformation.second;
    }

    bool MoveCursorOnScreenStateMachine::isCursorAtLastScreenBufferColumn(pair screenCursor, std::pair<size_t, bool> const &lineInformation, std::pair<size_t, bool> const &lineDownInformation, utils::ScreenBuffer const &screenBuffer, bool isCallerSecondaryHexModeWindow)
    {
        size_t screenBufferColumn = mapCursorColumnToScreenBufferColumn(screenCursor, screenBuffer, isCallerSecondaryHexModeWindow);
        if (!configuration->isHexMode() && hasNewLine(lineInformation) && lineInformation.first > 1UL)
        {
            if (utils::FEATHER_MODE::READ_MODE == currentFeatherMode)
            {
                return screenBufferColumn == screenBuffer[screenCursor.first].size() - 2UL;
            }
            else if (utils::FEATHER_MODE::WRITE_MODE == currentFeatherMode || utils::FEATHER_MODE::READ_MODE_SELECT_TEXT == currentFeatherMode)
            {
                return screenBufferColumn == screenBuffer[screenCursor.first].size() - 1UL;
            }
        }

        if (lineDownInformation.first)
        {
            return screenBufferColumn == screenBuffer[screenCursor.first].size() - 1UL;
        }
        else if (utils::FEATHER_MODE::READ_MODE == currentFeatherMode)
        {
            return screenBufferColumn == screenBuffer[screenCursor.first].size() - 1UL;
        }
        else if (utils::FEATHER_MODE::WRITE_MODE == currentFeatherMode || utils::FEATHER_MODE::READ_MODE_SELECT_TEXT == currentFeatherMode)
        {
            return screenBufferColumn == screenBuffer[screenCursor.first].size();
        }
        return false;
    }

    size_t MoveCursorOnScreenStateMachine::mapCursorColumnToFirstNotLessColumn(pair cursorOnScreenPosition, utils::ScreenBuffer const &screenBuffer)
    {
        size_t screenColumn = 0UL, screenBufferColumn = 0UL;
        //Check first non white character
        size_t firstNonWhiteCharacterColumnPos = 0, firstNonWhiteCharacterScreenBufferIdx = 0;
        for (; firstNonWhiteCharacterScreenBufferIdx < screenBuffer[cursorOnScreenPosition.first].size() && utils::helpers::Lambda::isWhiteChar(screenBuffer[cursorOnScreenPosition.first][firstNonWhiteCharacterScreenBufferIdx]); ++firstNonWhiteCharacterScreenBufferIdx)
        {
            firstNonWhiteCharacterColumnPos += utils::helpers::Lambda::getCharacterWidth(screenBuffer[cursorOnScreenPosition.first][firstNonWhiteCharacterScreenBufferIdx], true, false);
        }
        if (utils::helpers::Lambda::isNewLineChar(screenBuffer[cursorOnScreenPosition.first][firstNonWhiteCharacterScreenBufferIdx]) || utils::helpers::Lambda::isWhiteChar(screenBuffer[cursorOnScreenPosition.first][firstNonWhiteCharacterScreenBufferIdx]))
        {
            firstNonWhiteCharacterScreenBufferIdx = firstNonWhiteCharacterColumnPos = 0;
        }
        for (; screenBufferColumn < screenBuffer[cursorOnScreenPosition.first].size(); ++screenBufferColumn)
        {
            size_t nextWidthToAdd = utils::helpers::Lambda::getCharacterWidth(screenBuffer[cursorOnScreenPosition.first][screenBufferColumn], true, false);
            if (screenColumn + nextWidthToAdd > cursorOnScreenPosition.second)
            {
                return (firstNonWhiteCharacterScreenBufferIdx < screenBuffer[cursorOnScreenPosition.first].size() && screenColumn < firstNonWhiteCharacterColumnPos) ? firstNonWhiteCharacterColumnPos : screenColumn;
            }
            screenColumn += nextWidthToAdd;
        }
        return screenColumn;
    }

    size_t MoveCursorOnScreenStateMachine::mapScreenBufferColumnToCursorColumn(size_t screenBufferRow, size_t screenBufferColumn, utils::ScreenBuffer const &screenBuffer)
    {
        size_t cursorColumn = 0;
        for (size_t localScreenBufferColumn = 0UL; localScreenBufferColumn < screenBufferColumn; ++localScreenBufferColumn)
        {
            cursorColumn += utils::helpers::Lambda::getCharacterWidth(screenBuffer[screenBufferRow][localScreenBufferColumn], true, false);
        }
        return cursorColumn;
    }
} // namespace feather::utils