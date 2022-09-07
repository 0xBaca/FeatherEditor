#include "action/RemoveBetweenWhiteCharactersAction.hpp"
#include "utils/helpers/Lambda.hpp"

namespace feather::action
{
    RemoveBetweenWhiteCharactersAction::RemoveBetweenWhiteCharactersAction(RemoveBetweenWhiteCharactersActionInput &&input)
        : Action<RemoveBetweenWhiteCharactersActionInput, RemoveBetweenWhiteCharactersActionOutput>(input, RemoveBetweenWhiteCharactersActionOutput())
    {
    }

    RemoveBetweenWhiteCharactersActionOutput RemoveBetweenWhiteCharactersAction::execute()
    {
        auto initialCursorRealPosition = getActionInput().cursorPos;
        auto leftSideCursorRealPos = initialCursorRealPosition;
        auto rightSideCursorRealPos = initialCursorRealPosition;

        if (getActionInput().printingOrchestrator->isAllCharactersRemoved(getActionInput().windowUUID, getActionInput().storage->getSize()))
        {
            return RemoveBetweenWhiteCharactersActionOutput{initialCursorRealPosition};
        }
        else if (utils::helpers::Lambda::isCharAtPos(initialCursorRealPosition, utils::helpers::Lambda::isWhiteCharOrNewLine, getActionInput().windowUUID, getActionInput().storage, getActionInput().printingOrchestrator).first)
        {
            return RemoveBetweenWhiteCharactersActionOutput{initialCursorRealPosition};
        }
        else if (!getActionInput().printingOrchestrator->isAnyChangesMade(getActionInput().windowUUID) && !getActionInput().storage->getSize())
        {
            return RemoveBetweenWhiteCharactersActionOutput{initialCursorRealPosition};
        }

        //LEFT side
        auto previousWhiteChar = utils::helpers::Lambda::findPrevious(leftSideCursorRealPos, utils::helpers::Lambda::isWhiteCharOrNewLine, getActionInput().printingOrchestrator, getActionInput().windowUUID, getActionInput().storage, getActionInput().windowsManager, true);
        if (previousWhiteChar.second.has_value() && utils::helpers::Lambda::isWhiteCharOrNewLine(previousWhiteChar.second.value()))
        {
            leftSideCursorRealPos = getActionInput().printingOrchestrator->getNextVirtualPosition(getActionInput().windowUUID, getActionInput().storage, previousWhiteChar.first);
        }
        else
        {
            leftSideCursorRealPos = previousWhiteChar.first;
        }
        //RIGHT side
        rightSideCursorRealPos = utils::helpers::Lambda::findNext(rightSideCursorRealPos, utils::helpers::Lambda::isWhiteCharOrNewLine, getActionInput().printingOrchestrator, getActionInput().windowUUID, getActionInput().storage, getActionInput().windowsManager, false, true).first;
        //rightSideCursorRealPos = getActionInput().printingOrchestrator->getNextVirtualPosition(getActionInput().windowUUID, getActionInput().storage, rightSideCursorRealPos);
        return RemoveBetweenWhiteCharactersActionOutput{utils::helpers::Lambda::alignToProperPosition(getActionInput().printingOrchestrator->removeCharacters(getActionInput().windowUUID, getActionInput().storage, leftSideCursorRealPos, rightSideCursorRealPos, true, getActionInput().cursorPos), getActionInput().storage, getActionInput().printingOrchestrator, getActionInput().windowUUID)};
    }
} // namespace feather::action