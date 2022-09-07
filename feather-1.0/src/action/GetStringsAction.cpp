#include "action/GetStringsAction.hpp"
#include "utils/helpers/Lambda.hpp"

extern feather::utils::FEATHER_MODE currentFeatherMode;

namespace feather::action
{
    GetStringsAction::GetStringsAction(GetStringsActionInput &&input)
        : Action<GetStringsActionInput, GetStringsActionOutput>(input, GetStringsActionOutput())
    {
    }

    GetStringsActionOutput GetStringsAction::execute()
    {
        auto const &screenBuffer = getActionInput().window->getBufferFiller()->getScreenBuffer();
        std::vector<std::pair<pair, pair>> toReturn;
        std::optional<pair> currStart = std::nullopt;
        bool lastBracket = false;
        bool firstRow = true;
        size_t row = 0, col = 0;
        for (row = 0; row < screenBuffer.getFilledRows(); ++row)
        {
            for (col = 0; col < screenBuffer[row].size(); ++col)
            {
                if (screenBuffer[row][col] == U'"' && ((firstRow && !col) || screenBuffer[row][col - 1] != U'\\') && !lastBracket)
                {
                    if (!currStart.has_value())
                    {
                        currStart = std::make_optional(pair(row, col));
                    }
                    else
                    {
                        lastBracket = true;
                        continue;
                    }
                }
                else if (lastBracket)
                {
                    toReturn.push_back(std::make_pair(currStart.value(), pair(row, col)));
                    currStart = std::nullopt;
                    lastBracket = false;
                    col -= 1;
                    continue;
                }

                if (utils::helpers::Lambda::isNewLineChar(screenBuffer[row][col]))
                {
                    currStart = std::nullopt;
                    lastBracket = false;
                    continue;
                }
            }
            firstRow = false;
        }
        if (lastBracket)
        {
            toReturn.push_back(std::make_pair(currStart.value(), pair(row - 1, col)));
        }
        updateResult(toReturn);
        return GetStringsActionOutput{toReturn};
    }

    void GetStringsAction::updateResult(std::vector<std::pair<pair, pair>> &result)
    {
        for (auto &e : result)
        {
            e.first.second = getActionInput().window->getBufferFiller()->getScreenBuffer().mapScreenBufferColumnToDecoratedColumnStart(e.first.first, e.first.second, getActionInput().window->isSecondaryHexModeMainWindow());
            e.second.second = getActionInput().window->getBufferFiller()->getScreenBuffer().mapScreenBufferColumnToDecoratedColumnEnd(e.second.first, e.second.second, currentFeatherMode, getActionInput().window->isSecondaryHexModeMainWindow());
        }
    }
} // namespace feather::action