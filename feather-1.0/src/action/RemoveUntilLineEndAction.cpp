#include "action/RemoveUntilLineEndAction.hpp"
#include "utils/helpers/Lambda.hpp"
#include "windows/WindowsManager.hpp"

namespace feather::action
{
    RemoveUntilLineEndAction::RemoveUntilLineEndAction(RemoveUntilLineEndActionInput &&input)
        : Action<RemoveUntilLineEndActionInput, RemoveUntilLineEndActionOutput>(input, RemoveUntilLineEndActionOutput())
    {
    }

    RemoveUntilLineEndActionOutput RemoveUntilLineEndAction::execute()
    {
        auto initialCursorPosition = getActionInput().cursorPos;
        auto newVirtualPosition = initialCursorPosition;
        if (getActionInput().printingOrchestrator->isAllCharactersRemoved(getActionInput().windowUUID, getActionInput().storage->getSize()))
        {
            return RemoveUntilLineEndActionOutput{initialCursorPosition};
        }
        else if (utils::helpers::Lambda::isCharAtPos(newVirtualPosition, utils::helpers::Lambda::isNewLineChar, getActionInput().windowUUID, getActionInput().storage, getActionInput().printingOrchestrator).first)
        {
            return RemoveUntilLineEndActionOutput{newVirtualPosition};
        }
        else if (!getActionInput().printingOrchestrator->isAnyChangesMade(getActionInput().windowUUID) && !getActionInput().storage->getSize())
        {
            return RemoveUntilLineEndActionOutput{initialCursorPosition};
        }
        newVirtualPosition = utils::helpers::Lambda::findNext(newVirtualPosition, utils::helpers::Lambda::isNewLineChar, getActionInput().printingOrchestrator, getActionInput().windowUUID, getActionInput().storage, getActionInput().windowsManager, false, true).first;
        return RemoveUntilLineEndActionOutput{utils::helpers::Lambda::alignToProperPosition(getActionInput().printingOrchestrator->removeCharacters(getActionInput().windowUUID, getActionInput().storage, initialCursorPosition, newVirtualPosition), getActionInput().storage, getActionInput().printingOrchestrator, getActionInput().windowUUID)};
    }
} // namespace feather::action