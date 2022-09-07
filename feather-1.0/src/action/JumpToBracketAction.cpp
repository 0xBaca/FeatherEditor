#include "action/JumpToBracketAction.hpp"
#include "utils/helpers/Lambda.hpp"

namespace feather::action
{
    JumpToBracketAction::JumpToBracketAction(JumpToBracketActionInput &&input)
        : Action<JumpToBracketActionInput, JumpToBracketActionOutput>(input, JumpToBracketActionOutput())
    {
    }

    JumpToBracketActionOutput JumpToBracketAction::execute()
    {
        std::stack<char32_t> bracketsStack;
        auto initialCursorPosition = getActionInput().cursorPos;
        auto isCharacterBracket = utils::helpers::Lambda::isCharAtPos(initialCursorPosition, utils::helpers::Lambda::isBracket, getActionInput().windowUUID, getActionInput().storage, getActionInput().printingOrchestrator);
        if (!isCharacterBracket.first ||
            (utils::helpers::Lambda::isOpenBracket(isCharacterBracket.second.value()) && utils::helpers::Lambda::isOpenBracket(getActionInput().searchedBracket)) ||
            (utils::helpers::Lambda::isClosingBracket(isCharacterBracket.second.value()) && utils::helpers::Lambda::isClosingBracket(getActionInput().searchedBracket)) ||
            (utils::helpers::Lambda::isOpenBracket(isCharacterBracket.second.value()) && (utils::helpers::Lambda::openToCloseBracket[isCharacterBracket.second.value()] != getActionInput().searchedBracket)) ||
            (utils::helpers::Lambda::isClosingBracket(isCharacterBracket.second.value()) && (utils::helpers::Lambda::closedToOpenBracket[isCharacterBracket.second.value()] != getActionInput().searchedBracket)))
        {
            return JumpToBracketActionOutput{std::nullopt};
        }

        bool searchClosingBracket = utils::helpers::Lambda::isOpenBracket(isCharacterBracket.second.value());
        bracketsStack.push(isCharacterBracket.second.value());

        if (searchClosingBracket)
        {
            auto isOpenBracket = [&](char32_t c)
            {
                return c == utils::helpers::Lambda::closedToOpenBracket[getActionInput().searchedBracket];
            };
            auto isClosingBracket = [&](char32_t c)
            {
                return c == utils::helpers::Lambda::openToCloseBracket[utils::helpers::Lambda::closedToOpenBracket[getActionInput().searchedBracket]];
            };
            auto nextOpeningBracket = utils::helpers::Lambda::findNext(initialCursorPosition, isOpenBracket, getActionInput().printingOrchestrator, getActionInput().windowUUID, getActionInput().storage, getActionInput().windowsManager, true, true);
            auto nextClosingBracket = utils::helpers::Lambda::findNext(initialCursorPosition, isClosingBracket, getActionInput().printingOrchestrator, getActionInput().windowUUID, getActionInput().storage, getActionInput().windowsManager, true, false);
            while (true)
            {
                if (nextOpeningBracket.second.has_value() && nextClosingBracket.second.has_value())
                {
                    if (nextOpeningBracket.first < nextClosingBracket.first)
                    {
                        bracketsStack.push(nextOpeningBracket.second.value());
                        nextOpeningBracket = utils::helpers::Lambda::findNext(nextOpeningBracket.first, isOpenBracket, getActionInput().printingOrchestrator, getActionInput().windowUUID, getActionInput().storage, getActionInput().windowsManager, true, false);
                    }
                    else
                    {
                        if (utils::helpers::Lambda::closedToOpenBracket[nextClosingBracket.second.value()] == bracketsStack.top())
                        {
                            bracketsStack.pop();
                        }
                        if (bracketsStack.empty())
                        {
                            return JumpToBracketActionOutput{nextClosingBracket.first};
                        }
                        nextClosingBracket = utils::helpers::Lambda::findNext(nextClosingBracket.first, isClosingBracket, getActionInput().printingOrchestrator, getActionInput().windowUUID, getActionInput().storage, getActionInput().windowsManager, true, false);
                    }
                }
                else if (nextClosingBracket.second.has_value())
                {
                    if (utils::helpers::Lambda::closedToOpenBracket[nextClosingBracket.second.value()] == bracketsStack.top())
                    {
                        bracketsStack.pop();
                    }
                    if (bracketsStack.empty())
                    {
                        return JumpToBracketActionOutput{nextClosingBracket.first};
                    }
                    nextClosingBracket = utils::helpers::Lambda::findNext(nextClosingBracket.first, isClosingBracket, getActionInput().printingOrchestrator, getActionInput().windowUUID, getActionInput().storage, getActionInput().windowsManager, true, false);
                }
                else
                {
                    return JumpToBracketActionOutput{std::nullopt};
                }
            }
            return JumpToBracketActionOutput{std::nullopt};
        }
        else
        {
            auto isOpenBracket = [&](char32_t c)
            {
                return c == getActionInput().searchedBracket;
            };
            auto isClosingBracket = [&](char32_t c)
            {
                return c == utils::helpers::Lambda::openToCloseBracket[getActionInput().searchedBracket];
            };
            auto prevOpeningBracket = utils::helpers::Lambda::findPrevious(initialCursorPosition, isOpenBracket, getActionInput().printingOrchestrator, getActionInput().windowUUID, getActionInput().storage, getActionInput().windowsManager, true);
            auto prevClosingBracket = utils::helpers::Lambda::findPrevious(initialCursorPosition, isClosingBracket, getActionInput().printingOrchestrator, getActionInput().windowUUID, getActionInput().storage, getActionInput().windowsManager, false);
            while (true)
            {
                if (prevOpeningBracket.second.has_value() && prevClosingBracket.second.has_value())
                {
                    if (prevClosingBracket.first > prevOpeningBracket.first)
                    {
                        bracketsStack.push(prevClosingBracket.second.value());
                        prevClosingBracket = utils::helpers::Lambda::findPrevious(prevClosingBracket.first, isClosingBracket, getActionInput().printingOrchestrator, getActionInput().windowUUID, getActionInput().storage, getActionInput().windowsManager, false);
                    }
                    else
                    {
                        if (utils::helpers::Lambda::openToCloseBracket[prevOpeningBracket.second.value()] == bracketsStack.top())
                        {
                            bracketsStack.pop();
                        }
                        if (bracketsStack.empty())
                        {
                            return JumpToBracketActionOutput{prevOpeningBracket.first};
                        }
                        prevOpeningBracket = utils::helpers::Lambda::findPrevious(prevOpeningBracket.first, isOpenBracket, getActionInput().printingOrchestrator, getActionInput().windowUUID, getActionInput().storage, getActionInput().windowsManager, false);
                    }
                }
                else if (prevOpeningBracket.second.has_value())
                {
                    if (utils::helpers::Lambda::openToCloseBracket[prevOpeningBracket.second.value()] == bracketsStack.top())
                    {
                        bracketsStack.pop();
                    }
                    if (bracketsStack.empty())
                    {
                        return JumpToBracketActionOutput{prevOpeningBracket.first};
                    }
                    prevOpeningBracket = utils::helpers::Lambda::findPrevious(prevOpeningBracket.first, isOpenBracket, getActionInput().printingOrchestrator, getActionInput().windowUUID, getActionInput().storage, getActionInput().windowsManager, false);
                }
                else
                {
                    return JumpToBracketActionOutput{std::nullopt};
                }
            }
            return JumpToBracketActionOutput{std::nullopt};
        }
    }
} // namespace feather::action