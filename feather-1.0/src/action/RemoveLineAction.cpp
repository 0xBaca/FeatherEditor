#include "action/RemoveLineAction.hpp"
#include "utils/helpers/Lambda.hpp"

namespace feather::action
{
    RemoveLineAction::RemoveLineAction(RemoveLineActionInput &&input)
        : Action<RemoveLineActionInput, RemoveLineActionOutput>(input, RemoveLineActionOutput())
    {
    }

    RemoveLineActionOutput RemoveLineAction::execute()
    {
        auto initialCursorRealPosition = getActionInput().cursorPos;
        size_t noLines = getActionInput().noLines;
        pair lastValidPosition = getActionInput().printingOrchestrator->getLastValidPosition(getActionInput().windowUUID, getActionInput().storage);
        auto leftSideCursorRealPos = initialCursorRealPosition;
        auto rightSideCursorRealPos = initialCursorRealPosition;

        if (getActionInput().printingOrchestrator->isAllCharactersRemoved(getActionInput().windowUUID, getActionInput().storage->getSize()))
        {
            return RemoveLineActionOutput{initialCursorRealPosition};
        }
        else if (utils::helpers::Lambda::isCharAtPos(initialCursorRealPosition, utils::helpers::Lambda::isNewLineChar, getActionInput().windowUUID, getActionInput().storage, getActionInput().printingOrchestrator).first)
        {
            auto nextPos = getActionInput().printingOrchestrator->getNextVirtualPosition(getActionInput().windowUUID, getActionInput().storage, initialCursorRealPosition);
            return RemoveLineActionOutput{utils::helpers::Lambda::alignToProperPosition(getActionInput().printingOrchestrator->removeCharacters(getActionInput().windowUUID, getActionInput().storage, leftSideCursorRealPos, nextPos), getActionInput().storage, getActionInput().printingOrchestrator, getActionInput().windowUUID)};
        }
        else if (!getActionInput().printingOrchestrator->isAnyChangesMade(getActionInput().windowUUID) && !getActionInput().storage->getSize())
        {
            return RemoveLineActionOutput{initialCursorRealPosition};
        }

        //LEFT side
        auto previousNewLine = utils::helpers::Lambda::findPrevious(leftSideCursorRealPos, utils::helpers::Lambda::isNewLineChar, getActionInput().printingOrchestrator, getActionInput().windowUUID, getActionInput().storage, getActionInput().windowsManager, true);
        if (previousNewLine.second.has_value() && utils::helpers::Lambda::isNewLineChar(previousNewLine.second.value()))
        {
            leftSideCursorRealPos = getActionInput().printingOrchestrator->getNextVirtualPosition(getActionInput().windowUUID, getActionInput().storage, previousNewLine.first);
        }
        else
        {
            leftSideCursorRealPos = previousNewLine.first;
        }
        //RIGHT side
        while (noLines--)
        {
            rightSideCursorRealPos = utils::helpers::Lambda::findNext(rightSideCursorRealPos, utils::helpers::Lambda::isNewLineChar, getActionInput().printingOrchestrator, getActionInput().windowUUID, getActionInput().storage, getActionInput().windowsManager, false, true).first;
            if (rightSideCursorRealPos > lastValidPosition)
            {
                break;
            }
            rightSideCursorRealPos = getActionInput().printingOrchestrator->getNextVirtualPosition(getActionInput().windowUUID, getActionInput().storage, rightSideCursorRealPos);
        }
        return RemoveLineActionOutput{utils::helpers::Lambda::alignToProperPosition(getActionInput().printingOrchestrator->removeCharacters(getActionInput().windowUUID, getActionInput().storage, leftSideCursorRealPos, rightSideCursorRealPos, true, getActionInput().cursorPos), getActionInput().storage, getActionInput().printingOrchestrator, getActionInput().windowUUID)};
    }
} // namespace feather::action