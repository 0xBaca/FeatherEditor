#include "action/JumpToBracketAction.hpp"
#include "action/HighlightBetweenBracketsAction.hpp"
#include "config/Config.hpp"
#include "utils/helpers/Lambda.hpp"
#include "windows/WindowsManager.hpp"

extern std::unique_ptr<const feather::config::Configuration> configuration;

namespace feather::action
{
    HighlightBetweenBracketsAction::HighlightBetweenBracketsAction(HighlightBetweenBracketsActionInput &&input)
        : Action<HighlightBetweenBracketsActionInput, HighlightBetweenBracketsActionOutput>(input, HighlightBetweenBracketsActionOutput())
    {
    }

    HighlightBetweenBracketsActionOutput HighlightBetweenBracketsAction::execute()
    {
        auto previousOpeningBracket = findOpeningBracket(getActionInput().openingBracket, utils::helpers::Lambda::openToCloseBracket[getActionInput().openingBracket]);
        if (!previousOpeningBracket.second.has_value())
        {
            return HighlightBetweenBracketsActionOutput{std::nullopt};
        }
        auto closingBracket = JumpToBracketAction({getActionInput().windowUUID, previousOpeningBracket.first, getActionInput().storage, getActionInput().printingOrchestrator, getActionInput().windowsManager, utils::helpers::Lambda::openToCloseBracket[previousOpeningBracket.second.value()]}).execute();
        if (!closingBracket.newCursorPosition.has_value() || (getActionInput().cursorPos > closingBracket.newCursorPosition))
        {
            return HighlightBetweenBracketsActionOutput{std::nullopt};
        }
        return HighlightBetweenBracketsActionOutput{std::make_pair(getActionInput().printingOrchestrator->getNextVirtualPosition(getActionInput().windowUUID, getActionInput().storage, previousOpeningBracket.first), closingBracket.newCursorPosition.value())};
    }

    std::pair<pair, std::optional<char32_t>> HighlightBetweenBracketsAction::findOpeningBracket(char32_t openingBracket, char32_t closingBracket)
    {
        std::stack<char32_t> bracketsStack;
        auto prevOpeningBracket = utils::helpers::Lambda::findPrevious(
            getActionInput().cursorPos, [&](char32_t c)
            { return c == openingBracket; },
            getActionInput().printingOrchestrator, getActionInput().windowUUID, getActionInput().storage, getActionInput().windowsManager, true);
        auto prevClosingBracket = utils::helpers::Lambda::findPrevious(
            getActionInput().cursorPos, [&](char32_t c)
            { return c == closingBracket; },
            getActionInput().printingOrchestrator, getActionInput().windowUUID, getActionInput().storage, getActionInput().windowsManager, false);

        while (true)
        {
            if (!prevOpeningBracket.second.has_value())
            {
                return std::make_pair(pair(-1, -1), std::nullopt);
            }

            if (prevClosingBracket.second.has_value() && (prevOpeningBracket.first < prevClosingBracket.first))
            {
                bracketsStack.push(prevClosingBracket.second.value());
                prevClosingBracket = utils::helpers::Lambda::findPrevious(
                    prevClosingBracket.first, [&](char32_t c)
                    { return c == closingBracket; },
                    getActionInput().printingOrchestrator, getActionInput().windowUUID, getActionInput().storage, getActionInput().windowsManager, false);
                continue;
            }

            if (prevOpeningBracket.second.has_value())
            {
                if (!bracketsStack.empty() && utils::helpers::Lambda::closedToOpenBracket[bracketsStack.top()] == prevOpeningBracket.second.value())
                {
                    prevOpeningBracket = utils::helpers::Lambda::findPrevious(
                        prevOpeningBracket.first, [&](char32_t c)
                        { return c == openingBracket; },
                        getActionInput().printingOrchestrator, getActionInput().windowUUID, getActionInput().storage, getActionInput().windowsManager, false);
                    bracketsStack.pop();
                }
                else
                {
                    return prevOpeningBracket;
                }
            }
        }
        return std::make_pair(pair(-1, -1), std::nullopt);
    }
} // namespace feather::action