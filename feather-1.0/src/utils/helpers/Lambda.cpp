#include "config/Config.hpp"
#include "printer/CurrFrameVisitor.hpp"
#include "utils/Filesystem.hpp"
#include "utils/MoveCursorOnScreenStateMachine.hpp"
#include "utils/NCursesWrapper.hpp"
#include "utils/Utf8Util.hpp"
#include "utils/algorithm/SearchBuffer.hpp"
#include "utils/datatypes/Strings.hpp"
#include "utils/exception/FeatherInterruptedException.hpp"
#include "utils/exception/FeatherInvalidArgumnetException.hpp"
#include "utils/helpers/Conversion.hpp"
#include "utils/helpers/Lambda.hpp"
#include "utils/helpers/Signal.hpp"
#include "utils/storage/InMemoryStorageFactory.hpp"
#include "windows/WindowsAbstractFactory.hpp"
#include "windows/WindowsManager.hpp"

#include <filesystem>
#include <thread>

extern sig_atomic_t sigIntReceived;
extern sig_atomic_t sigBusReceived;
extern std::unique_ptr<const feather::config::Configuration> configuration;
extern feather::utils::FEATHER_MODE currentFeatherMode;
extern bool isHexMode;

namespace feather::utils::helpers
{
    std::function<bool()> Lambda::isScreenBigEnough = []()
    {
        auto screenDimensions = utils::NCursesWrapper::getScreenDimenstions();
        return screenDimensions.first >= feather::windows::WindowsAbstractFactory::MIN_ACCEPTABLE_WINDOW_HEIGHT && screenDimensions.second >= feather::windows::WindowsAbstractFactory::MIN_ACCEPTABLE_WINDOW_WIDTH;
    };

    std::function<bool(char32_t)> Lambda::characterBelongToWord = [](char32_t c)
    {
        auto characterBytesTaken = utils::Utf8Util::getCharacterBytesTakenLight(c);
        return ((1UL == characterBytesTaken && !charactersNotBelongingToWord.count(c)) || characterBytesTaken > 1);
    };

    std::function<bool(char32_t)> Lambda::characterDontBelongToWord = [](char32_t c)
    { return (1UL == utils::Utf8Util::getCharacterBytesTakenLight(c)) && charactersNotBelongingToWord.count(static_cast<char>(c)); };

    std::function<bool(char32_t)> Lambda::characterIsNotWhiteCharacter = [](char32_t c)
    { return (1UL == utils::Utf8Util::getCharacterBytesTakenLight(c)) && (U' ' != c) && (U'\t' != c) && (U'\n' != c); };
    
    std::function<bool(char32_t)> Lambda::isBackspace = [](char32_t c)
    { return (1UL == utils::Utf8Util::getCharacterBytesTakenLight(c)) && (KEY_BACKSPACE == c || 127 == c || 0x08 == c); };
    std::function<bool(char32_t)> Lambda::isBracket = [](char32_t c)
    { return (1UL == utils::Utf8Util::getCharacterBytesTakenLight(c)) && parentheses.count(c); };
  
    std::function<bool(char32_t)> Lambda::isClosingBracket = [](char32_t c)
    { return (1UL == utils::Utf8Util::getCharacterBytesTakenLight(c)) && (c == U')' || c == U'}' || c == U']' || c == U'>'); }; 

    std::function<bool(char32_t)> Lambda::isDigit = [](char32_t c)
    {
        size_t bytesTaken = utils::Utf8Util::getCharacterBytesTakenLight(c);
        return (1UL == bytesTaken) && std::isdigit(static_cast<char>(c));
    };

    std::function<bool(char32_t)> Lambda::isNewLineChar = [](char32_t c)
    { return (1UL == utils::Utf8Util::getCharacterBytesTakenLight(c)) && static_cast<char>(c) == '\n'; };

    std::function<bool(char32_t)> Lambda::isNonSpacingUtf8 = [](char32_t c)
    {
        return nonSpacingUtf8.count(c);
    };

    std::function<bool(char32_t)> Lambda::isNotNewLineChar = [](char32_t c)
    {
        size_t bytesTaken = utils::Utf8Util::getCharacterBytesTakenLight(c);
        return (bytesTaken > 1UL || !isNewLineChar(c));
    };

    std::function<bool(char32_t)> Lambda::isNotWhiteChar = [](char32_t c)
    {
        size_t bytesTaken = utils::Utf8Util::getCharacterBytesTakenLight(c);
        return (bytesTaken > 1UL || !isWhiteChar(c));
    };

    std::function<bool(char32_t)> Lambda::isOpenBracket = [](char32_t c)
    { return (1UL == utils::Utf8Util::getCharacterBytesTakenLight(c)) && (c == U'(' || c == U'{' || c == U'[' || c == U'<'); };

    std::function<bool(char32_t)> Lambda::isReadModeChar = [](char32_t c)
    {
        return (c == KEY_PPAGE || c == KEY_NPAGE || c == KEY_RESIZE || c == KEY_UP || c == KEY_DOWN || c == KEY_LEFT || c == KEY_RIGHT || isBackspace(c) || isDigit(c));
        // c == ctrl('u') || c == ctrl('d') || c == 27 || c == ctrl('k') || c == ctrl('x') || c == ctrl('w') || c == ctrl('u') || c == ctrl('r') || c == ctrl('b') || c == ctrl('p') || c == ctrl('n')) || c == U'i' ||
        // c == U'o' || c == U'O' || c == U'w' || c == U'b' || c == U'0' || c == U'$' || c == U'A' || c == U'a' || U'D' || U'g' || U'G' || U'x' || U'z' || U'y' || U'p' || U'd' || U':' || U'/' || U'\\' || U'w');
    };
    
    std::function<bool(char32_t)> Lambda::isSpace = [](char32_t c)
    { return (1UL == utils::Utf8Util::getCharacterBytesTakenLight(c)) && static_cast<char>(c) == ' '; };

    std::function<bool(char32_t)> Lambda::isTabChar = [](char32_t c)
    { return (1UL == utils::Utf8Util::getCharacterBytesTakenLight(c)) && static_cast<char>(c) == '\t'; };

    std::function<bool(char32_t)> Lambda::isWhiteChar = [](char32_t c)
    { return (1UL == utils::Utf8Util::getCharacterBytesTakenLight(c)) && (isSpace(c) || isTabChar(c)); };

    std::function<bool(char32_t)> Lambda::isWhiteCharOrNewLine = [](char32_t c)
    {
        return (1UL == utils::Utf8Util::getCharacterBytesTakenLight(c)) && (isSpace(c) || isTabChar(c) || isNewLineChar(c));
    };

    std::function<bool(char32_t, bool)> Lambda::isControlOrUnhandledChar = [](char32_t c, bool isCallerMainWindow)
    {
        int charWidth = wcwidth(Utf8Util::getCodePoint(c, isCallerMainWindow));
        return !isNewLineChar(c) && !isTabChar(c) && ((-1 == charWidth) || (0 == charWidth) || (!Utf8Util::isValidUtf8Character(c)));
    };

    std::function<bool(char32_t, bool)> Lambda::isRightToLeftUtf8 = [](char32_t c, bool isCallerMainWindow)
    {
        int codePoint = Utf8Util::getCodePoint(c, isCallerMainWindow);
        if (rightToLeftUtf8CodePoints.count(codePoint))
        {
            return true;
        }
        auto res = rightToLeftUtf8CodePointsPairs.lower_bound(std::pair(codePoint, codePoint));
        if (res != rightToLeftUtf8CodePointsPairs.end())
        {
            if (res->first == codePoint)
            {
                return true;
            }
            else if (res == rightToLeftUtf8CodePointsPairs.begin())
            {
                return false;
            }
            auto prev = std::prev(res);
            return prev->first <= codePoint && prev->second >= codePoint;
        }
        return rightToLeftUtf8CodePointsPairs.rbegin()->first <= codePoint && rightToLeftUtf8CodePointsPairs.rbegin()->second >= codePoint;
    };

    std::function<bool(pair, size_t)> Lambda::isCursorAtFirstPosition = [](pair p, size_t noDummyDeletedBytes)
    {
        return noDummyDeletedBytes == p.first && 0ULL == p.second;
    };

    std::function<bool(std::pair<pair, pair>, std::map<pair, pair> const &)> Lambda::intervalsOverlap = [](std::pair<pair, pair> interval, std::map<pair, pair> const &intervals)
    {
		for (auto &e : intervals)
		{
			if ((e.first <= interval.first && e.second >= interval.second) || (e.first <= interval.first && e.second > interval.first) || (e.first < interval.second && e.second > interval.second)) 
			{
				return true;
			}
		}
		return false;
	};

    std::function<bool(std::shared_ptr<utils::storage::AbstractStorage>)> Lambda::isValidFeatherStorage = [](std::shared_ptr<utils::storage::AbstractStorage> storage)
    {
        size_t currCursorPos = storage->getCurrentPointerPosition();
        if (storage->getLine() + '\n' != FEATHER_WATERMARK)
        {
            storage->setCursor(currCursorPos);
            return false;
        }
        storage->setCursor(currCursorPos);
        return true;
    };

    std::function<bool(utils::FEATHER_MODE)> Lambda::isSearchMode = [](utils::FEATHER_MODE const &currFeatherMode)
    {
        return currFeatherMode == utils::FEATHER_MODE::SEARCH_MODE || currFeatherMode == utils::FEATHER_MODE::HEX_SEARCH_MODE || currFeatherMode == utils::FEATHER_MODE::BROWSE_SEARCH_RESULT_DOWN_MODE || currFeatherMode == utils::FEATHER_MODE::BROWSE_SEARCH_RESULT_UP_MODE || currFeatherMode == utils::FEATHER_MODE::BROWSE_SEARCH_RESULT_MODE;
    };

std::function<pair(pair, bool, bool, utils::FEATHER_MODE, utils::datatypes::Uuid const &, std::shared_ptr<printer::PrintingOrchestratorInterface>, std::shared_ptr<utils::storage::AbstractStorage>)> Lambda::getLastValidPosInReadMode = [](pair lastValidPosition, bool isLastValidPosNewLine, bool isHexMode, utils::FEATHER_MODE currentFeatherMode, utils::datatypes::Uuid const &windowUUID, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator, std::shared_ptr<utils::storage::AbstractStorage> storage)
    {
        return isHexMode ? ((utils::FEATHER_MODE::READ_MODE_SELECT_TEXT == currentFeatherMode) ? printingOrchestrator->getNextVirtualPosition(windowUUID, storage, lastValidPosition) : lastValidPosition) : ((utils::FEATHER_MODE::READ_MODE_SELECT_TEXT == currentFeatherMode) ? (isLastValidPosNewLine ? lastValidPosition : printingOrchestrator->getNextVirtualPosition(windowUUID, storage, lastValidPosition)) : (isLastValidPosNewLine ? printingOrchestrator->getPreviousVirtualPosition(windowUUID, storage, lastValidPosition) : lastValidPosition));
    };

    std::function<pair(pair, size_t, printer::PrintingOrchestratorInterface *, utils::datatypes::Uuid const &)> Lambda::moveVirtualPositionBackward = [](pair pos, size_t charBytesTaken, printer::PrintingOrchestratorInterface *printingOrchestrator, utils::datatypes::Uuid const &windowUUID)
    {
        while (charBytesTaken--)
        {
            if (printingOrchestrator->isPreviousCharacterDeleted(pos, windowUUID))
            {
                pos = printingOrchestrator->getDeletionStart(windowUUID, pos);
            }
            if (pos.second > 0)
            {
                --pos.second;
            }
            else
            {
                --pos.first;
                pos.second = printingOrchestrator->getNumberOfChangesBytesAtPos(pos.first, windowUUID);
            }
        }
        return pos;
    };

    std::function<pair(pair, size_t, printer::PrintingOrchestratorInterface *, utils::datatypes::Uuid const &, bool)> Lambda::moveVirtualPositionForward = [](pair pos, size_t charBytesTaken, printer::PrintingOrchestratorInterface *printingOrchestrator, utils::datatypes::Uuid const &windowUUID, bool ignoreDeletions)
    {
        while (charBytesTaken--)
        {
            if (printingOrchestrator->isCharAtPosDeleted(pos, windowUUID) && !ignoreDeletions)
            {
                pos = printingOrchestrator->getDeletionEnd(windowUUID, pos);
            }
            if (pos.second < printingOrchestrator->getNumberOfChangesBytesAtPos(pos.first, windowUUID))
            {
                ++pos.second;
            }
            else
            {
                ++pos.first;
                pos.second = 0;
            }
        }
        if (printingOrchestrator->isCharAtPosDeleted(pos, windowUUID) && !ignoreDeletions)
        {
            pos = printingOrchestrator->getDeletionEnd(windowUUID, pos);
        }
        return pos;
    };

    std::function<pair(pair, size_t, std::shared_ptr<printer::PrintingOrchestratorInterface>, utils::datatypes::Uuid const &, std::shared_ptr<utils::storage::AbstractStorage>, bool)> Lambda::convertByteOffsetToPosition = [](pair currPosition, size_t byteMoveTo, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator, utils::datatypes::Uuid const &windowUUID, std::shared_ptr<utils::storage::AbstractStorage> storage, bool skipMiddleOfCharCheck)
    {
        size_t currByte = 0;
        while (currByte != byteMoveTo)
        {
            if (printingOrchestrator->isCharAtPosDeleted(currPosition, windowUUID))
            {
                currPosition = printingOrchestrator->getDeletionEnd(windowUUID, currPosition);
            }
            const size_t diffToNextChange = printingOrchestrator->getDiffToNextChange(currPosition, storage, windowUUID);
            const size_t diffToNextDeletion = printingOrchestrator->getDiffToNextDeletion(currPosition, storage, windowUUID);

            if (currByte + diffToNextChange < byteMoveTo && (diffToNextChange < diffToNextDeletion))
            {
                currPosition.first += diffToNextChange - (printingOrchestrator->getNumberOfChangesBytesAtPos(currPosition.first, windowUUID) - currPosition.second);
                currPosition.second = 0;
                currByte += diffToNextChange;
                continue;
            }

            if (currByte + diffToNextDeletion < byteMoveTo)
            {
                if (printingOrchestrator->isCharAtPosWithChanges(currPosition.first, windowUUID) && (currPosition.second + diffToNextDeletion <= printingOrchestrator->getNumberOfChangesBytesAtPos(currPosition.first, windowUUID)))
                {
                    currPosition.second += diffToNextDeletion;
                    currByte += diffToNextDeletion;
                    continue;
                }
                currPosition.first += diffToNextDeletion - (printingOrchestrator->getNumberOfChangesBytesAtPos(currPosition.first, windowUUID) - currPosition.second);
                currPosition.second = 0;
                currByte += diffToNextDeletion;
                continue;
            }

            if (!printingOrchestrator->isCharAtPosWithChanges(currPosition.first, windowUUID))
            {
                currPosition.first += std::min(diffToNextChange, std::min(byteMoveTo - currByte, diffToNextDeletion));
                currPosition.second = 0;
                currByte += std::min(diffToNextChange, std::min(byteMoveTo - currByte, diffToNextDeletion));
                continue;
            }
            if (printingOrchestrator->getNumberOfChangesBytesAtPos(currPosition.first, windowUUID) - currPosition.second >= (byteMoveTo - currByte))
            {
                currPosition.second = byteMoveTo - currByte;
                currByte += byteMoveTo - currByte;
                continue;
            }
            currByte += printingOrchestrator->getNumberOfChangesBytesAtPos(currPosition.first, windowUUID) - currPosition.second;
            currPosition.second = printingOrchestrator->getNumberOfChangesBytesAtPos(currPosition.first, windowUUID);
            auto lastCharBytes = printingOrchestrator->getCharacterBytesTaken(currPosition, storage, windowUUID);
            currByte += lastCharBytes;
            currPosition = printingOrchestrator->getNextVirtualPosition(windowUUID, storage, currPosition);
        }

        while (!skipMiddleOfCharCheck && utils::Utf8Util::isMiddleOfCharacter(printingOrchestrator->getContinousCharacters(currPosition, 1UL, storage, windowUUID).first[0]))
        {
            currPosition = printingOrchestrator->getPreviousVirtualPosition(windowUUID, storage, currPosition);
        }

        if (printingOrchestrator->isCharAtPosDeleted(currPosition, windowUUID))
        {
            currPosition = printingOrchestrator->getDeletionEnd(windowUUID, currPosition);
        }
        return currPosition;
    };

    std::function<pair(pair, size_t, std::shared_ptr<printer::PrintingOrchestratorInterface>, utils::datatypes::Uuid const &, std::shared_ptr<utils::storage::AbstractStorage>, std::basic_string<char> const &)> Lambda::convertBufferByteOffsetToPosition = [](pair currPosition, size_t offset, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator, utils::datatypes::Uuid const &windowUUID, std::shared_ptr<utils::storage::AbstractStorage> storage, std::basic_string<char> const &buffer)
    {
        size_t currOffset = 0;
        while (currOffset != offset)
        {
            if (printingOrchestrator->isCharAtPosDeleted(currPosition, windowUUID))
            {
                currPosition = printingOrchestrator->getDeletionEnd(windowUUID, currPosition);
            }
            auto characterBytesTaken = utils::Utf8Util::getCharacterBytesTaken(utils::helpers::Lambda::getSubString(buffer, currOffset, 4UL));
            if (currPosition.second < printingOrchestrator->getNumberOfChangesBytesAtPos(currPosition.first, windowUUID))
            {
                currPosition.second += characterBytesTaken;
            }
            else
            {
                currPosition.first += characterBytesTaken;
                currPosition.second = 0;
            }
            currOffset += characterBytesTaken;
        }
        if (printingOrchestrator->isCharAtPosDeleted(currPosition, windowUUID))
        {
            currPosition = printingOrchestrator->getDeletionEnd(windowUUID, currPosition);
        }
        return currPosition;
    };

    std::function<pair(pair, std::shared_ptr<feather::printer::PrintingOrchestratorInterface>, std::unique_ptr<utils::BufferFillerInterface> const &, utils::datatypes::Uuid const &, size_t, bool, bool)> Lambda::getCursorCoordinatesFromRealPosition = [](pair targetRealPosition, std::shared_ptr<feather::printer::PrintingOrchestratorInterface> printingOrchestrator, std::unique_ptr<utils::BufferFillerInterface> const &bufferFiller, utils::datatypes::Uuid const &windowUUID, size_t windowColumns, bool wasNewLineCharacterPressed, bool isSecondaryHexModeWindow)
    {
        pair currRealPosition = bufferFiller->getFramePositions().startFramePosition;
        // Cornercase: target is already less than frameStartPos. This is usefull when highliting the text.
        if (targetRealPosition < currRealPosition)
        {
            return pair(0, 0);
        }
        pair const lastValidPosition = printingOrchestrator->getLastValidPosition(windowUUID, bufferFiller->getStorage());
        size_t screenBufferRow = 0, screenBufferColumn = 0, screenColumn = 0UL;
        while (currRealPosition != targetRealPosition)
        {
            if (lastValidPosition == currRealPosition)
            {
                size_t const lastCharacterWidth = utils::helpers::Lambda::getCharacterWidth(bufferFiller->getScreenBuffer()[screenBufferRow][screenBufferColumn], true, isSecondaryHexModeWindow);
                if ((windowColumns == screenColumn + lastCharacterWidth) || (wasNewLineCharacterPressed && !isHexMode))
                {
                    return pair(screenBufferRow + 1UL, 0);
                }
                return pair(screenBufferRow, screenColumn + lastCharacterWidth); // new line pressed
            }
            else if (printingOrchestrator->isCharAtPosDeleted(currRealPosition, windowUUID))
            {
                currRealPosition = printingOrchestrator->getDeletionEnd(windowUUID, currRealPosition);
                continue;
            }
            else if (bufferFiller->getScreenBuffer()[screenBufferRow].size() - 1UL == screenBufferColumn)
            {
                if (screenBufferRow == bufferFiller->getScreenBuffer().getFilledRows() - 1UL)
                {
                    return pair(screenBufferRow, screenColumn);
                }
                auto nextPos = utils::helpers::Lambda::moveVirtualPositionForward(currRealPosition, utils::Utf8Util::getCharacterBytesTakenLight(bufferFiller->getScreenBuffer()[screenBufferRow][screenBufferColumn]), printingOrchestrator.get(), windowUUID, false);
                if (nextPos > targetRealPosition)
                {
                    return pair(screenBufferRow, screenColumn);
                }
                currRealPosition = nextPos;
                ++screenBufferRow;
                screenBufferColumn = 0UL;
                screenColumn = 0UL;
                continue;
            }
            auto nextPos = utils::helpers::Lambda::moveVirtualPositionForward(currRealPosition, utils::Utf8Util::getCharacterBytesTakenLight(bufferFiller->getScreenBuffer()[screenBufferRow][screenBufferColumn]), printingOrchestrator.get(), windowUUID, false);
            if (nextPos > targetRealPosition)
            {
                return pair(screenBufferRow, screenColumn);
            }
            currRealPosition = nextPos;
            screenColumn += utils::helpers::Lambda::getCharacterWidth(bufferFiller->getScreenBuffer()[screenBufferRow][screenBufferColumn], true, isSecondaryHexModeWindow);
            ++screenBufferColumn;
        }
        return pair(screenBufferRow, screenColumn);
    };

    std::function<size_t(size_t, std::string)> Lambda::convertToBytes = [](size_t value, std::string unit)
    {
        if ("KB" == unit)
        {
            return value >> 10;
        }
        else if ("M" == unit || "MB" == unit)
        {
            return value >> 20;
        }
        return value;
    };

    std::function<size_t (std::u32string const &, size_t, size_t, bool, bool)> Lambda::getStringWidth = [](std::u32string const &input, size_t startOffset, size_t endOffset, bool isCalledMainWindow, bool isSecondaryHexWindow)
    {
        if (startOffset >= input.size())
        {
            return 0UL;
        }
        size_t currWidth = 0;
        while (startOffset < input.length() && startOffset <= endOffset)
        {
            currWidth += getCharacterWidth(input[startOffset++], isCalledMainWindow, isSecondaryHexWindow);
        }
        return currWidth;
    };

    std::function<size_t(std::u32string const &, size_t, size_t,  bool, bool)> Lambda::getLastIndexInWindow = [](std::u32string const &input, size_t offset, size_t windowSize, bool isCalledMainWindow, bool isSecondaryHexWindow)
    {
        if (offset > input.size())
        {
            return 0UL;
        }
        size_t currWindowSize = 0;
        while (offset < input.length())
        {
            auto charWidth = getCharacterWidth(input[offset], isCalledMainWindow, isSecondaryHexWindow);
            if (currWindowSize + charWidth > windowSize)
            {
                --offset;
                break;
            }
            currWindowSize += charWidth;
            ++offset;
        }
        return offset;
    };

    std::function<size_t(std::u32string const &, size_t, size_t, size_t)> Lambda::getIndexFromOffset = [](std::u32string const &input, size_t firstIdx, size_t offset, size_t windowWidth)
    {
        size_t currOffset = 0;
        for (; firstIdx < input.length() && currOffset < offset; ++firstIdx)
        {
            currOffset += getCharacterWidth(input[firstIdx], false, false);
        }
        return firstIdx;
    };

    std::function<std::pair<pair, pair>(std::pair<pair, pair> const &, pair const &, size_t)> Lambda::shiftInterval = [](std::pair<pair, pair> const &interval, pair const &pos, size_t size)
    {
        std::pair<pair, pair> shiftedInterval = interval;
        if (interval.first.first == pos.first && interval.first.second >= pos.second)
        {
            shiftedInterval.first.second += size;
        }
        if (interval.second.first == pos.first && interval.second.second > pos.second)
        {
            shiftedInterval.second.second += size;
        }
        return shiftedInterval;
    };

    std::function<std::map<pair, pair>(std::map<pair, pair> const &, pair const &, size_t, std::optional<pair>)> Lambda::shiftIntervals = [](std::map<pair, pair> const &intervals, pair const &pos, size_t size, std::optional<pair> toSkip)
    {
        std::map<pair, pair> shiftedIntervals;
        for (auto const &e : intervals)
        {
            if (toSkip.has_value() && e.first == toSkip)
            {
                shiftedIntervals.insert(e);
                continue;
            }
            shiftedIntervals.emplace(shiftInterval(e, pos, size));
        }
        return shiftedIntervals;
    };

    std::function<std::map<pair, pair>(std::map<pair, pair> const &, pair, pair)> Lambda::disjoin = [](std::map<pair, pair> const &partitions, pair posStart, pair posEnd)
    {
        std::map<pair, pair> reversedPartitions;
        for (auto &p : partitions)
        {
            if (p.first <= posStart && p.second >= posEnd)
            {
                if (p.first == posStart && p.second == posEnd)
                {
                    continue;
                }
                else if (posStart > p.first && posEnd < p.second)
                {
                    reversedPartitions.insert({p.first, posStart});
                    reversedPartitions.insert({posEnd, p.second});
                }
                else if (posStart == p.first)
                {
                    reversedPartitions.insert({posEnd, p.second});
                }
                else
                {
                    reversedPartitions.insert({p.first, posStart});
                }
            }
            else
            {
                reversedPartitions.insert(p);
            }
        }
        return reversedPartitions;
    };

    std::function<std::map<pair, pair>(std::map<pair, pair> const &, pair, pair)> Lambda::disjoinDeletions = [](std::map<pair, pair> const &partitions, pair posStart, pair posEnd)
    {
        std::map<pair, pair> disjoinedPartitions;
        for (auto &p : partitions)
        {
            if (p.second < posStart || p.first > posEnd)
            {
                continue;
            }
            else
            {
                disjoinedPartitions[posStart] = p.first;
                posStart = p.second;
            }
        }
        if (posStart != posEnd)
        {
            disjoinedPartitions[posStart] = posEnd;
        }
        return disjoinedPartitions;
    };

    std::function<std::map<pair, pair>(std::map<pair, pair> const &, std::map<pair, pair> const &)> Lambda::mergeIntervals = [](std::map<pair, pair> const &mergeTo, std::map<pair, pair> const &mergeFrom)
    {
        std::map<pair, pair> toReturn = mergeTo;
        for (auto const &from : mergeFrom)
        {
            std::map<pair, pair> localResult;
            toReturn[from.first] = std::max(from.second, toReturn[from.first]);
            std::map<pair, pair>::const_iterator currRange = toReturn.cbegin();
            std::pair<pair, pair> currDeletion = *currRange++;
            while (currRange != toReturn.cend())
            {
                if (currRange->first <= currDeletion.second)
                {
                    currDeletion.second = std::max(currRange->second, currDeletion.second);
                }
                else
                {
                    localResult.insert(currDeletion);
                    currDeletion = *currRange;
                }
                ++currRange;
            }
            localResult.insert(currDeletion);
            toReturn.swap(localResult);
        }
        return toReturn;
    };

    std::unordered_set<char> Lambda::charactersNotBelongingToWord{0, '+', '=', ',', '@', '\t', ' ', '.', '\n', '>', '<', '/', ':', '-', '(', ')', '[', ']', '{', '}', '\\', '"', ';', '&', '#', '\''};

    std::unordered_set<unsigned int> Lambda::nonSpacingUtf8{0x80cc, 0x80cd, 0x80dd, 0x81cc, 0x81cd, 0x81d7, 0x81dd, 0x82cc, 0x82cd, 0x82d7, 0x82dd, 0x83cc, 0x83cd, 0x83d2, 0x83dd, 0x84cc, 0x84cd, 0x84d2, 0x84d7, 0x84dd, 0x85cc, 0x85cd, 0x85d2, 0x85d7, 0x85dd, 0x86cc, 0x86cd, 0x86d2, 0x86dd, 0x87cc, 0x87cd, 0x87d2, 0x87d7, 0x87dd, 0x88cc, 0x88cd, 0x88dd, 0x89cc, 0x89cd, 0x89dd, 0x8acc, 0x8acd, 0x8add, 0x8bcc, 0x8bcd, 0x8bd9, 0x8ccc, 0x8ccd, 0x8cd9, 0x8dcc, 0x8dcd, 0x8dd9, 0x8ecc, 0x8ecd, 0x8ed9, 0x8fcc, 0x8fcd, 0x8fd9, 0x90cc, 0x90cd, 0x90d8, 0x90d9, 0x91cc, 0x91cd, 0x91d6, 0x91d8, 0x91d9, 0x91dc, 0x92cc, 0x92cd, 0x92d6, 0x92d8, 0x92d9, 0x93cc, 0x93cd, 0x93d6, 0x93d8, 0x93d9, 0x94cc, 0x94cd, 0x94d6, 0x94d8, 0x94d9, 0x95cc, 0x95cd, 0x95d6, 0x95d8, 0x95d9, 0x96cc, 0x96cd, 0x96d6, 0x96d8, 0x96d9, 0x96db, 0x97cc, 0x97cd, 0x97d6, 0x97d8, 0x97d9, 0x97db, 0x98cc, 0x98cd, 0x98d6, 0x98d8, 0x98d9, 0x98db, 0x99cc, 0x99cd, 0x99d6, 0x99d8, 0x99d9, 0x99db, 0x9acc, 0x9acd, 0x9ad6, 0x9ad8, 0x9ad9, 0x9adb, 0x9bcc, 0x9bcd, 0x9bd6, 0x9bd9, 0x9bdb, 0x9ccc, 0x9ccd, 0x9cd6, 0x9cd9, 0x9cdb, 0x9dcc, 0x9dcd, 0x9dd6, 0x9dd9, 0x9ecc, 0x9ecd, 0x9ed6, 0x9ed9, 0x9fcc, 0x9fcd, 0x9fd6, 0x9fd9, 0x9fdb, 0xa0cc, 0xa0cd, 0xa0d6, 0xa0db, 0xa1cc, 0xa1cd, 0xa1d6, 0xa1db, 0xa2cc, 0xa2cd, 0xa2d6, 0xa2db, 0xa3cc, 0xa3cd, 0xa3d6, 0xa3db, 0xa4cc, 0xa4cd, 0xa4d6, 0xa4db, 0xa5cc, 0xa5cd, 0xa5d6, 0xa6cc, 0xa6cd, 0xa6d6, 0xa6de, 0xa7cc, 0xa7cd, 0xa7d6, 0xa7db, 0xa7de, 0xa8cc, 0xa8cd, 0xa8d6, 0xa8db, 0xa8de, 0xa9cc, 0xa9cd, 0xa9d6, 0xa9de, 0xaacc, 0xaacd, 0xaad6, 0xaadb, 0xaade, 0xabcc, 0xabcd, 0xabd6, 0xabdb, 0xabde, 0xabdf, 0xaccc, 0xaccd, 0xacd6, 0xacdb, 0xacde, 0xacdf, 0xadcc, 0xadcd, 0xadd6, 0xaddb, 0xadde, 0xaddf, 0xaecc, 0xaecd, 0xaed6, 0xaede, 0xaedf, 0xafcc, 0xafcd, 0xafd6, 0xafde, 0xafdf, 0xb0cc, 0xb0d6, 0xb0d9, 0xb0dc, 0xb0de, 0xb0df, 0xb1cc, 0xb1d6, 0xb1dc, 0xb1df, 0xb2cc, 0xb2d6, 0xb2dc, 0xb2df, 0xb3cc, 0xb3d6, 0xb3dc, 0xb3df, 0xb4cc, 0xb4d6, 0xb4dc, 0xb5cc, 0xb5d6, 0xb5dc, 0xb6cc, 0xb6d6, 0xb6dc, 0xb7cc, 0xb7d6, 0xb7dc, 0xb8cc, 0xb8d6, 0xb8dc, 0xb9cc, 0xb9d6, 0xb9dc, 0xbacc, 0xbad6, 0xbadc, 0xbbcc, 0xbbd6, 0xbbdc, 0xbccc, 0xbcd6, 0xbcdc, 0xbdcc, 0xbdd6, 0xbddc, 0xbddf, 0xbecc, 0xbedc, 0xbfcc, 0xbfd6, 0xbfdc, 0x80a4e0, 0x80a6ea, 0x80abe1, 0x80ace1, 0x80aee1, 0x80afe0, 0x80b0e0, 0x80b1e0, 0x80b4e0, 0x80b7e1, 0x80b8ef, 0x80bee0, 0x81a4e0, 0x81a5e0, 0x81a6e0, 0x81a6ea, 0x81a7e0, 0x81a8e0, 0x81a9e0, 0x81aae0, 0x81abe0, 0x81abea, 0x81ace0, 0x81ace1, 0x81ade0, 0x81aee1, 0x81b2e0, 0x81b4e0, 0x81b5e0, 0x81b6e0, 0x81b7e1, 0x81b8ef, 0x81bee0, 0x8282e1, 0x82a0ea, 0x82a4e0, 0x82a5e0, 0x82a6ea, 0x82a7e0, 0x82a8e0, 0x82a9e0, 0x82aae0, 0x82abe0, 0x82ace1, 0x82ade0, 0x82ade1, 0x82aee0, 0x82b5e0, 0x82b7e1, 0x82b8ef, 0x82bee0, 0x83a5e0, 0x83a7e0, 0x83a9ea, 0x83abe0, 0x83ace1, 0x83ade0, 0x83b5e0, 0x83b7e1, 0x83b8ef, 0x83bee0, 0x84a3ea, 0x84a5e0, 0x84a7e0, 0x84abe0, 0x84ade0, 0x84b0e0, 0x84b5e0, 0x84b7e1, 0x84b8ef, 0x84bee0, 0x8582e1, 0x85a2e1, 0x85a3ea, 0x85a5e0, 0x85abe0, 0x85b7e1, 0x85b8ef, 0x8682e1, 0x869fe1, 0x86a0ea, 0x86a2e1, 0x86a5e0, 0x86b1e0, 0x86b3e0, 0x86b7e1, 0x86b8ef, 0x86bee0, 0x86bfe0, 0x87a5e0, 0x87a5ea, 0x87a9e0, 0x87abe0, 0x87b1e0, 0x87b7e1, 0x87b8ef, 0x87b9e0, 0x87bee0, 0x88a5e0, 0x88a5ea, 0x88a9e0, 0x88abe0, 0x88b1e0, 0x88b7e1, 0x88b8ef, 0x88b9e0, 0x88bbe0, 0x899fe1, 0x89a5ea, 0x89b7e1, 0x89b8ef, 0x89b9e0, 0x89bbe0, 0x8a9fe1, 0x8aa5ea, 0x8ab1e0, 0x8ab7e0, 0x8ab7e1, 0x8ab8ef, 0x8ab9e0, 0x8abbe0, 0x8b9fe1, 0x8ba0e1, 0x8ba0ea, 0x8ba5ea, 0x8ba9e0, 0x8bb1e0, 0x8bb7e1, 0x8bb8ef, 0x8bb9e0, 0x8bbbe0, 0x8c9fe1, 0x8ca0e1, 0x8ca5ea, 0x8ca9e0, 0x8ca9ea, 0x8cb1e0, 0x8cb3e0, 0x8cb7e1, 0x8cb8ef, 0x8cb9e0, 0x8cbbe0, 0x8d82e1, 0x8d9fe1, 0x8da0e1, 0x8da5e0, 0x8da5ea, 0x8da7e0, 0x8da9e0, 0x8dabe0, 0x8dade0, 0x8dafe0, 0x8db1e0, 0x8db3e0, 0x8db5e0, 0x8db7e1, 0x8db8ef, 0x8db9e0, 0x8dbbe0, 0x8dbee0, 0x8e9fe1, 0x8ea5ea, 0x8eb7e1, 0x8eb8ef, 0x8eb9e0, 0x8ebee0, 0x8f9fe1, 0x8fa5ea, 0x8fb7e1, 0x8fb8ef, 0x8fbee0, 0x9083e2, 0x909fe1, 0x90a5ea, 0x90b3e1, 0x90b7e1, 0x90bee0, 0x9183e2, 0x919fe1, 0x91a5e0, 0x91a5ea, 0x91a9e0, 0x91b3e1, 0x91b7e1, 0x91bee0, 0x9283e2, 0x929ce1, 0x929de1, 0x929fe1, 0x92a5e0, 0x92b3e1, 0x92b7e0, 0x92b7e1, 0x92bee0, 0x9383e2, 0x939ce1, 0x939de1, 0x939fe1, 0x93a3e0, 0x93a5e0, 0x93b7e0, 0x93b7e1, 0x93bee0, 0x9483e2, 0x949ce1, 0x94a3e0, 0x94a5e0, 0x94b3e1, 0x94b7e0, 0x94b7e1, 0x94bee0, 0x9583e2, 0x95a3e0, 0x95a5e0, 0x95ade0, 0x95b1e0, 0x95b3e1, 0x95b7e1, 0x95bee0, 0x9683e2, 0x96a0e0, 0x96a3e0, 0x96a5e0, 0x96a9e1, 0x96ade0, 0x96b1e0, 0x96b3e1, 0x96b7e0, 0x96b7e1, 0x96bee0, 0x9783e2, 0x97a0e0, 0x97a3e0, 0x97a5e0, 0x97a8e1, 0x97b3e1, 0x97b7e1, 0x97bee0, 0x9881e1, 0x9883e2, 0x98a0e0, 0x98a3e0, 0x98a8e1, 0x98a9e1, 0x98b3e1, 0x98b7e1, 0x98bce0, 0x9981e1, 0x9982e3, 0x9983e2, 0x99a0e0, 0x99a1e0, 0x99a3e0, 0x99a9e1, 0x99b3e1, 0x99b7e1, 0x99bce0, 0x99bee0, 0x9a82e3, 0x9a83e2, 0x9aa1e0, 0x9aa3e0, 0x9aa9e1, 0x9ab3e1, 0x9ab7e1, 0x9abee0, 0x9b83e2, 0x9ba0e0, 0x9ba1e0, 0x9ba3e0, 0x9ba8e1, 0x9ba9e1, 0x9bb3e1, 0x9bb7e1, 0x9bbee0, 0x9c83e2, 0x9ca0e0, 0x9ca3e0, 0x9ca9e1, 0x9cb3e1, 0x9cb7e1, 0x9cbee0, 0x9d82e1, 0x9d8de1, 0x9d9fe1, 0x9da0e0, 0x9da3e0, 0x9da9e1, 0x9db3e1, 0x9db7e1, 0x9dbee0, 0x9e81e1, 0x9e8de1, 0x9e9aea, 0x9ea0e0, 0x9ea3e0, 0x9ea9e1, 0x9eacef, 0x9eb3e1, 0x9eb7e1, 0x9ebee0, 0x9f81e1, 0x9f8de1, 0x9f9aea, 0x9fa0e0, 0x9fa3e0, 0x9fb3e1, 0x9fb7e1, 0x9fbee0, 0xa081e1, 0xa0a0e0, 0xa0a3e0, 0xa0a3ea, 0xa0a4e1, 0xa0a9e1, 0xa0b3e1, 0xa0b7e1, 0xa0b7e2, 0xa0b8ef, 0xa0bee0, 0xa183e2, 0xa1a0e0, 0xa1a3e0, 0xa1a3ea, 0xa1a4e1, 0xa1b7e1, 0xa1b7e2, 0xa1b8ef, 0xa1bee0, 0xa2a0e0, 0xa2a3ea, 0xa2a4e1, 0xa2a5e0, 0xa2a7e0, 0xa2a9e1, 0xa2abe0, 0xa2ade0, 0xa2aee1, 0xa2b1e0, 0xa2b3e0, 0xa2b3e1, 0xa2b5e0, 0xa2b7e1, 0xa2b7e2, 0xa2b8ef, 0xa2bee0, 0xa3a0e0, 0xa3a3e0, 0xa3a3ea, 0xa3a5e0, 0xa3a7e0, 0xa3abe0, 0xa3ade0, 0xa3aee1, 0xa3b1e0, 0xa3b3e0, 0xa3b3e1, 0xa3b5e0, 0xa3b7e1, 0xa3b7e2, 0xa3b8ef, 0xa3bee0, 0xa4a3e0, 0xa4a3ea, 0xa4aee1, 0xa4b3e1, 0xa4b7e1, 0xa4b7e2, 0xa4b8ef, 0xa4bee0, 0xa583e2, 0xa5a0e0, 0xa5a0ea, 0xa5a3e0, 0xa5a3ea, 0xa5a7ea, 0xa5a9e1, 0xa5aee1, 0xa5afea, 0xa5b3e1, 0xa5b7e1, 0xa5b7e2, 0xa5b8ef, 0xa5bee0, 0xa683e2, 0xa6a0e0, 0xa6a0ea, 0xa6a3e0, 0xa6a3ea, 0xa6a4ea, 0xa6a9e1, 0xa6afe1, 0xa6b3e1, 0xa6b7e1, 0xa6b7e2, 0xa6b8ef, 0xa6bee0, 0xa783e2, 0xa7a0e0, 0xa7a3e0, 0xa7a3ea, 0xa7a4e1, 0xa7a4ea, 0xa7a9e1, 0xa7b3e1, 0xa7b7e1, 0xa7b7e2, 0xa7b8ef, 0xa7bee0, 0xa883e2, 0xa8a3e0, 0xa8a3ea, 0xa8a4e1, 0xa8a4ea, 0xa8a9e1, 0xa8aee1, 0xa8afe1, 0xa8afea, 0xa8b3e1, 0xa8b7e1, 0xa8b7e2, 0xa8b8ef, 0xa8bee0, 0xa983e2, 0xa9a0e0, 0xa9a2e1, 0xa9a3e0, 0xa9a3ea, 0xa9a4ea, 0xa9a8ea, 0xa9a9e1, 0xa9aee1, 0xa9afe1, 0xa9b7e1, 0xa9b7e2, 0xa9b8ef, 0xa9bee0, 0xaa80e3, 0xaa83e2, 0xaaa0e0, 0xaaa3e0, 0xaaa3ea, 0xaaa4ea, 0xaaa8ea, 0xaaa9e1, 0xaab7e1, 0xaab7e2, 0xaab8ef, 0xaabee0, 0xab80e3, 0xab83e2, 0xaba0e0, 0xaba3e0, 0xaba3ea, 0xaba4ea, 0xaba8ea, 0xaba9e1, 0xabade1, 0xabaee1, 0xabb7e1, 0xabb7e2, 0xabb8ef, 0xabbee0, 0xac80e3, 0xac83e2, 0xaca0e0, 0xaca0ea, 0xaca3e0, 0xaca3ea, 0xaca4ea, 0xaca8ea, 0xaca9e1, 0xacabea, 0xacade1, 0xacaee1, 0xacb0e1, 0xacb7e1, 0xacb7e2, 0xacb8ef, 0xacbee0, 0xad80e1, 0xad80e3, 0xad83e2, 0xada0e0, 0xada3e0, 0xada3ea, 0xada4ea, 0xada8ea, 0xadabea, 0xadade1, 0xadaee1, 0xadafe1, 0xadafea, 0xadb0e1, 0xadb3e1, 0xadb7e1, 0xadb7e2, 0xadb8ef, 0xadbee0, 0xae80e1, 0xae83e2, 0xaea3e0, 0xaea3ea, 0xaea8ea, 0xaeade1, 0xaeb0e1, 0xaeb7e1, 0xaeb7e2, 0xaeb8ef, 0xaebee0, 0xaf80e1, 0xaf83e2, 0xaf99ea, 0xafa3e0, 0xafa3ea, 0xafade1, 0xafafe1, 0xafb0e1, 0xafb3e2, 0xafb7e1, 0xafb7e2, 0xafb8ef, 0xafbee0, 0xb080e1, 0xb083e2, 0xb09bea, 0xb0a3e0, 0xb0a3ea, 0xb0a9e0, 0xb0aae1, 0xb0aaea, 0xb0ade1, 0xb0afe1, 0xb0b0e1, 0xb0b3e2, 0xb0b7e1, 0xb0b7e2, 0xb0bee0, 0xb181e1, 0xb19bea, 0xb1a3e0, 0xb1a3ea, 0xb1a8ea, 0xb1a9e0, 0xb1aae1, 0xb1ade1, 0xb1afe1, 0xb1b0e1, 0xb1b3e2, 0xb1b7e1, 0xb1b7e2, 0xb1b8e0, 0xb1bae0, 0xb1bde0, 0xb1bee0, 0xb280e1, 0xb281e1, 0xb29ce1, 0xb29de1, 0xb2a3e0, 0xb2a4e1, 0xb2a8ea, 0xb2aae1, 0xb2aaea, 0xb2ade1, 0xb2b0e1, 0xb2b7e1, 0xb2b7e2, 0xb2bde0, 0xb2bee0, 0xb380e1, 0xb381e1, 0xb39ce1, 0xb39de1, 0xb3a3e0, 0xb3a6ea, 0xb3a9e1, 0xb3aae1, 0xb3aaea, 0xb3ade1, 0xb3b0e1, 0xb3b7e1, 0xb3b7e2, 0xb3bde0, 0xb3bee0, 0xb480e1, 0xb481e1, 0xb499ea, 0xb49ce1, 0xb49ee1, 0xb4a3e0, 0xb4a9e1, 0xb4aae1, 0xb4aaea, 0xb4ace1, 0xb4b3e1, 0xb4b7e1, 0xb4b7e2, 0xb4b8e0, 0xb4bae0, 0xb4bde0, 0xb4bee0, 0xb580e1, 0xb599ea, 0xb59ee1, 0xb5a3e0, 0xb5a8ea, 0xb5a9e0, 0xb5a9e1, 0xb5aae1, 0xb5b7e1, 0xb5b7e2, 0xb5b8e0, 0xb5bae0, 0xb5bce0, 0xb5bde0, 0xb5bee0, 0xb680e1, 0xb699ea, 0xb6a3e0, 0xb6a6ea, 0xb6a8ea, 0xb6a9e1, 0xb6aae1, 0xb6abea, 0xb6ace1, 0xb6b0e1, 0xb6b7e1, 0xb6b7e2, 0xb6b8e0, 0xb6bae0, 0xb6bde0, 0xb6bee0, 0xb780e1, 0xb799ea, 0xb79ee1, 0xb7a3e0, 0xb7a6ea, 0xb7a9e1, 0xb7aae1, 0xb7aaea, 0xb7ace1, 0xb7b0e1, 0xb7b7e1, 0xb7b7e2, 0xb7b8e0, 0xb7bae0, 0xb7bce0, 0xb7bde0, 0xb7bee0, 0xb899ea, 0xb89ee1, 0xb8a3e0, 0xb8a6ea, 0xb8a9e1, 0xb8aae1, 0xb8aaea, 0xb8ace1, 0xb8b3e1, 0xb8b7e1, 0xb8b7e2, 0xb8b8e0, 0xb8bae0, 0xb8bde0, 0xb8bee0, 0xb980e1, 0xb999ea, 0xb99ee1, 0xb9a3e0, 0xb9a4e1, 0xb9a6ea, 0xb9a9e1, 0xb9aae1, 0xb9ace1, 0xb9b3e1, 0xb9b7e1, 0xb9b7e2, 0xb9b8e0, 0xb9bae0, 0xb9bce0, 0xb9bde0, 0xb9bee0, 0xba80e1, 0xba99ea, 0xba9ee1, 0xbaa3e0, 0xbaa4e0, 0xbaa4e1, 0xbaa9e1, 0xbaaae1, 0xbaabe0, 0xbaace1, 0xbab7e2, 0xbab8e0, 0xbabae0, 0xbabde0, 0xbabee0, 0xbb99ea, 0xbb9ee1, 0xbba3e0, 0xbba4e1, 0xbba9e1, 0xbbaae1, 0xbbabe0, 0xbbb4e0, 0xbbb7e1, 0xbbb7e2, 0xbbbae0, 0xbbbde0, 0xbbbee0, 0xbc99ea, 0xbc9ee1, 0xbca3e0, 0xbca4e0, 0xbca6e0, 0xbca6ea, 0xbca8e0, 0xbca9e1, 0xbca9ea, 0xbcaae0, 0xbcaae1, 0xbcabe0, 0xbcace0, 0xbcace1, 0xbcb2e0, 0xbcb4e0, 0xbcb7e1, 0xbcb7e2, 0xbcbae0, 0xbcbde0, 0xbcbee0, 0xbd80e1, 0xbd99ea, 0xbd9ee1, 0xbda3e0, 0xbda6ea, 0xbdaae1, 0xbdabe0, 0xbdb7e1, 0xbdb7e2, 0xbdbde0, 0xbe80e1, 0xbea3e0, 0xbea7e0, 0xbeaaea, 0xbeabe0, 0xbeb0e0, 0xbeb7e1, 0xbeb7e2, 0xbebde0, 0xbfa3e0, 0xbfa3ea, 0xbfa9e1, 0xbfaae1, 0xbfaaea, 0xbfabe0, 0xbface0, 0xbfb0e0, 0xbfb2e0, 0xbfb5e2, 0xbfb7e1, 0xbfb7e2, 0x80809ef0, 0x808191f0, 0x808291f0, 0x808491f0, 0x8084a0f3, 0x8085a0f3, 0x808691f0, 0x80869df0, 0x8086a0f3, 0x8087a0f3, 0x808c91f0, 0x808d91f0, 0x809391f0, 0x809791f0, 0x809991f0, 0x80a89df0, 0x80a99df0, 0x80b591f0, 0x818091f0, 0x81809ef0, 0x818191f0, 0x818291f0, 0x818491f0, 0x8184a0f3, 0x8185a0f3, 0x818691f0, 0x81869df0, 0x8186a0f3, 0x8187a0f3, 0x818c91f0, 0x81a890f0, 0x81a891f0, 0x81a89df0, 0x81a99df0, 0x81b591f0, 0x82809ef0, 0x828191f0, 0x828491f0, 0x8284a0f3, 0x8285a0f3, 0x82869df0, 0x8286a0f3, 0x8287a0f3, 0x82899df0, 0x829191f0, 0x829391f0, 0x82a890f0, 0x82a891f0, 0x82a89df0, 0x82a99df0, 0x82b591f0, 0x83809ef0, 0x838191f0, 0x8384a0f3, 0x8385a0f3, 0x8386a0f3, 0x8387a0f3, 0x83899df0, 0x839191f0, 0x839391f0, 0x83a591f0, 0x83a890f0, 0x83a891f0, 0x83a89df0, 0x83a99df0, 0x83b591f0, 0x84809ef0, 0x848191f0, 0x8484a0f3, 0x8485a0f3, 0x8486a0f3, 0x8487a0f3, 0x84899df0, 0x849191f0, 0x84a59ef0, 0x84a891f0, 0x84a89df0, 0x84a99df0, 0x84aa9df0, 0x84b591f0, 0x85809ef0, 0x858191f0, 0x8584a0f3, 0x8585a0f3, 0x85869df0, 0x8586a0f3, 0x8587a0f3, 0x85a59ef0, 0x85a890f0, 0x85a891f0, 0x85a89df0, 0x85a99df0, 0x85b591f0, 0x86809ef0, 0x868191f0, 0x8684a0f3, 0x8685a0f3, 0x86869df0, 0x8686a0f3, 0x8687a0f3, 0x869191f0, 0x86a59ef0, 0x86a890f0, 0x86a891f0, 0x86a89df0, 0x86a99df0, 0x86bd90f0, 0x8784a0f3,
                                                            0x8785a0f3, 0x87869df0, 0x8786a0f3, 0x8787a0f3, 0x87a59ef0, 0x87a891f0, 0x87a89df0, 0x87a991f0, 0x87a99df0, 0x87b591f0, 0x87bd90f0, 0x88809ef0, 0x8884a0f3, 0x8885a0f3, 0x88869df0, 0x8886a0f3, 0x8887a0f3, 0x88a59ef0, 0x88a891f0, 0x88a89df0, 0x88a99df0, 0x88bd90f0, 0x89809ef0, 0x8984a0f3, 0x8985a0f3, 0x89869df0, 0x8986a0f3, 0x898791f0, 0x8987a0f3, 0x89a59ef0, 0x89a891f0, 0x89a89df0, 0x89a99df0, 0x89bd90f0, 0x8a809ef0, 0x8a84a0f3, 0x8a85a0f3, 0x8a869df0, 0x8a86a0f3, 0x8a8791f0, 0x8a87a0f3, 0x8aa59ef0, 0x8aa891f0, 0x8aa89df0, 0x8aa99df0, 0x8aaa91f0, 0x8abd90f0, 0x8b809ef0, 0x8b84a0f3, 0x8b85a0f3, 0x8b869df0, 0x8b86a0f3, 0x8b8791f0, 0x8b87a0f3, 0x8ba89df0, 0x8ba99df0, 0x8baa91f0, 0x8bbd90f0, 0x8c809ef0, 0x8c84a0f3, 0x8c85a0f3, 0x8c86a0f3, 0x8c8791f0, 0x8c87a0f3, 0x8ca890f0, 0x8ca89df0, 0x8ca99df0, 0x8caa91f0, 0x8cbd90f0, 0x8d809ef0, 0x8d84a0f3, 0x8d85a0f3, 0x8d86a0f3, 0x8d87a0f3, 0x8da890f0, 0x8da89df0, 0x8da99df0, 0x8daa91f0, 0x8dbd90f0, 0x8e809ef0, 0x8e84a0f3, 0x8e85a0f3, 0x8e86a0f3, 0x8e87a0f3, 0x8ea890f0, 0x8ea89df0, 0x8ea99df0, 0x8eaa91f0, 0x8ebd90f0, 0x8f809ef0, 0x8f84a0f3, 0x8f85a0f3, 0x8f86a0f3, 0x8f8791f0, 0x8f87a0f3, 0x8fa890f0, 0x8fa89df0, 0x8fa99df0, 0x8faa91f0, 0x8fbd90f0, 0x8fbd96f0, 0x8fbe96f0, 0x90809ef0, 0x9084a0f3, 0x9085a0f3, 0x9086a0f3, 0x9087a0f3, 0x90a39ef0, 0x90a89df0, 0x90a99df0, 0x90aa91f0, 0x90b691f0, 0x90bd90f0, 0x90be96f0, 0x91809ef0, 0x9184a0f3, 0x9185a0f3, 0x9186a0f3, 0x9187a0f3, 0x91a39ef0, 0x91a89df0, 0x91a991f0, 0x91a99df0, 0x91aa91f0, 0x91b691f0, 0x91be96f0, 0x92809ef0, 0x9284a0f3, 0x9285a0f3, 0x9286a0f3, 0x9287a0f3, 0x92a39ef0, 0x92a89df0, 0x92a991f0, 0x92a99df0, 0x92aa91f0, 0x92b291f0, 0x92be96f0, 0x93809ef0, 0x9384a0f3, 0x9385a0f3, 0x9386a0f3, 0x9387a0f3, 0x93a39ef0, 0x93a89df0, 0x93a991f0, 0x93a99df0, 0x93aa91f0, 0x93b291f0, 0x94809ef0, 0x9484a0f3, 0x9485a0f3, 0x9486a0f3, 0x9487a0f3, 0x94a39ef0, 0x94a791f0, 0x94a89df0, 0x94a991f0, 0x94a99df0, 0x94aa91f0, 0x94b291f0, 0x95809ef0, 0x9584a0f3, 0x9585a0f3, 0x9586a0f3, 0x9587a0f3, 0x95a39ef0, 0x95a791f0, 0x95a89df0, 0x95a991f0, 0x95a99df0, 0x95aa91f0, 0x95b291f0, 0x95b691f0, 0x96809ef0, 0x9684a0f3, 0x9685a0f3, 0x9686a0f3, 0x9687a0f3, 0x96a39ef0, 0x96a791f0, 0x96a89df0, 0x96a991f0, 0x96a99df0, 0x96aa91f0, 0x96b291f0, 0x97809ef0, 0x9784a0f3, 0x9785a0f3, 0x9786a0f3, 0x9787a0f3, 0x97a791f0, 0x97a89df0, 0x97a99df0, 0x97b291f0, 0x97b691f0, 0x98809ef0, 0x9884a0f3, 0x9885a0f3, 0x9886a0f3, 0x9887a0f3, 0x98a89df0, 0x98a99df0, 0x98aa91f0, 0x98b291f0, 0x9984a0f3, 0x9985a0f3, 0x9986a0f3, 0x9987a0f3, 0x99a89df0, 0x99a991f0, 0x99a99df0, 0x99aa91f0, 0x99b291f0, 0x9a84a0f3, 0x9a85a0f3, 0x9a86a0f3, 0x9a87a0f3, 0x9aa791f0, 0x9aa89df0, 0x9aa991f0, 0x9aa99df0, 0x9ab291f0, 0x9b809ef0, 0x9b84a0f3, 0x9b85a0f3, 0x9b86a0f3, 0x9b87a0f3, 0x9ba791f0, 0x9ba89df0, 0x9ba991f0, 0x9ba99df0, 0x9baa9df0, 0x9bb291f0, 0x9c809ef0, 0x9c84a0f3, 0x9c85a0f3, 0x9c86a0f3, 0x9c87a0f3, 0x9c9791f0, 0x9ca89df0, 0x9ca99df0, 0x9caa9df0, 0x9cb291f0, 0x9d809ef0, 0x9d84a0f3, 0x9d85a0f3, 0x9d86a0f3, 0x9d87a0f3, 0x9d9791f0, 0x9d9c91f0, 0x9da89df0, 0x9da99df0, 0x9daa9df0, 0x9db291f0, 0x9db29bf0, 0x9e809ef0, 0x9e84a0f3, 0x9e85a0f3, 0x9e86a0f3, 0x9e87a0f3, 0x9e9191f0, 0x9e9c91f0, 0x9ea89df0, 0x9ea99df0, 0x9eaa9df0, 0x9eb291f0, 0x9eb29bf0, 0x9f809ef0, 0x9f84a0f3, 0x9f85a0f3, 0x9f86a0f3, 0x9f87a0f3, 0x9f8b91f0, 0x9f9c91f0, 0x9fa89df0, 0x9fa99df0, 0x9faa9df0, 0x9fb291f0, 0xa0809ef0, 0xa084a0f3, 0xa085a0f3, 0xa086a0f3, 0xa087a0f3, 0xa08b90f0, 0xa0a791f0, 0xa0a89df0, 0xa0a99df0, 0xa0b291f0, 0xa1809ef0, 0xa184a0f3, 0xa185a0f3, 0xa186a0f3, 0xa187a0f3, 0xa1a89df0, 0xa1a99df0, 0xa1aa9df0, 0xa1b291f0, 0xa284a0f3, 0xa285a0f3, 0xa286a0f3, 0xa287a0f3, 0xa29c91f0, 0xa2a89df0, 0xa2a99df0, 0xa2aa9df0, 0xa2b291f0, 0xa3809ef0, 0xa384a0f3, 0xa385a0f3, 0xa386a0f3, 0xa387a0f3, 0xa38b91f0, 0xa39c91f0, 0xa3a89df0, 0xa3a99df0, 0xa3aa9df0, 0xa3b291f0, 0xa4809ef0, 0xa484a0f3, 0xa485a0f3, 0xa486a0f3, 0xa487a0f3, 0xa48b91f0, 0xa49c91f0, 0xa4a89df0, 0xa4a99df0, 0xa4aa9df0, 0xa4b291f0, 0xa4b490f0, 0xa4bf96f0, 0xa584a0f3, 0xa585a0f3, 0xa586a0f3, 0xa587a0f3, 0xa58b91f0, 0xa59c91f0, 0xa5a89df0, 0xa5a99df0, 0xa5aa9df0, 0xa5ab90f0, 0xa5b291f0, 0xa5b490f0, 0xa6809ef0, 0xa684a0f3, 0xa685a0f3, 0xa686a0f3, 0xa687a0f3, 0xa68b91f0, 0xa68d91f0, 0xa6a89df0, 0xa6a99df0, 0xa6aa9df0, 0xa6ab90f0, 0xa6b291f0, 0xa6b490f0, 0xa7809ef0, 0xa78491f0, 0xa784a0f3, 0xa7859df0, 0xa785a0f3, 0xa786a0f3, 0xa787a0f3, 0xa78b91f0, 0xa78d91f0, 0xa79c91f0, 0xa7a89df0, 0xa7a99df0, 0xa7aa9df0, 0xa7b291f0, 0xa7b490f0, 0xa8809ef0, 0xa88491f0, 0xa884a0f3, 0xa8859df0, 0xa885a0f3, 0xa886a0f3, 0xa887a0f3, 0xa88b91f0, 0xa88d91f0, 0xa89c91f0, 0xa8a89df0, 0xa8a99df0, 0xa8aa9df0, 0xa9809ef0, 0xa98491f0, 0xa984a0f3, 0xa9859df0, 0xa985a0f3, 0xa986a0f3, 0xa987a0f3, 0xa98b91f0, 0xa98d91f0, 0xa99c91f0, 0xa9a89df0, 0xa9a99df0, 0xa9aa9df0, 0xaa809ef0, 0xaa8491f0, 0xaa84a0f3, 0xaa85a0f3, 0xaa869df0, 0xaa86a0f3, 0xaa87a0f3, 0xaa8b91f0, 0xaa8d91f0, 0xaa9c91f0, 0xaaa89df0, 0xaaa99df0, 0xaaaa9df0, 0xaab291f0, 0xab8491f0, 0xab84a0f3, 0xab85a0f3, 0xab869df0, 0xab86a0f3, 0xab87a0f3, 0xab8d91f0, 0xab9a91f0, 0xab9c91f0, 0xaba89df0, 0xaba99df0, 0xabaa9df0, 0xabb291f0, 0xabba90f0, 0xac84a0f3, 0xac85a0f3, 0xac869df0, 0xac86a0f3, 0xac87a0f3, 0xac8b9ef0, 0xac8d91f0, 0xaca89df0, 0xaca99df0, 0xacaa9df0, 0xacb291f0, 0xacba90f0, 0xad8491f0, 0xad84a0f3, 0xad85a0f3, 0xad869df0, 0xad86a0f3, 0xad87a0f3, 0xad8b9ef0, 0xad9a91f0, 0xada89df0, 0xadaa9df0, 0xadb291f0, 0xae8491f0, 0xae84a0f3, 0xae85a0f3, 0xae86a0f3, 0xae87a0f3, 0xae8b9ef0, 0xaea89df0, 0xaeaa9df0, 0xaeb291f0, 0xaf8491f0, 0xaf84a0f3, 0xaf85a0f3, 0xaf86a0f3, 0xaf87a0f3, 0xaf8891f0, 0xaf8b9ef0, 0xafa091f0, 0xafa89df0, 0xafaa9df0, 0xafb291f0, 0xb08491f0, 0xb0849ef0, 0xb084a0f3, 0xb085a0f3, 0xb086a0f3, 0xb08891f0, 0xb08d91f0, 0xb09a91f0, 0xb0a091f0, 0xb0a89df0, 0xb0ab96f0, 0xb0ac96f0, 0xb0b091f0, 0xb0b291f0, 0xb18491f0, 0xb1849ef0, 0xb184a0f3, 0xb185a0f3, 0xb186a0f3, 0xb18891f0, 0xb18d91f0, 0xb19a91f0, 0xb1a091f0, 0xb1a89df0, 0xb1ab96f0, 0xb1ac96f0, 0xb1b091f0, 0xb1b491f0, 0xb28491f0, 0xb2849ef0, 0xb284a0f3, 0xb285a0f3, 0xb286a0f3, 0xb28d91f0, 0xb29691f0, 0xb29a91f0, 0xb2a091f0, 0xb2a89df0, 0xb2ab96f0, 0xb2ac96f0, 0xb2b091f0, 0xb2b291f0, 0xb2b491f0, 0xb38291f0, 0xb38491f0, 0xb3849ef0, 0xb384a0f3, 0xb38591f0, 0xb385a0f3, 0xb386a0f3, 0xb38d91f0, 0xb39291f0, 0xb39691f0, 0xb39891f0, 0xb39a91f0, 0xb3a091f0, 0xb3a891f0, 0xb3a89df0, 0xb3ab96f0, 0xb3ac96f0, 0xb3b091f0, 0xb3b291f0, 0xb3b491f0, 0xb3bb91f0, 0xb48291f0, 0xb48491f0, 0xb4849ef0, 0xb484a0f3, 0xb485a0f3, 0xb486a0f3, 0xb48891f0, 0xb48d91f0, 0xb49291f0, 0xb49691f0, 0xb49891f0, 0xb49a91f0, 0xb4a091f0, 0xb4a891f0, 0xb4a89df0, 0xb4ab96f0, 0xb4ac96f0, 0xb4b091f0, 0xb4b491f0, 0xb4bb91f0, 0xb58291f0, 0xb5849ef0, 0xb584a0f3, 0xb585a0f3, 0xb586a0f3, 0xb59291f0, 0xb59691f0, 0xb59891f0, 0xb59a91f0, 0xb5a091f0, 0xb5a891f0, 0xb5a89df0, 0xb5a99df0, 0xb5ac96f0, 0xb5b091f0, 0xb5b291f0, 0xb5b491f0, 0xb68291f0, 0xb6849ef0, 0xb684a0f3, 0xb685a0f3, 0xb68691f0, 0xb686a0f3, 0xb68891f0, 0xb68d90f0, 0xb69291f0, 0xb69891f0, 0xb6a091f0, 0xb6a891f0, 0xb6a89df0, 0xb6ac96f0, 0xb6b091f0, 0xb6b291f0, 0xb6b491f0, 0xb784a0f3, 0xb785a0f3, 0xb78691f0, 0xb786a0f3, 0xb78891f0, 0xb78d90f0, 0xb79291f0, 0xb79891f0, 0xb79a91f0, 0xb7a091f0, 0xb7a891f0, 0xb88091f0, 0xb884a0f3, 0xb885a0f3, 0xb88691f0, 0xb886a0f3, 0xb88d90f0, 0xb89091f0, 0xb89291f0, 0xb89891f0, 0xb8a890f0, 0xb8a891f0, 0xb8b091f0, 0xb98091f0, 0xb98291f0, 0xb984a0f3, 0xb985a0f3, 0xb98691f0, 0xb986a0f3, 0xb98d90f0, 0xb99091f0, 0xb99891f0, 0xb9a091f0, 0xb9a890f0, 0xb9b091f0, 0xba8091f0, 0xba8291f0, 0xba84a0f3, 0xba85a0f3, 0xba8691f0, 0xba86a0f3, 0xba8d90f0, 0xba9091f0, 0xba9291f0, 0xba9891f0, 0xbaa091f0, 0xbaa890f0, 0xbab091f0, 0xbab491f0, 0xbb8091f0, 0xbb84a0f3, 0xbb859df0, 0xbb85a0f3, 0xbb8691f0, 0xbb86a0f3, 0xbb8c91f0, 0xbb9091f0, 0xbba491f0, 0xbba891f0, 0xbba89df0, 0xbbb091f0, 0xbc8091f0, 0xbc84a0f3, 0xbc859df0, 0xbc85a0f3, 0xbc8691f0, 0xbc86a0f3, 0xbc8c91f0, 0xbc9091f0, 0xbc9691f0, 0xbca491f0, 0xbca891f0, 0xbca89df0, 0xbcb091f0, 0xbcb491f0, 0xbd8091f0, 0xbd84a0f3, 0xbd859df0, 0xbd85a0f3, 0xbd8691f0, 0xbd86a0f3, 0xbd8790f0, 0xbd9091f0, 0xbd9691f0, 0xbd9891f0, 0xbda891f0, 0xbda89df0, 0xbdb091f0, 0xbdb491f0, 0xbe8091f0, 0xbe84a0f3, 0xbe859df0, 0xbe85a0f3, 0xbe8691f0, 0xbe86a0f3, 0xbe8891f0, 0xbe9091f0, 0xbea491f0, 0xbea891f0, 0xbea89df0, 0xbf8091f0, 0xbf8191f0, 0xbf84a0f3, 0xbf859df0, 0xbf85a0f3, 0xbf86a0f3, 0xbf9091f0, 0xbf9291f0, 0xbf9691f0, 0xbf9891f0, 0xbfa890f0, 0xbfa89df0, 0xbfb091f0, 0xbfb491f0};

    std::map<char32_t, char32_t> Lambda::openToCloseBracket{{U'(', U')'}, {U'{', U'}'}, {U'[', U']'}, {U'<', U'>'}};

    std::map<char32_t, char32_t> Lambda::closedToOpenBracket{{U')', U'('}, {U'}', U'{'}, {U']', U'['}, {U'>', U'<'}};

    std::unordered_set<char32_t> Lambda::parentheses{U'(', U'{', U'[', U'<', U'>', U']', U'}', U')'};

    std::unordered_set<int> Lambda::rightToLeftUtf8CodePoints{0x0608, 0x060B, 0x060D, 0x061B, 0x061C, 0x061D, 0x0640, 0x066D, 0x06D4, 0x06D5, 0x06FF, 0x070E, 0x070F, 0x0710, 0x07B1, 0x08B5, 0xFDFC, 0xFE75, 0x1EC70, 0x1ECAC, 0x1ECB0, 0x1ED00, 0x1EE20, 0x1EE23, 0x1EE24, 0x1EE27, 0x1EE28, 0x1EE33, 0x1EE38, 0x1EE39, 0x1EE3A, 0x1EE3B, 0x1EE47, 0x1EE48, 0x1EE49, 0x1EE4A, 0x1EE4B, 0x1EE4C, 0x1EE50, 0x1EE53, 0x1EE54, 0x1EE57, 0x1EE58, 0x1EE59, 0x1EE5A, 0x1EE5B, 0x1EE5C, 0x1EE5D, 0x1EE5E, 0x1EE5F, 0x1EE60, 0x1EE63, 0x1EE64, 0x1EE6B, 0x1EE73, 0x1EE78, 0x1EE7D, 0x1EE7E, 0x1EE7F, 0x1EE8A, 0x1EEA4, 0x1EEAA, 0x1EE42, 0x06DD, 0x08E2, 0x060C, 0x06DE, 0x06E9, 0xFDFD, 0x0670, 0x081A, 0x07FA, 0x0590, 0x05BE, 0x05C0, 0x05C3, 0x05C6, 0x0824, 0x0828, 0x083F, 0x085E, 0x085F, 0x200F, 0xFB1D, 0xFB3D, 0xFB3E, 0xFB3F, 0xFB37, 0xFB42, 0xFB45, 0x10808, 0x10809, 0x10836, 0x1083C, 0x10856, 0x10857, 0x108F3, 0x1093F, 0x10A00, 0x10A04, 0x10A14, 0x10A18, 0x10A7F, 0x10AC8, 0x10E7F, 0x10EAA, 0x10EAD, 0x10F27, 0x1E94B, 0x202E, 0x202B, 0x2067};

    std::set<std::pair<int, int>> Lambda::rightToLeftUtf8CodePointsPairs{std::make_pair(0x05C8, 0x05CF), std::make_pair(0x05D0, 0x05EA), std::make_pair(0x05EB, 0x05EE), std::make_pair(0x05EF, 0x05F2), std::make_pair(0x05F3, 0x05F4), std::make_pair(0x05F5, 0x05FF), std::make_pair(0x07C0, 0x07C9), std::make_pair(0x07CA, 0x07EA), std::make_pair(0x07F4, 0x07F5), std::make_pair(0x07FB, 0x07FC), std::make_pair(0x07FE, 0x07FF), std::make_pair(0x0800, 0x0815), std::make_pair(0x082E, 0x082F), std::make_pair(0x0830, 0x083E), std::make_pair(0x0840, 0x0858), std::make_pair(0x085C, 0x085D), std::make_pair(0x0870, 0x089F), std::make_pair(0xFB1F, 0xFB28), std::make_pair(0xFB2A, 0xFB36), std::make_pair(0xFB38, 0xFB3C), std::make_pair(0xFB40, 0xFB41), std::make_pair(0xFB43, 0xFB44), std::make_pair(0xFB46, 0xFB4F), std::make_pair(0x10800, 0x10805), std::make_pair(0x10806, 0x10807), std::make_pair(0x1080A, 0x10835), std::make_pair(0x10837, 0x10838), std::make_pair(0x10839, 0x1083B), std::make_pair(0x1083D, 0x1083E), std::make_pair(0x1083F, 0x10855), std::make_pair(0x10858, 0x1085F), std::make_pair(0x10860, 0x10876), std::make_pair(0x10877, 0x10878), std::make_pair(0x10879, 0x1087F), std::make_pair(0x10880, 0x1089E), std::make_pair(0x1089F, 0x108A6), std::make_pair(0x108A7, 0x108AF), std::make_pair(0x108B0, 0x108DF), std::make_pair(0x108E0, 0x108F2), std::make_pair(0x108F4, 0x108F5), std::make_pair(0x108F6, 0x108FA), std::make_pair(0x108FB, 0x108FF), std::make_pair(0x10900, 0x10915), std::make_pair(0x10916, 0x1091B), std::make_pair(0x1091C, 0x1091E), std::make_pair(0x10920, 0x10939), std::make_pair(0x1093A, 0x1093E), std::make_pair(0x10940, 0x1097F), std::make_pair(0x10980, 0x109B7), std::make_pair(0x109B8, 0x109BB), std::make_pair(0x109BC, 0x109BD), std::make_pair(0x109BE, 0x109BF), std::make_pair(0x109C0, 0x109CF), std::make_pair(0x109D0, 0x109D1), std::make_pair(0x109D2, 0x109FF), std::make_pair(0x10A07, 0x10A0B), std::make_pair(0x10A10, 0x10A13), std::make_pair(0x10A15, 0x10A17), std::make_pair(0x10A19, 0x10A35), std::make_pair(0x10A36, 0x10A37), std::make_pair(0x10A3B, 0x10A3E), std::make_pair(0x10A40, 0x10A48), std::make_pair(0x10A49, 0x10A4F), std::make_pair(0x10A50, 0x10A58), std::make_pair(0x10A59, 0x10A5F), std::make_pair(0x10A60, 0x10A7C), std::make_pair(0x10A7D, 0x10A7E), std::make_pair(0x10A80, 0x10A9C), std::make_pair(0x10A9D, 0x10A9F), std::make_pair(0x10AA0, 0x10ABF), std::make_pair(0x10AC0, 0x10AC7), std::make_pair(0x10AC9, 0x10AE4), std::make_pair(0x10AE7, 0x10AEA), std::make_pair(0x10AEB, 0x10AEF), std::make_pair(0x10AF0, 0x10AF6), std::make_pair(0x10AF7, 0x10AFF), std::make_pair(0x10B00, 0x10B35), std::make_pair(0x10B36, 0x10B38), std::make_pair(0x10B40, 0x10B55), std::make_pair(0x10B56, 0x10B57), std::make_pair(0x10B58, 0x10B5F), std::make_pair(0x10B60, 0x10B72), std::make_pair(0x10B73, 0x10B77), std::make_pair(0x10B78, 0x10B7F), std::make_pair(0x10B80, 0x10B91), std::make_pair(0x10B92, 0x10B98), std::make_pair(0x10B99, 0x10B9C), std::make_pair(0x10B9D, 0x10BA8), std::make_pair(0x10BA9, 0x10BAF), std::make_pair(0x10BB0, 0x10BFF), std::make_pair(0x10C00, 0x10C48), std::make_pair(0x10C49, 0x10C7F), std::make_pair(0x10C80, 0x10CB2), std::make_pair(0x10CB3, 0x10CBF), std::make_pair(0x10CC0, 0x10CF2), std::make_pair(0x10CF3, 0x10CF9), std::make_pair(0x10CFA, 0x10CFF), std::make_pair(0x10D40, 0x10E5F), std::make_pair(0x10E80, 0x10EA9), std::make_pair(0x10EAE, 0x10EAF), std::make_pair(0x10EB0, 0x10EB1), std::make_pair(0x10EB2, 0x10EFF), std::make_pair(0x10F00, 0x10F1C), std::make_pair(0x10F1D, 0x10F26), std::make_pair(0x10F28, 0x10F2F), std::make_pair(0x10F70, 0x10FAF), std::make_pair(0x10FB0, 0x10FC4), std::make_pair(0x10FC5, 0x10FCB), std::make_pair(0x10FCC, 0x10FDF), std::make_pair(0x10FE0, 0x10FF6), std::make_pair(0x10FF7, 0x10FFF), std::make_pair(0x1E800, 0x1E8C4), std::make_pair(0x1E8C5, 0x1E8C6), std::make_pair(0x1E8C7, 0x1E8CF), std::make_pair(0x1E8D7, 0x1E8FF), std::make_pair(0x1E900, 0x1E943), std::make_pair(0x1E94C, 0x1E94F), std::make_pair(0x1E950, 0x1E959), std::make_pair(0x1E95A, 0x1E95D), std::make_pair(0x1E95E, 0x1E95F), std::make_pair(0x1E960, 0x1EC6F), std::make_pair(0x1ECC0, 0x1ECFF), std::make_pair(0x1ED50, 0x1EDFF), std::make_pair(0x1EF00, 0x1EFFF), std::make_pair(0x0600, 0x0605), std::make_pair(0x0660, 0x0669), std::make_pair(0x066B, 0x066C), std::make_pair(0x10D30, 0x10D39), std::make_pair(0x10E60, 0x10E7E), std::make_pair(0x058D, 0x058E), std::make_pair(0x0606, 0x0607), std::make_pair(0x060E, 0x060F), std::make_pair(0x1EEF0, 0x1EEF1), std::make_pair(0x0610, 0x061A), std::make_pair(0x064B, 0x065F), std::make_pair(0x06D6, 0x06DC), std::make_pair(0x06DF, 0x06E4), std::make_pair(0x06E7, 0x06E8), std::make_pair(0x06EA, 0x06ED), std::make_pair(0x08D3, 0x08E1), std::make_pair(0x08E3, 0x0902), std::make_pair(0x061E, 0x061F), std::make_pair(0x0620, 0x063F), std::make_pair(0x0641, 0x064A), std::make_pair(0x066E, 0x066F), std::make_pair(0x0671, 0x06D3), std::make_pair(0x06E5, 0x06E6), std::make_pair(0x06EE, 0x06EF), std::make_pair(0x06FA, 0x06FC), std::make_pair(0x06FD, 0x06FE), std::make_pair(0x0700, 0x070D), std::make_pair(0x0712, 0x072F), std::make_pair(0x074B, 0x074C), std::make_pair(0x074D, 0x07A5), std::make_pair(0x07B2, 0x07BF), std::make_pair(0x0860, 0x086A), std::make_pair(0x086B, 0x086F), std::make_pair(0x08A0, 0x08B4), std::make_pair(0x08B6, 0x08C7), std::make_pair(0x08C8, 0x08D2), std::make_pair(0xFB50, 0xFBB1), std::make_pair(0xFBB2, 0xFBC1), std::make_pair(0xFBC2, 0xFBD2), std::make_pair(0xFBD3, 0xFD3D), std::make_pair(0xFD40, 0xFD4F), std::make_pair(0xFD50, 0xFD8F), std::make_pair(0xFD90, 0xFD91), std::make_pair(0xFD92, 0xFDC7), std::make_pair(0xFDC8, 0xFDCF), std::make_pair(0xFDF0, 0xFDFB), std::make_pair(0xFDFE, 0xFDFF), std::make_pair(0xFE70, 0xFE74), std::make_pair(0xFE76, 0xFEFC), std::make_pair(0xFEFD, 0xFEFE), std::make_pair(0x10D00, 0x10D23), std::make_pair(0x10D28, 0x10D2F), std::make_pair(0x10D3A, 0x10D3F), std::make_pair(0x10F30, 0x10F45), std::make_pair(0x10F51, 0x10F54), std::make_pair(0x10F55, 0x10F59), std::make_pair(0x10F5A, 0x10F6F), std::make_pair(0x1EC71, 0x1ECAB), std::make_pair(0x1ECAD, 0x1ECAF), std::make_pair(0x1ECB1, 0x1ECB4), std::make_pair(0x1ECB5, 0x1ECBF), std::make_pair(0x1ED01, 0x1ED2D), std::make_pair(0x1ED2F, 0x1ED3D), std::make_pair(0x1ED3E, 0x1ED4F), std::make_pair(0x1EE00, 0x1EE03), std::make_pair(0x1EE05, 0x1EE1F), std::make_pair(0x1EE21, 0x1EE22), std::make_pair(0x1EE25, 0x1EE26), std::make_pair(0x1EE29, 0x1EE32), std::make_pair(0x1EE34, 0x1EE37), std::make_pair(0x1EE3C, 0x1EE41), std::make_pair(0x1EE43, 0x1EE46), std::make_pair(0x1EE4D, 0x1EE4F), std::make_pair(0x1EE51, 0x1EE52), std::make_pair(0x1EE55, 0x1EE56), std::make_pair(0x1EE61, 0x1EE62), std::make_pair(0x1EE65, 0x1EE66), std::make_pair(0x1EE67, 0x1EE6A), std::make_pair(0x1EE6C, 0x1EE72), std::make_pair(0x1EE74, 0x1EE77), std::make_pair(0x1EE79, 0x1EE7C), std::make_pair(0x1EE80, 0x1EE89), std::make_pair(0x1EE8B, 0x1EE9B), std::make_pair(0x1EE9C, 0x1EEA0), std::make_pair(0x1EEA1, 0x1EEA3), std::make_pair(0x1EEA5, 0x1EEA9), std::make_pair(0x1EEAB, 0x1EEBB), std::make_pair(0x1EEBC, 0x1EEEF), std::make_pair(0x1EEF2, 0x1EEFF)};

    std::function<pair(pair, std::shared_ptr<utils::storage::AbstractStorage>, std::shared_ptr<printer::PrintingOrchestratorInterface>, utils::datatypes::Uuid const &)> Lambda::alignToProperPosition = [](pair pos, std::shared_ptr<utils::storage::AbstractStorage> const storage, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator, utils::datatypes::Uuid const &windowUUID)
    {
        auto probeBackward = printingOrchestrator->getContinousCharactersBackward(pos, MAX_UTF8_CHAR_BYTES, storage, windowUUID);
        auto probeForward = printingOrchestrator->getContinousCharacters(pos, 4UL, storage, windowUUID);
        auto mergedIntervals = probeBackward.first;
        std::copy(probeForward.first.cbegin(), probeForward.first.cend(), std::back_inserter(mergedIntervals));
        size_t posIdx = probeBackward.first.size();
        for (size_t delta = 0; delta < probeBackward.first.size(); ++delta)
        {
            if (delta + utils::Utf8Util::getCharacterBytesTaken(std::vector<char>(std::next(mergedIntervals.begin(), delta), mergedIntervals.end())) > posIdx)
            {
                pair probeBackwardStartPos = probeBackward.second;
                while (delta--)
                {
                    probeBackwardStartPos = printingOrchestrator->getNextVirtualPosition(windowUUID, storage, probeBackwardStartPos);
                }
                return probeBackwardStartPos;
            }
        }
        pair lastValidPos = printingOrchestrator->getLastValidPosition(windowUUID, storage);
        if (pos > lastValidPos)
        {
            bool isLastValidPosNewLine = isCharAtPos(lastValidPos, isNewLineChar, windowUUID, storage, printingOrchestrator).first;
            if (!isHexMode)
            {
                if (utils::FEATHER_MODE::WRITE_MODE == currentFeatherMode)
                {
                    pos = isLastValidPosNewLine ? lastValidPos : printingOrchestrator->getNextVirtualPosition(windowUUID, storage, lastValidPos);
                }
                else
                {
                    pos = isLastValidPosNewLine ? printingOrchestrator->getPreviousVirtualPosition(windowUUID, storage, lastValidPos) : lastValidPos;
                }
            }
        }
        return pos;
    };

    std::function<unsigned long(char32_t, bool, bool)> Lambda::getCharacterWidth = [](char32_t c, bool callerIsMainWindow, bool isCallerSecondaryHexWindow)
    {
        if (callerIsMainWindow && isHexMode)
        {
            return isCallerSecondaryHexWindow ? 1UL : 3UL;
        }
        else if (isTabChar(c))
        {
            return configuration->getTabWidth();
        }
        else if (isNewLineChar(c))
        {
            return 1UL;
        }

        wchar_t cp = Utf8Util::getCodePoint(c, callerIsMainWindow);
        int width = wcwidth(cp);
        if (-1 == width || 0 == width || !Utf8Util::isValidUtf8Character(c) || isRightToLeftUtf8(c, callerIsMainWindow) || isNonSpacingUtf8(c))
        {
            return utils::Utf8Util::getCharacterBytesTakenLight(c) * 2UL;
        }
        // Private area code points are arbitrary 2 in width
        // else if ((cp >= 0xE000 && cp <= 0xF8FF) || (cp >= 0x10600 && cp <= 0x1077F) || (cp >= 0x100000 && cp <= 0x10FFFF) || (cp >= 0x1000 && cp <= 0x1FFF))
        {
            //    return 2UL;
        }
        return static_cast<unsigned long>(width);
    };

    std::function<std::pair<pair, pair>(std::pair<pair, pair>, std::shared_ptr<feather::printer::PrintingOrchestratorInterface>, std::unique_ptr<utils::BufferFillerInterface> const &, utils::datatypes::Uuid const &, size_t, bool)> Lambda::getScreenBufferPositions = [](std::pair<pair, pair> cursorRealPositions, std::shared_ptr<feather::printer::PrintingOrchestratorInterface> printingOrchestrator, std::unique_ptr<utils::BufferFillerInterface> const &bufferFiller, utils::datatypes::Uuid const &windowUUID, size_t windowColumns, bool isSecondaryHexModeWindow)
    {
        if (printingOrchestrator->isCharAtPosDeleted(cursorRealPositions.first, windowUUID))
        {
            cursorRealPositions.first = printingOrchestrator->getDeletionEnd(windowUUID, cursorRealPositions.first);
        }
        if (printingOrchestrator->isCharAtPosDeleted(cursorRealPositions.second, windowUUID))
        {
            cursorRealPositions.second = printingOrchestrator->getDeletionStart(windowUUID, cursorRealPositions.second);
        }
        pair const cursorCoordinatesStart = getCursorCoordinatesFromRealPosition(cursorRealPositions.first, printingOrchestrator, bufferFiller, windowUUID, windowColumns, false, isSecondaryHexModeWindow);
        pair const cursorCoordinatesEnd = getCursorCoordinatesFromRealPosition(cursorRealPositions.second, printingOrchestrator, bufferFiller, windowUUID, windowColumns, false, isSecondaryHexModeWindow);
        auto bufferPositions = std::make_pair(pair(cursorCoordinatesStart.first, utils::MoveCursorOnScreenStateMachine::mapCursorColumnToScreenBufferColumn(cursorCoordinatesStart, bufferFiller->getScreenBuffer(), isSecondaryHexModeWindow)), pair(cursorCoordinatesEnd.first, utils::MoveCursorOnScreenStateMachine::mapCursorColumnToScreenBufferColumn(cursorCoordinatesEnd, bufferFiller->getScreenBuffer(), isSecondaryHexModeWindow)));
        auto decoratedStartPosition = std::make_pair(bufferPositions.first.first, bufferFiller->getScreenBuffer().mapScreenBufferColumnToDecoratedColumnStart(bufferPositions.first.first, bufferPositions.first.second, isSecondaryHexModeWindow));
        auto decoratedEndPosition = std::make_pair(bufferPositions.second.first, bufferFiller->getScreenBuffer().mapScreenBufferColumnToDecoratedColumnEnd(bufferPositions.second.first, bufferPositions.second.second, currentFeatherMode, isSecondaryHexModeWindow));
        return std::make_pair(decoratedStartPosition, decoratedEndPosition);
    };

    std::function<std::string(std::string)> Lambda::getFolderName = [](std::string filePath)
    {
        auto lastSlashPos = filePath.rfind('/');
        if (std::string::npos != lastSlashPos)
        {
            filePath.erase(lastSlashPos + 1UL);
        }

        if (filePath.find("./") != 0 && filePath.find("../") != 0 && filePath.find("/") != 0)
        {
            return "./" + filePath;
        }
        return filePath;
    };

    std::function<std::vector<char>(std::vector<char> const &, size_t, size_t)> Lambda::getSubVector = [](std::vector<char> const &input, size_t startIdx, size_t size)
    {
        return std::vector<char>(std::next(input.begin(), startIdx), std::next(input.begin(), startIdx + std::min(input.size() - startIdx, size)));
    };

    std::function<std::basic_string<char>(std::basic_string<char> const &, size_t, size_t)> Lambda::getSubString= [](std::basic_string<char> const &input, size_t startIdx, size_t size)
    {
        return std::basic_string<char>(std::next(input.begin(), startIdx), std::next(input.begin(), startIdx + std::min(input.size() - startIdx, size)));
    };

    std::function<std::vector<char>(std::string const &, size_t, size_t)> Lambda::getSubVectorFromString = [](std::string const &input, size_t startIdx, size_t size)
    {
        return std::vector<char>(std::next(input.begin(), startIdx), std::next(input.begin(), startIdx + std::min(input.size() - startIdx, size)));
    };

    std::function<std::string(std::string const &, std::u32string, std::shared_ptr<utils::FilesystemInterface>)> Lambda::getAbsolutePath = [](std::string const &fileName, std::u32string commandBuffer, std::shared_ptr<utils::FilesystemInterface> filesystem)
    {
        if (commandBuffer.empty())
        {
            return filesystem->getAbsolutePath(fileName);
        }
        std::vector<std::u32string> splittedCommand = utils::helpers::Conversion::splitString(commandBuffer);
        auto currentDir = utils::helpers::Lambda::mapStringToWideString(filesystem->getCurrentDirectory());
        auto sourceFileAbsoluteDir = ((splittedCommand.size() == 1UL) || (splittedCommand[1].front() != U'/' && splittedCommand[1].front() != U'~')) ? currentDir : U"";
        std::u32string filePath;
        if (splittedCommand.size() > 1UL)
        {
            if (0 == splittedCommand[1UL].find(U"./"))
            {
                splittedCommand[1UL].erase(0, 1);
                filePath = splittedCommand[1UL];
            }
            else if (U'/' != splittedCommand[1UL][0] && U"~" != splittedCommand[1UL])
            {
                filePath = U"/" + splittedCommand[1UL];
            }
            else
            {
                filePath = splittedCommand[1UL];
            }
            for (size_t i = 2UL; i < splittedCommand.size(); ++i)
            {
                filePath += U" " + splittedCommand[i];
            }

            if (filePath.size() > 1UL && U'/' == filePath[0] && U'~' == filePath[1])
            {
                if (U'/' == filePath[2])
                {
                    filePath = mapStringToWideString(configuration->getHomeDirectory()) + filePath.substr(2UL);
                }
                else
                {
                    filePath = mapStringToWideString(configuration->getHomeDirectory()) + U'/' + filePath.substr(1UL);
                }
            }
            else if (U"~" == filePath)
            {
                filePath = mapStringToWideString(configuration->getHomeDirectory());
            }
        }
        std::u32string candidatePath = sourceFileAbsoluteDir + filePath;
        if ('/' == candidatePath.back())
        {
            candidatePath += mapStringToWideString(fileName).substr(fileName.rfind(U'/') + 1UL, fileName.length());
        }
        return filesystem->getAbsolutePath(utils::helpers::Conversion::squeezeu32String(candidatePath));
    };

    std::function<std::pair<size_t, std::string>(std::string)> Lambda::getValueWithUnit = [](std::string arg)
    {
        size_t pos = 0;
        while (std::isdigit(arg[pos]))
        {
            ++pos;
        }
        std::string value = arg.substr(0, pos);
        std::string unit = arg.substr(pos, arg.size());
        if (unit.empty())
        {
            return std::pair<size_t, std::string>(std::stoull(value), "B");
        }
        std::transform(unit.begin(), unit.end(), unit.begin(), [](unsigned char c)
                       { return std::toupper(c); });
        if (unit != "B" && unit != "KB" && unit != "M" && unit != "MB")
        {
            throw utils::exception::FeatherInvalidArgumentException("Invalid value : " + arg);
        }
        return std::pair<size_t, std::string>(std::stoull(value), unit);
    };

    std::function<std::u32string(std::u32string)> Lambda::convertStrigifiedHexToHex = [](std::u32string input)
    {
        std::u32string result;
        std::transform(input.begin(), input.end(), input.begin(), ::toupper);
        if (std::u32string::npos != input.find(U"0X"))
        {
            input.erase(0UL, 2UL);
        }
        char32_t twoNibbles = 0;
        bool twoNibblesFilled = false;
        for (char32_t c : input)
        {
            switch (c)
            {
            case U'0':
            case U'1':
            case U'2':
            case U'3':
            case U'4':
            case U'5':
            case U'6':
            case U'7':
            case U'8':
            case U'9':
            {
                twoNibbles |= (c - U'0');
                if (twoNibblesFilled)
                {
                    result.push_back(twoNibbles);
                    twoNibbles = 0;
                    twoNibblesFilled ^= true;
                    break;
                }
                twoNibblesFilled ^= true;
                twoNibbles <<= 4UL;
                break;
            }
            case U'A':
            case U'B':
            case U'C':
            case U'D':
            case U'E':
            case U'F':
            {
                twoNibbles |= (0x0A + (c - U'A'));
                if (twoNibblesFilled)
                {
                    result.push_back(twoNibbles);
                    twoNibbles = 0;
                    twoNibblesFilled ^= true;
                    break;
                }
                twoNibblesFilled ^= true;
                twoNibbles <<= 4UL;
                break;
            }
            default:
                throw utils::exception::FeatherInvalidArgumentException(U"Not valid hex string");
            }
        }
        if (0 != twoNibbles)
        {
            twoNibbles >>= 4UL;
            result.push_back(twoNibbles);
        }
        return result;
    };

    std::function<std::pair<pair, std::optional<char32_t>>(pair, std::function<bool(char32_t)> predicate, std::shared_ptr<printer::PrintingOrchestratorInterface>, utils::datatypes::Uuid const &, std::shared_ptr<utils::storage::AbstractStorage>, std::shared_ptr<feather::windows::WindowsManager>, bool, bool)> Lambda::findNext = [](pair cursorPos, std::function<bool(char32_t)> predicate, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator, utils::datatypes::Uuid const windowUUID, std::shared_ptr<utils::storage::AbstractStorage> storage, std::shared_ptr<feather::windows::WindowsManager> windowsManager, bool skipFirstChar, bool resetStartTime)
    {
        static auto startTime = std::chrono::steady_clock::now();
        static bool alreadyRefreshed = false;
        if (resetStartTime)
        {
            startTime = std::chrono::steady_clock::now();
            alreadyRefreshed = false;
        }
        feather::utils::helpers::unblockSignals();
        std::unique_ptr<printer::PrintingVisitorInterface> currFrameVisitor = std::make_unique<printer::CurrFrameVisitor>(printingOrchestrator);
        std::unique_ptr<utils::algorithm::SearchBuffer> localBuffer = std::make_unique<utils::algorithm::SearchBuffer>(windowUUID, printingOrchestrator, cursorPos);
        auto readChunk = printingOrchestrator->getContinousCharacters(cursorPos, configuration->getMemoryBytesRelaxed(), storage, windowUUID, true);
        timeout(0);
        while (!readChunk.first.empty())
        {
            localBuffer->addEntry(std::move(readChunk.first));
            if (!alreadyRefreshed && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime) > HALF_SECOND)
            {
                currentFeatherMode = feather::utils::FEATHER_MODE::WORKING_MODE;
                windowsManager->refreshAllWindows(currFrameVisitor);
                alreadyRefreshed = true;
            }

            while (localBuffer->getNoUnreadBytes())
            {
                auto character = localBuffer->getNextCharacter();
                if (skipFirstChar)
                {
                    skipFirstChar = false;
                    cursorPos = localBuffer->getCurrentPosition();
                    continue;
                }
                if (predicate(character))
                {
                    timeout(-1);
                    feather::utils::helpers::blockSignals();
                    alreadyRefreshed = false;
                    return std::make_pair(cursorPos, std::optional<char32_t>(character));
                }
                cursorPos = localBuffer->getCurrentPosition();
                if (KEY_RESIZE == windowsManager->getMainWindow(windowUUID)->getLibraryWindowHandler()->getCharacter())
                {
                    currentFeatherMode = feather::utils::FEATHER_MODE::WORKING_MODE;
                    alreadyRefreshed = true;
                    windowsManager->recreateWindows();
                    windowsManager->refreshAllWindows(currFrameVisitor);
                }
                else if (sigIntReceived)
                {
                    timeout(-1);
                    subWindowPrint(utils::datatypes::Strings::INTERRUPTED, ONE_SECOND, windowsManager->getSubWindow(windowsManager->getBottomBarWindow(windowUUID).value()), windowsManager->getSubWindow(windowsManager->getProgressBarWindow(windowUUID).value()), currFrameVisitor);
                    feather::utils::helpers::blockSignals();
                    throw exception::FeatherInterruptedException("");
                }
            }
            readChunk = printingOrchestrator->getContinousCharacters(cursorPos, configuration->getMemoryBytesRelaxed(), storage, windowUUID);
        }
#ifndef _FEATHER_TEST_
        timeout(-1);
        feather::utils::helpers::blockSignals();
#endif
        return std::pair<pair, std::optional<char32_t>>(cursorPos, std::nullopt);
    };

    std::function<std::pair<pair, std::optional<char32_t>>(pair, std::function<bool(char32_t)> predicate, std::shared_ptr<printer::PrintingOrchestratorInterface>, utils::datatypes::Uuid const &, std::shared_ptr<utils::storage::AbstractStorage>, std::shared_ptr<feather::windows::WindowsManager>, size_t, bool, bool)> Lambda::findNthNext = [](pair cursorPos, std::function<bool(char32_t)> predicate, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator, utils::datatypes::Uuid const windowUUID, std::shared_ptr<utils::storage::AbstractStorage> storage, std::shared_ptr<feather::windows::WindowsManager> windowsManager, size_t nth, bool skipFirstChar, bool resetStartTime)
    {
        static auto startTime = std::chrono::steady_clock::now();
        static bool alreadyRefreshed = false;
        if (resetStartTime)
        {
            startTime = std::chrono::steady_clock::now();
            alreadyRefreshed = false;
        }
        feather::utils::helpers::unblockSignals();
        std::unique_ptr<printer::PrintingVisitorInterface> currFrameVisitor = std::make_unique<printer::CurrFrameVisitor>(printingOrchestrator);
        std::unique_ptr<utils::algorithm::SearchBuffer> localBuffer = std::make_unique<utils::algorithm::SearchBuffer>(windowUUID, printingOrchestrator, cursorPos);
        auto readChunk = printingOrchestrator->getContinousCharacters(cursorPos, configuration->getMemoryBytesRelaxed(), storage, windowUUID, true);
        timeout(0);
        while (!readChunk.first.empty())
        {
            localBuffer->addEntry(std::move(readChunk.first));
            if (!alreadyRefreshed && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime) > HALF_SECOND)
            {
                currentFeatherMode = feather::utils::FEATHER_MODE::WORKING_MODE;
                windowsManager->refreshAllWindows(currFrameVisitor);
                alreadyRefreshed = true;
            }

            while (localBuffer->getNoUnreadBytes())
            {
                auto character = localBuffer->getNextCharacter();
                if (skipFirstChar)
                {
                    skipFirstChar = false;
                    cursorPos = localBuffer->getCurrentPosition();
                    continue;
                }
                if (predicate(character))
                {
                    if (0 == --nth)
                    {
                        timeout(-1);
                        feather::utils::helpers::blockSignals();
                        return std::make_pair(cursorPos, std::optional<char32_t>(character));
                    }
                }
                cursorPos = localBuffer->getCurrentPosition();
                if (KEY_RESIZE == windowsManager->getMainWindow(windowUUID)->getLibraryWindowHandler()->getCharacter())
                {
                    currentFeatherMode = feather::utils::FEATHER_MODE::WORKING_MODE;
                    alreadyRefreshed = true;
                    windowsManager->recreateWindows();
                    windowsManager->refreshAllWindows(currFrameVisitor);
                }
                else if (sigIntReceived)
                {
                    timeout(-1);
                    subWindowPrint(utils::datatypes::Strings::INTERRUPTED, ONE_SECOND, windowsManager->getSubWindow(windowsManager->getBottomBarWindow(windowUUID).value()), windowsManager->getSubWindow(windowsManager->getProgressBarWindow(windowUUID).value()), currFrameVisitor);
                    feather::utils::helpers::blockSignals();
                    throw exception::FeatherInterruptedException("");
                }
            }
            readChunk = printingOrchestrator->getContinousCharacters(cursorPos, configuration->getMemoryBytesRelaxed(), storage, windowUUID);
        }
        timeout(-1);
        feather::utils::helpers::blockSignals();
        return std::pair<pair, std::optional<char32_t>>(cursorPos, std::nullopt);
    };

    std::function<std::pair<pair, std::optional<char32_t>>(pair, std::function<bool(char32_t)> predicate, std::shared_ptr<printer::PrintingOrchestratorInterface>, utils::datatypes::Uuid const &, std::shared_ptr<utils::storage::AbstractStorage>, std::shared_ptr<feather::windows::WindowsManager>, bool)> Lambda::findPrevious = [](pair cursorPos, std::function<bool(char32_t)> predicate, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator, utils::datatypes::Uuid const windowUUID, std::shared_ptr<utils::storage::AbstractStorage> storage, std::shared_ptr<feather::windows::WindowsManager> windowsManager, bool resetStartTime)
    {
        static auto startTime = std::chrono::steady_clock::now();
        static bool alreadyRefreshed = false;
        if (resetStartTime)
        {
            startTime = std::chrono::steady_clock::now();
            alreadyRefreshed = false;
        }
        feather::utils::helpers::unblockSignals();
        std::unique_ptr<printer::PrintingVisitorInterface> currFrameVisitor = std::make_unique<printer::CurrFrameVisitor>(printingOrchestrator);
        std::unique_ptr<utils::algorithm::SearchBuffer> localBuffer = std::make_unique<utils::algorithm::SearchBuffer>(windowUUID, printingOrchestrator, cursorPos);
        auto readChunk = printingOrchestrator->getContinousCharactersBackward(cursorPos, configuration->getMemoryBytesRelaxed(), storage, windowUUID);
        timeout(0);
        while (!readChunk.first.empty())
        {
            localBuffer->addEntryToFront(std::move(readChunk.first));
            if (!alreadyRefreshed && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime) > HALF_SECOND)
            {
                currentFeatherMode = feather::utils::FEATHER_MODE::WORKING_MODE;
                windowsManager->refreshAllWindows(currFrameVisitor);
                alreadyRefreshed = true;
            }

            while (localBuffer->getNoUnreadBytes())
            {
                char32_t character = localBuffer->getPreviousCharacter();
                if (predicate(character))
                {
                    timeout(-1);
                    feather::utils::helpers::blockSignals();
                    return std::pair<pair, std::optional<char32_t>>(localBuffer->getCurrentPosition(), character);
                }
                cursorPos = localBuffer->getCurrentPosition();
                if (KEY_RESIZE == windowsManager->getMainWindow(windowUUID)->getLibraryWindowHandler()->getCharacter())
                {
                    currentFeatherMode = feather::utils::FEATHER_MODE::WORKING_MODE;
                    alreadyRefreshed = true;
                    windowsManager->recreateWindows();
                    windowsManager->refreshAllWindows(currFrameVisitor);
                }
                else if (sigIntReceived)
                {
                    timeout(-1);
                    subWindowPrint(utils::datatypes::Strings::INTERRUPTED, ONE_SECOND, windowsManager->getSubWindow(windowsManager->getBottomBarWindow(windowUUID).value()), windowsManager->getSubWindow(windowsManager->getProgressBarWindow(windowUUID).value()), currFrameVisitor);
                    feather::utils::helpers::blockSignals();
                    throw exception::FeatherInterruptedException("");
                }
            }
            readChunk = printingOrchestrator->getContinousCharactersBackward(cursorPos, configuration->getMemoryBytesRelaxed(), storage, windowUUID);
        }
        timeout(-1);
        feather::utils::helpers::blockSignals();
        return std::pair<pair, std::optional<char32_t>>(cursorPos, std::nullopt);
    };

    std::function<std::pair<std::basic_string<wchar_t>, std::vector<unsigned int>>(std::basic_string<char> const &)> Lambda::charToWchar = [](std::basic_string<char> const &input)
    {
        std::basic_string<wchar_t> output;
        output.reserve(input.size());
        std::vector<unsigned int> posMapping;
        posMapping.reserve(input.size());
        char32_t utf8Character = 0ULL;
        unsigned short utf8CharacterBytesTaken = 0;

        for (unsigned int idx = 0; idx < input.size();)
        {
            if (sigIntReceived || sigBusReceived)
            {
                throw utils::exception::FeatherInterruptedException("");
            }
            utf8Character = 0ULL;
            memcpy((void*)&utf8Character, (void*)&input[idx], std::min(MAX_UTF8_CHAR_BYTES, input.size() - idx));
            utf8CharacterBytesTaken = utils::Utf8Util::getCharacterBytesTakenLight(utf8Character);
            memset(reinterpret_cast<char*>(&utf8Character) + utf8CharacterBytesTaken, 0, sizeof(char32_t) - utf8CharacterBytesTaken);
            posMapping.push_back(idx);
            idx += utf8CharacterBytesTaken;
            output.push_back(utils::Utf8Util::getCodePoint(utf8Character, true));
        }
        return std::make_pair(output, posMapping);
    };

    std::function<std::pair<bool, std::optional<char32_t>>(pair, std::function<bool(char32_t)>, utils::datatypes::Uuid const &, std::shared_ptr<utils::storage::AbstractStorage>, std::shared_ptr<printer::PrintingOrchestratorInterface>)> Lambda::isCharAtPos = [](pair pos, std::function<bool(char32_t)> predicate, utils::datatypes::Uuid const &windowUUID, std::shared_ptr<utils::storage::AbstractStorage> storage, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator)
    {
        auto continousCharacters = printingOrchestrator->getContinousCharacters(pos, MAX_UTF8_CHAR_BYTES, storage, windowUUID).first;
        if (continousCharacters.empty())
        {
            return std::pair<bool, std::optional<char32_t>>(false, std::nullopt);
        }
        auto character = utils::helpers::Conversion::vectorOfCharToChar32_t(continousCharacters);
        return std::pair<bool, std::optional<char32_t>>(predicate(character), character);
    };

    std::function<std::u32string(std::string)> Lambda::mapStringToWideString = [](std::string const &string)
    {
        std::u32string result;
        for (size_t idx = 0; idx < string.size();)
        {
            result.push_back(utils::Utf8Util::getUtf8Character(string, idx));
            idx += utils::Utf8Util::getCharacterBytesTaken(std::vector<char>(std::next(string.begin(), idx), std::next(string.begin(), idx + std::min(4UL, string.size() - idx))));
        }
        return result;
    };

    std::function<void(utils::datatypes::Uuid const &, std::string)> Lambda::removeFilesContainingSubstring = [](utils::datatypes::Uuid const &windowUUID, std::string substr)
    {
        std::string dir = configuration->getStorageDirectory() + windowUUID.getString();
        if (!std::filesystem::exists(dir))
        {
            return;
        }
        for (const auto &e : std::filesystem::directory_iterator(std::filesystem::path(dir)))
        {
            if (e.path().filename().string().find(substr) != std::string::npos)
            {
                std::filesystem::remove(e);
            }
        }
    };

    std::function<std::vector<char>(std::vector<char> &&)> Lambda::alignBufferBegining = [](std::vector<char> &&buffer)
    {
        size_t pos = 0;
        size_t maxCheck = 3UL;
        while (pos < buffer.size() && utils::Utf8Util::isMiddleOfCharacter(buffer[pos]) && maxCheck--)
        {
            ++pos;
        }
        buffer.erase(buffer.begin(), std::next(buffer.begin(), pos));
        return std::move(buffer);
    };

    std::function<std::string(std::string &&)> Lambda::alignBufferBeginingForString = [](std::string &&buffer)
    {
        size_t pos = 0;
        size_t maxCheck = 3UL;
        while (pos < buffer.size() && utils::Utf8Util::isMiddleOfCharacter(buffer[pos]) && maxCheck--)
        {
            ++pos;
        }
        buffer.erase(buffer.begin(), std::next(buffer.begin(), pos));
        return std::move(buffer);
    };

    std::function<std::set<std::u32string>(std::u32string const &)> Lambda::getAllFilesInDirectory = [](std::u32string const &dir)
    {
#ifdef _FEATHER_TEST_
        return utils::storage::InMemoryStorageFactory::getAllFiles();
#else
        std::string squeezedWideString = utils::helpers::Conversion::squeezeu32String(dir);
        std::set<std::u32string> result;
        if (!std::filesystem::is_directory(std::filesystem::path(squeezedWideString)) || (std::filesystem::is_directory(std::filesystem::path(squeezedWideString)) && !std::unique_ptr<feather::utils::Filesystem>(new utils::Filesystem())->hasFolderExecutePermissions(squeezedWideString)))
        {
            return result;
        }
        try
        {
            for (const auto &e : std::filesystem::directory_iterator(std::filesystem::path(squeezedWideString)))
            {
                result.insert(utils::helpers::Lambda::mapStringToWideString(e.is_directory() ? e.path().filename().string() + '/' : e.path().filename().string()));
            }
        }
        catch (std::exception const &e)
        {
            return result;
        }
        return result;
#endif
    };

    std::function<std::map<pair, pair>(std::map<pair, pair> const &, utils::datatypes::Uuid const &, std::shared_ptr<printer::PrintingOrchestratorInterface>, size_t)> Lambda::mergeInterval = [](std::map<pair, pair> const &intervals, utils::datatypes::Uuid const &windowUUID, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator, size_t diff)
    {
        std::map<pair, pair> toReturn;
        if (intervals.size() > 1)
        {
            auto startInterval = intervals.begin();
            auto currLeftInterval = startInterval, currRightInterval = std::next(startInterval);
            while (currRightInterval != intervals.end())
            {
                if (printingOrchestrator->getDiffBytes(currLeftInterval->second, currRightInterval->first, windowUUID) <= diff)
                {
                    currLeftInterval = currRightInterval;
                    currRightInterval = std::next(currRightInterval);
                }
                else
                {
                    toReturn.insert(std::make_pair(startInterval->first, currLeftInterval->second));
                    startInterval = currRightInterval;
                    currLeftInterval = currRightInterval;
                    currRightInterval = std::next(currRightInterval);
                }
            }
            toReturn.insert(std::make_pair(startInterval->first, currLeftInterval->second));
        }
        else
        {
            toReturn = intervals;
        }
        return toReturn;
    };

    std::function<void(std::shared_ptr<feather::windows::WindowsManager>, std::shared_ptr<printer::PrintingOrchestratorInterface>, std::unique_ptr<printer::PrintingVisitorInterface> &, std::unique_ptr<printer::PrintingVisitorInterface> &, pair)> Lambda::setCursor = [](std::shared_ptr<feather::windows::WindowsManager> windowsManager, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator, std::unique_ptr<printer::PrintingVisitorInterface> &currFrameVisitor, std::unique_ptr<printer::PrintingVisitorInterface> &halfFrameUpVisitor, pair cursorPos)
    {
        if (!utils::helpers::Lambda::isScreenBigEnough())
        {
            return;
        }
        auto mainWindow = windowsManager->getMainWindow(windowsManager->getActiveMainWindow());
        if (isHexMode)
        {
            auto hexWindow = windowsManager->getMainWindow(windowsManager->getSecondaryActiveMainWindowForHexMode());
            hexWindow->setCursorFromRealPosition(cursorPos, printingOrchestrator, halfFrameUpVisitor);
            hexWindow->showSecondaryWindowCursor(std::make_pair(cursorPos, printingOrchestrator->getNextVirtualPosition(mainWindow->getUUID(), mainWindow->getStorage(), cursorPos)), printingOrchestrator);
            windowsManager->refreshAllWindows(currFrameVisitor);
        }
        mainWindow->setCursorFromRealPosition(cursorPos, printingOrchestrator, halfFrameUpVisitor);
    };

    std::function<std::u32string(std::u32string const &)> Lambda::fixPath = [](std::u32string const &path)
    {
        if (path.empty())
        {
            return std::u32string(U"./");
        }
        else if (U"/" == path)
        {
            return path;
        }
        else if (U"~" == path || U"~/" == path)
        {
            return mapStringToWideString(configuration->getHomeDirectory()) + U'/';
        }
        else if (path.size() >= 2UL && path[0] == U'~' && path[1] == U'/')
        {
            return mapStringToWideString(configuration->getHomeDirectory()) + U'/' + path.substr(2UL);
        }
        else if (path.size() >= 2UL && path.rfind(U"..") == (path.size() - 2UL))
        {
            return path + U"/";
        }
        /*else if (path.size() > 1 && U'~' == path[0])
        {
            if (U'/' == path[1])
            {
                return mapStringToWideString(configuration->getHomeDirectory()) + path.substr(1UL);
            }
            return mapStringToWideString(configuration->getHomeDirectory()) + U'/' + path.substr(1UL);
        }
        */
        else if (U'/' != path[0] && U'.' != path[0] && U'~' != path[0])
        {
            return U"./" + path;
        }
        return path;
    };

    std::function<void(std::list<pair> &, pair const &, size_t, std::list<pair>::iterator &)> Lambda::shiftPairs = [](std::list<pair> &pairs, pair const &pos, size_t size, std::list<pair>::iterator &currentIt)
    {
        size_t offset = 0;
        if (currentIt != pairs.end())
        {
            offset = std::distance(pairs.begin(), currentIt);
        }
        std::list<pair> shiftedPairs;
        for (auto const &p : pairs)
        {
            if (pos.first == p.first && pos.second <= p.second)
            {
                shiftedPairs.push_back(pair(p.first, p.second + size));
            }
            else
            {
                shiftedPairs.push_back(p);
            }
        }

        if (currentIt != pairs.end())
        {
            pairs = shiftedPairs;
            currentIt = std::next(pairs.begin(), offset);
        }
        else
        {
            pairs = shiftedPairs;
            currentIt = pairs.end();
        }
    };

    std::function<std::set<pair>(std::set<pair> const &, pair const &, size_t)> Lambda::shiftSetPairs = [](std::set<pair> const &pairs, pair const &pos, size_t size)
    {
        std::set<pair> shiftedPairs;
        for (auto const &p : pairs)
        {
            if (pos.first == p.first && pos.second <= p.second)
            {
                shiftedPairs.insert(pair(p.first, p.second + size));
            }
            else
            {
                shiftedPairs.insert(p);
            }
        }
        return shiftedPairs;
    };

    std::function<void(std::u32string const &, std::u32string const &, std::chrono::seconds, std::unique_ptr<feather::windows::SubWindowInterface> &)> Lambda::blinkPrint = [](std::u32string const &newText, std::u32string const &oldText, std::chrono::seconds duration, std::unique_ptr<feather::windows::SubWindowInterface> &window)
    {
        if (!isScreenBigEnough())
        {
            return;
        }
        std::optional<pair> savedCursorPos = std::nullopt;
        bool shouldDisableCursor = window->getLibraryWindowHandler()->isCursorEnabled();
        if (shouldDisableCursor)
        {
            savedCursorPos = window->getLibraryWindowHandler()->getCursorPosition();
            window->disableCursor();
        }
        std::vector<char32_t> vectoredNewText(newText.begin(), newText.end());
        std::vector<char32_t> vectoredOldText(oldText.begin(), oldText.end());
        window->print(utils::ScreenBuffer(vectoredNewText, 1UL));
        utils::NCursesWrapper::applyRefresh();
        std::this_thread::sleep_for(duration);
        window->print(utils::ScreenBuffer(vectoredOldText, 1UL));
        if (shouldDisableCursor)
        {
            window->getLibraryWindowHandler()->setCursorAtPos(savedCursorPos.value());
            window->enableCursor();
        }
        utils::NCursesWrapper::applyRefresh();
    };

    std::function<void(std::u32string const &, std::optional<std::chrono::milliseconds>, std::unique_ptr<feather::windows::SubWindowInterface> &, std::unique_ptr<feather::windows::SubWindowInterface> &, std::unique_ptr<printer::PrintingVisitorInterface> &)> Lambda::subWindowPrintMs = [](std::u32string const &newText, std::optional<std::chrono::milliseconds> duration, std::unique_ptr<feather::windows::SubWindowInterface> &subWindow, std::unique_ptr<feather::windows::SubWindowInterface> &dependentWindow, std::unique_ptr<printer::PrintingVisitorInterface> &visitor)
    {
        if (!isScreenBigEnough())
        {
            return;
        }
        bool shouldDisableCursor = subWindow->getLibraryWindowHandler()->isCursorEnabled();
        if (shouldDisableCursor)
        {
            subWindow->disableCursor();
        }
        std::vector<char32_t> vectoredNewText(newText.begin(), newText.end());
        subWindow->print(utils::ScreenBuffer(vectoredNewText, 1UL));
        dependentWindow->refreshBuffer(visitor.get());
        if (shouldDisableCursor)
        {
            subWindow->enableCursor();
        }
        utils::NCursesWrapper::applyRefresh();
        if (duration.has_value())
        {
            std::this_thread::sleep_for(duration.value());
        }
    };

    std::function<void(std::u32string const &, std::optional<std::chrono::seconds>, std::unique_ptr<feather::windows::SubWindowInterface> &, std::unique_ptr<feather::windows::SubWindowInterface> &, std::unique_ptr<printer::PrintingVisitorInterface> &)> Lambda::subWindowPrint = [](std::u32string const &newText, std::optional<std::chrono::seconds> duration, std::unique_ptr<feather::windows::SubWindowInterface> &subWindow, std::unique_ptr<feather::windows::SubWindowInterface> &dependentWindow, std::unique_ptr<printer::PrintingVisitorInterface> &visitor)
    {
        if (!isScreenBigEnough())
        {
            return;
        }
        bool shouldDisableCursor = subWindow->getLibraryWindowHandler()->isCursorEnabled();
        if (shouldDisableCursor)
        {
            subWindow->disableCursor();
        }
        std::vector<char32_t> vectoredNewText(newText.begin(), newText.end());
        subWindow->print(utils::ScreenBuffer(vectoredNewText, 1UL));
        dependentWindow->refreshBuffer(visitor.get());
        if (shouldDisableCursor)
        {
            subWindow->enableCursor();
        }
        utils::NCursesWrapper::applyRefresh();
        if (duration.has_value())
        {
            std::this_thread::sleep_for(duration.value());
        }
    };

    std::function<void(std::shared_ptr<feather::windows::WindowsManager>, std::unique_ptr<printer::PrintingVisitorInterface> &, utils::FEATHER_MODE)> Lambda::restoreFeatherMode = [](std::shared_ptr<feather::windows::WindowsManager> windowsManager, std::unique_ptr<printer::PrintingVisitorInterface> &visitor, utils::FEATHER_MODE mode)
    {
        currentFeatherMode = mode;
        windowsManager->refreshAllWindows(visitor);
    };

    std::function<void(std::u32string const &, std::unique_ptr<feather::windows::SubWindowInterface> &, std::unique_ptr<feather::windows::SubWindowInterface> &, std::unique_ptr<printer::PrintingVisitorInterface> &)> Lambda::subWindowPrintWithoutDelay = [](std::u32string const &newText, std::unique_ptr<feather::windows::SubWindowInterface> &subWindow, std::unique_ptr<feather::windows::SubWindowInterface> &dependentWindow, std::unique_ptr<printer::PrintingVisitorInterface> &visitor)
    {
        if (!isScreenBigEnough())
        {
            return;
        }
        std::vector<char32_t> vectoredNewText(newText.begin(), newText.end());
        subWindow->print(utils::ScreenBuffer(vectoredNewText, 1UL));
        dependentWindow->refreshBuffer(visitor.get());
        subWindow->enableCursor();
        utils::NCursesWrapper::applyRefresh();
    };


    std::function<void(std::u32string const &, std::optional<std::chrono::seconds>, std::unique_ptr<feather::windows::SubWindowInterface> &, std::unique_ptr<printer::PrintingVisitorInterface> &)> Lambda::subWindowPrintWithoutDependent = [](std::u32string const &newText, std::optional<std::chrono::seconds> duration, std::unique_ptr<feather::windows::SubWindowInterface> &subWindow, std::unique_ptr<printer::PrintingVisitorInterface> &visitor)
    {
        if (!isScreenBigEnough())
        {
            return;
        }
        bool shouldDisableCursor = subWindow->getLibraryWindowHandler()->isCursorEnabled();
        if (shouldDisableCursor)
        {
            subWindow->disableCursor();
        }
        std::vector<char32_t> vectoredNewText(newText.begin(), newText.end());
        subWindow->print(utils::ScreenBuffer(vectoredNewText, 1UL));
        if (shouldDisableCursor)
        {
            subWindow->enableCursor();
        }
        utils::NCursesWrapper::applyRefresh();
        if (duration.has_value())
        {
            std::this_thread::sleep_for(duration.value());
        }
    };
} // namespace feather::utils::helpers
