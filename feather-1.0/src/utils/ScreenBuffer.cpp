#include "utils/ScreenBuffer.hpp"
#include "utils/Utf8Util.hpp"
#include "utils/exception/FeatherMemoryException.hpp"

#include <stack>

extern std::unique_ptr<const feather::config::Configuration> configuration;

namespace feather::utils
{
    ScreenBuffer::ScreenBuffer(size_t noRowsArg)
        : lastFilledLine(0), noRows(noRowsArg)
    {
        try
        {
            internalBuffer = std::vector<std::vector<char32_t>>(noRows, std::vector<char32_t>());
            internalBufferAsString.reserve(1000 * noRows);
        }
        catch (std::bad_alloc const &e)
        {
            throw exception::FeatherMemoryException("Could not allocate memory for new window...");
        }
    }

    ScreenBuffer::ScreenBuffer(size_t filledRowsArg, size_t noRowsArg, size_t maxRowSize)
        : lastFilledLine(filledRowsArg), noRows(noRowsArg)
    {
        try
        {
            internalBuffer = std::vector<std::vector<char32_t>>(noRows, std::vector<char32_t>());
            for (auto &row : internalBuffer)
            {
                row.reserve(maxRowSize);
            }
            internalBufferAsString.reserve(1000 * noRows);
        }
        catch (std::bad_alloc const &e)
        {
            throw exception::FeatherMemoryException("Could not allocate memory for new window...");
        }
    }

    ScreenBuffer::ScreenBuffer(std::vector<char32_t> &&oneLineBuffer, size_t noRowsArg, size_t lineLength)
        : lastFilledLine(0), noRows(noRowsArg)
    {
        try
        {
            internalBuffer = std::vector<std::vector<char32_t>>(noRows, std::vector<char32_t>());
            if (oneLineBuffer.size() < lineLength)
            {
                for (size_t idx = 0; idx < (lineLength - oneLineBuffer.size()); ++idx)
                {
                    oneLineBuffer.emplace_back(U' ');
                }
            }
            std::copy(oneLineBuffer.begin(), oneLineBuffer.end(), std::back_inserter(internalBuffer[0]));
            std::copy(oneLineBuffer.begin(), oneLineBuffer.end(), std::back_inserter(internalBufferAsString));
            ++lastFilledLine;
        }
        catch (std::bad_alloc const &e)
        {
            throw exception::FeatherMemoryException("Could not allocate memory for new window...");
        }
    }

    ScreenBuffer::ScreenBuffer(std::vector<char32_t> &oneLineBuffer, size_t noRowsArg)
        : lastFilledLine(0), noRows(noRowsArg)
    {
        try
        {
            internalBuffer = std::vector<std::vector<char32_t>>(noRowsArg, std::vector<char32_t>());
            std::copy(oneLineBuffer.begin(), oneLineBuffer.end(), std::back_inserter(internalBuffer[0]));
            std::copy(oneLineBuffer.begin(), oneLineBuffer.end(), std::back_inserter(internalBufferAsString));
            ++lastFilledLine;
        }
        catch (std::bad_alloc const &e)
        {
            throw exception::FeatherMemoryException("Could not allocate memory for new window...");
        }
    }

    ScreenBuffer::ScreenBuffer(std::vector<char32_t> &&oneLineBuffer, size_t noRowsArg)
        : lastFilledLine(0), noRows(noRowsArg)
    {
        try
        {
            internalBuffer = std::vector<std::vector<char32_t>>(noRows, std::vector<char32_t>());
            std::copy(oneLineBuffer.begin(), oneLineBuffer.end(), std::back_inserter(internalBufferAsString));
            internalBuffer[0].swap(oneLineBuffer);
            ++lastFilledLine;
        }
        catch (std::bad_alloc const &e)
        {
            throw exception::FeatherMemoryException("Could not allocate memory for new window...");
        }
    }

    ScreenBuffer::ScreenBuffer(ScreenBuffer &&other)
    {
        noRows = other.noRows;
        lastFilledLine = other.lastFilledLine;
        std::swap(internalBuffer, other.internalBuffer);
        std::swap(internalBufferAsString, other.internalBufferAsString);
    }

    ScreenBuffer::ScreenBuffer(ScreenBuffer const &other)
    {
        noRows = other.noRows;
        lastFilledLine = other.lastFilledLine;
        internalBuffer = other.internalBuffer;
        internalBufferAsString = other.internalBufferAsString;
    }

    ScreenBuffer &ScreenBuffer::operator=(ScreenBuffer const &r)
    {
        this->internalBuffer = r.internalBuffer;
        this->internalBufferAsString = r.internalBufferAsString;
        this->highlightedText = r.highlightedText;
        this->highlightedTextChunks = r.highlightedTextChunks;
        this->blinkMap = r.blinkMap;
        this->colorMap = r.colorMap;
        this->posMappedToHex = r.posMappedToHex;
        this->posMappedToHexStart = r.posMappedToHexStart;
        this->highlightedBrackets = r.highlightedBrackets;
        this->lastFilledLine = r.lastFilledLine;
        this->noRows = r.noRows;
        return *this;
    }

    bool ScreenBuffer::operator==(utils::ScreenBuffer &r)
    {
        if (getFilledRows() != r.getFilledRows())
        {
            return false;
        }

        for (size_t idx = 0; idx < getFilledRows(); ++idx)
        {
            if (internalBuffer[idx] != r[idx])
            {
                return false;
            }
        }
        return true;
    }

    void ScreenBuffer::applyBlinking(std::pair<pair, pair> const &pos)
    {
        blinkMap[pos.first] = pos.second;
    }

    void ScreenBuffer::applyColors(std::unordered_multimap<std::u32string, std::pair<size_t, std::set<utils::datatypes::COLOR>>> const &highlitedStrings) const
    {
        for (auto &e : highlitedStrings)
        {
            size_t currEntryIdx = 0UL;
            pair startPos;
            for (size_t row = 0UL; row < internalBuffer.size(); ++row)
            {
                for (size_t column = 0UL; column < internalBuffer[row].size(); ++column)
                {
                    if (internalBuffer[row][column] == e.first[currEntryIdx])
                    {
                        if (!currEntryIdx)
                        {
                            startPos = pair(row, column);
                        }
                        ++currEntryIdx;
                        if (e.first.size() == currEntryIdx)
                        {
                            colorMap.emplace(std::make_pair(startPos, std::pair<pair, std::pair<size_t, std::set<utils::datatypes::COLOR>>>(pair(row, column), e.second)));
                            currEntryIdx = 0UL;
                        }
                    }
                    else if (internalBuffer[row][column] == e.first[0])
                    {
                        startPos = pair(row, column);
                        currEntryIdx = 1UL;
                    }
                    else
                    {
                        currEntryIdx = 0UL;
                    }
                }
            }
        }
    }

    bool ScreenBuffer::highlightBrackets(pair currentPos, bool isSecondaryHexMainWindow) const
    {
        if (configuration->isHexMode())
        {
            return false;
        }
        std::map<char32_t, char32_t> openToCloseBracket{{U'(', U')'}, {U'{', U'}'}, {U'[', U']'}, {U'<', U'>'}};
        std::map<char32_t, char32_t> closedToOpenBracket{{U')', U'('}, {U'}', U'{'}, {U']', U'['}, {U'>', U'<'}};
        std::stack<char32_t> bracketsStack;

        if (utils::helpers::Lambda::isOpenBracket(internalBuffer[currentPos.first][currentPos.second]))
        {
            size_t startColumn = currentPos.second;
            char32_t currOpeningBracket = internalBuffer[currentPos.first][currentPos.second];
            for (size_t row = currentPos.first; row < getFilledRows(); ++row)
            {
                for (size_t column = startColumn; column < internalBuffer[row].size(); ++column)
                {
                    if (currOpeningBracket == internalBuffer[row][column])
                    {
                        bracketsStack.push(internalBuffer[row][column]);
                        continue;
                    }
                    else if (utils::helpers::Lambda::isClosingBracket(internalBuffer[row][column]))
                    {
                        if (openToCloseBracket[bracketsStack.top()] == internalBuffer[row][column])
                        {
                            bracketsStack.pop();
                        }
                    }
                    if (bracketsStack.empty())
                    {
                        highlightedBrackets.insert(pair(currentPos.first, mapScreenBufferColumnToDecoratedColumnStart(currentPos.first, currentPos.second, isSecondaryHexMainWindow)));
                        highlightedBrackets.insert(pair(row, mapScreenBufferColumnToDecoratedColumnStart(row, column, isSecondaryHexMainWindow)));
                        return true;
                    }
                }
                startColumn = 0;
            }
        }
        else if (utils::helpers::Lambda::isClosingBracket(internalBuffer[currentPos.first][currentPos.second]))
        {
            int endColumn = static_cast<int>(currentPos.second);
            char32_t currClosingBracket = internalBuffer[currentPos.first][currentPos.second];
            for (int row = currentPos.first; row >= 0; --row)
            {
                endColumn = (endColumn == -1) ? internalBuffer[row].size() - 1 : endColumn;
                for (int column = endColumn; column >= 0; --column)
                {
                    if (currClosingBracket == internalBuffer[row][column])
                    {
                        bracketsStack.push(internalBuffer[row][column]);
                        continue;
                    }
                    else if (utils::helpers::Lambda::isOpenBracket(internalBuffer[row][column]))
                    {
                        if (closedToOpenBracket[bracketsStack.top()] == internalBuffer[row][column])
                        {
                            bracketsStack.pop();
                        }
                    }
                    if (bracketsStack.empty())
                    {
                        highlightedBrackets.insert(pair(currentPos.first, mapScreenBufferColumnToDecoratedColumnStart(currentPos.first, currentPos.second, isSecondaryHexMainWindow)));
                        highlightedBrackets.insert(pair(row, mapScreenBufferColumnToDecoratedColumnStart(row, column, isSecondaryHexMainWindow)));
                        return true;
                    }
                }
                endColumn = -1;
            }
        }
        return false;
    }

    void ScreenBuffer::unhighlightBrackets() const
    {
        highlightedBrackets.clear();
    }

    void ScreenBuffer::decorateBuffer(ScreenBuffer const &otherBuffer, bool isSecondaryHexModeMainWindow)
    {
        for (size_t row = 0UL; row < otherBuffer.getFilledRows(); ++row)
        {
            for (size_t column = 0UL, offset = 0UL; column < otherBuffer[row].size(); ++column)
            {
                if (utils::helpers::Lambda::isControlOrUnhandledChar(otherBuffer[row][column], true) || utils::helpers::Lambda::isRightToLeftUtf8(otherBuffer[row][column], true) || utils::helpers::Lambda::isNonSpacingUtf8(otherBuffer[row][column]) || configuration->isHexMode())
                {
                    if (isSecondaryHexModeMainWindow)
                    {
                        internalBuffer[row].push_back(otherBuffer[row][column] > 33 && otherBuffer[row][column] < 127 ? otherBuffer[row][column] : U' ');
                        internalBufferAsString.push_back(internalBuffer[row].back());
                    }
                    else
                    {
                        std::string utf8AsHexString = utils::helpers::Conversion::toHexString(otherBuffer[row][column]);
                        if (configuration->isHexMode())
                        {
                            utf8AsHexString.push_back(U' ');
                        }
                        short shouldAdd = 1;
                        for (auto const &e : utf8AsHexString)
                        {
                            internalBuffer[row].push_back(e);
                            internalBufferAsString.push_back(internalBuffer[row].back());
                            if (shouldAdd)
                            {
                                posMappedToHexStart.insert(pair(row, offset));
                            }
                            posMappedToHex.insert(pair(row, offset));
                            ++offset;
                            shouldAdd ^= 1;
                        }
                    }
                }
                else
                {
                    internalBuffer[row].push_back(otherBuffer[row][column]);
                    internalBufferAsString.push_back(internalBuffer[row].back());
                    ++offset;
                }
            }
        }
    }

    void ScreenBuffer::decorateAsBottomBarWindowBuffer(ScreenBuffer const &otherBuffer)
    {
        for (size_t row = 0UL; row < otherBuffer.getFilledRows(); ++row)
        {
            for (size_t column = 0UL, offset = 0UL; column < otherBuffer[row].size(); ++column)
            {
                if (utils::helpers::Lambda::isControlOrUnhandledChar(otherBuffer[row][column], false) || utils::helpers::Lambda::isRightToLeftUtf8(otherBuffer[row][column], false) || utils::helpers::Lambda::isNonSpacingUtf8(otherBuffer[row][column]))
                {
                    std::string utf8AsHexString = utils::helpers::Conversion::toHexString(otherBuffer[row][column]);
                    short shouldAdd = 1;
                    for (auto const &e : utf8AsHexString)
                    {
                        internalBuffer[row].push_back(e);
                        if (shouldAdd)
                        {
                            posMappedToHexStart.insert(pair(row, offset));
                        }
                        posMappedToHex.insert(pair(row, offset));
                        ++offset;
                        shouldAdd ^= 1;
                    }
                }
                else
                {
                    internalBuffer[row].push_back(otherBuffer[row][column]);
                    ++offset;
                }
            }
        }
    }

    bool ScreenBuffer::isBracket(pair pos) const
    {
        return highlightedBrackets.count(pos);
    }

    bool ScreenBuffer::isBufferEmpty() const
    {
        return internalBuffer.empty() || std::all_of(internalBuffer.begin(), internalBuffer.end(),
                                                     [](std::vector<char32_t> const &e)
                                                     {
                                                         return e.empty();
                                                     });
    }

    bool ScreenBuffer::isPosMappedToHex(pair pos) const
    {
        return posMappedToHex.count(pos);
    }

    bool ScreenBuffer::isSearchedTextPos(pair pos) const
    {
        return std::find_if(blinkMap.begin(), blinkMap.end(), [&](std::pair<pair, pair> const &e)
                            { return pos >= e.first && pos < e.second; }) != blinkMap.end();
    }

    void ScreenBuffer::fillBufferLine(std::u32string &&line)
    {
        std::vector<char32_t> newLine(line.begin(), line.end());
        if (line.empty())
        {
            newLine.emplace_back(U'\n');
            internalBufferAsString.push_back(U'\n');
        }
        std::copy(newLine.begin(), newLine.end(), std::back_inserter(internalBufferAsString));
        internalBuffer[lastFilledLine++].swap(newLine);
    }

    void ScreenBuffer::fillBufferLine(std::u32string const &line)
    {
        std::vector<char32_t> newLine(line.begin(), line.end());
        if (newLine.empty())
        {
            newLine.emplace_back('\n');
            internalBufferAsString.push_back(U'\n');
        }
        else
        {
            for (char32_t c : line)
            {
                internalBuffer[lastFilledLine].push_back(c);
            }
        }
        std::copy(newLine.begin(), newLine.end(), std::back_inserter(internalBufferAsString));
        ++lastFilledLine;
    }

    std::optional<char32_t> ScreenBuffer::getCharAt(pair pos) const
    {
        if (internalBuffer.size() <= pos.first || internalBuffer[pos.first].size() <= pos.second)
        {
            return std::nullopt;
        }
        return internalBuffer[pos.first][pos.second];
    }

    void ScreenBuffer::rotateLine(size_t lineNo)
    {
        if (internalBuffer[lineNo].empty())
        {
            return;
        }
        std::rotate(internalBuffer[lineNo].begin(), std::next(internalBuffer[lineNo].begin()), internalBuffer[lineNo].end());
    }

    size_t ScreenBuffer::getFilledRows() const
    {
        return lastFilledLine;
    }

    std::vector<char32_t> &ScreenBuffer::operator[](size_t idx)
    {
        return internalBuffer.at(idx);
    }

    std::vector<char32_t> const &ScreenBuffer::operator[](size_t idx) const
    {
        return internalBuffer.at(idx);
    }

    std::u32string const &ScreenBuffer::getBufferAsString() const
    {
        return internalBufferAsString;
    }

    std::set<utils::datatypes::COLOR> ScreenBuffer::getColor(size_t row, size_t column) const
    {
        if (this->highlightedText.has_value() &&
            (this->highlightedText->first.first != this->highlightedText->first.second) &&
            (this->highlightedText->first.first <= pair(row, column) && this->highlightedText->first.second > pair(row, column)))
        {
            return this->highlightedText->second;
        }
        pair pos = pair(row, column);
        // Get the color that was most recently added
        std::pair<size_t, std::set<utils::datatypes::COLOR>> lruMatch{0, {}};
        for (auto &e : colorMap)
        {
            if (e.first <= pos && e.second.first >= pos)
            {
                if (lruMatch.second.empty() || e.second.second.first > lruMatch.first)
                {
                    lruMatch = std::make_pair(e.second.second.first, e.second.second.second);
                }
            }
        }

        std::optional<std::pair<size_t, std::set<utils::datatypes::COLOR>>> highlightedTextEntry;
        std::optional<std::pair<size_t, std::set<utils::datatypes::COLOR>>> highlightedTextEntryBackground;
        for (auto &e : highlightedTextChunks)
        {
            if (e.first.first <= pair(row, column) && e.first.second > pair(row, column) && (!highlightedTextEntry.has_value() || highlightedTextEntry->first < e.second.first))
            {
                if (e.second.first != 4095)
                {
                    highlightedTextEntryBackground = e.second;
                }
                highlightedTextEntry = e.second;
            }
        }

        if (!lruMatch.second.empty())
        {
            auto fgColors = lruMatch.second;
            auto bgColors = std::set<utils::datatypes::COLOR>{};
            if (highlightedTextEntry.has_value())
            {
                bgColors = highlightedTextEntry->second;
                bgColors.erase(utils::datatypes::COLOR::FEATHER_COLOR_WHITE);
            }
            fgColors.merge(bgColors);
            return fgColors;
        }

        if (highlightedTextEntry.has_value())
        {
            if (highlightedTextEntry->first == 4095 && highlightedTextEntryBackground.has_value())
            {
                highlightedTextEntryBackground->second.erase(utils::datatypes::COLOR::FEATHER_COLOR_WHITE);
                highlightedTextEntry->second.merge(highlightedTextEntryBackground->second);
            }
            return highlightedTextEntry->second;
        }
        return lruMatch.second;
    }

    std::unordered_set<pair, pair_hash> ScreenBuffer::getHighlightedBrackets() const
    {
        return this->highlightedBrackets;
    }

    void ScreenBuffer::setHighlightedBrackets(std::unordered_set<pair, pair_hash> const &highlightedBrackets)
    {
        this->highlightedBrackets = highlightedBrackets;
    }

    void ScreenBuffer::setHighlightedTextChunks(std::multimap<std::pair<pair, pair>, std::pair<size_t, std::set<utils::datatypes::COLOR>>> const &highlightedTextChunks)
    {
        this->highlightedTextChunks = highlightedTextChunks;
    }

    void ScreenBuffer::setHighlightedText(std::pair<pair, pair> const &highlightedText, std::set<utils::datatypes::COLOR> color)
    {
        this->highlightedText = std::make_pair(highlightedText, color);
    }

    size_t ScreenBuffer::mapScreenBufferColumnToDecoratedColumnStart(size_t screenBufferRow, size_t screenBufferColumn, bool isSecondaryHexMainWindow) const
    {
        size_t decoratedColumn = 0, localScreenBufferColumn = 0;
        for (; localScreenBufferColumn < screenBufferColumn; ++localScreenBufferColumn)
        {
            decoratedColumn += configuration->isHexMode() ? (isSecondaryHexMainWindow ? 1UL : HEX_MODE_CHRACTER_WIDTH) : (utils::helpers::Lambda::isControlOrUnhandledChar(internalBuffer[screenBufferRow][localScreenBufferColumn], true) || utils::helpers::Lambda::isRightToLeftUtf8(internalBuffer[screenBufferRow][localScreenBufferColumn], true) || utils::helpers::Lambda::isNonSpacingUtf8(internalBuffer[screenBufferRow][localScreenBufferColumn])) ? utils::helpers::Conversion::toHexString(internalBuffer[screenBufferRow][localScreenBufferColumn]).size()
                                                                                                                                                                                                                                                                                                                                                                                                                                                               : 1UL;
        }
        return decoratedColumn;
    }

    size_t ScreenBuffer::mapScreenBufferColumnToDecoratedColumnEnd(size_t screenBufferRow, size_t screenBufferColumn, utils::FEATHER_MODE currFeatherMode, bool isSecondaryHexMainWindow) const
    {
        size_t decoratedColumn = 0, localScreenBufferColumn = 0;
        for (; localScreenBufferColumn < screenBufferColumn; ++localScreenBufferColumn)
        {
            decoratedColumn += configuration->isHexMode() ? (isSecondaryHexMainWindow ? 1UL : HEX_MODE_CHRACTER_WIDTH) : (utils::helpers::Lambda::isControlOrUnhandledChar(internalBuffer[screenBufferRow][localScreenBufferColumn], true) || utils::helpers::Lambda::isRightToLeftUtf8(internalBuffer[screenBufferRow][localScreenBufferColumn], true) || utils::helpers::Lambda::isNonSpacingUtf8(internalBuffer[screenBufferRow][localScreenBufferColumn])) ? utils::helpers::Conversion::toHexString(internalBuffer[screenBufferRow][localScreenBufferColumn]).size()
                                                                                                                                                                                                                                                                                                                                                                                                                                                               : 1UL;
        }
        return decoratedColumn;
    }

    ScreenBuffer::~ScreenBuffer()
    {
    }
} // namespace feather::utils
