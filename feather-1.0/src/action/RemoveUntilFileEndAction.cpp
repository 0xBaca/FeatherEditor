#include "action/RemoveUntilFileEndAction.hpp"
#include "config/Config.hpp"
#include "utils/helpers/Lambda.hpp"
#include "windows/WindowsManager.hpp"

extern std::unique_ptr<const feather::config::Configuration> configuration;

namespace feather::action
{
    RemoveUntilFileEndAction::RemoveUntilFileEndAction(RemoveUntilFileEndActionInput &&input)
        : Action<RemoveUntilFileEndActionInput, RemoveUntilFileEndActionOutput>(input, RemoveUntilFileEndActionOutput())
    {
    }

    RemoveUntilFileEndActionOutput RemoveUntilFileEndAction::execute()
    {
        auto initialCursorPosition = getActionInput().cursorPos;
        auto lastValidPosition = getActionInput().printingOrchestrator->getLastValidPosition(getActionInput().windowUUID, getActionInput().storage);

        if (getActionInput().printingOrchestrator->isAllCharactersRemoved(getActionInput().windowUUID, getActionInput().storage->getSize()))
        {
            return RemoveUntilFileEndActionOutput{initialCursorPosition};
        }
        else if (!getActionInput().printingOrchestrator->isAnyChangesMade(getActionInput().windowUUID) && !getActionInput().storage->getSize())
        {
            return RemoveUntilFileEndActionOutput{initialCursorPosition};
        }
        auto deletionEnd = getActionInput().printingOrchestrator->getNextVirtualPosition(getActionInput().windowUUID, getActionInput().storage, lastValidPosition);
        pair newVirtualPosition = utils::helpers::Lambda::alignToProperPosition(getActionInput().printingOrchestrator->removeCharacters(getActionInput().windowUUID, getActionInput().storage, initialCursorPosition, deletionEnd),
                                                                                getActionInput().storage, getActionInput().printingOrchestrator, getActionInput().windowUUID);
        if (utils::helpers::Lambda::isCharAtPos(newVirtualPosition, utils::helpers::Lambda::isNewLineChar, getActionInput().windowUUID, getActionInput().storage, getActionInput().printingOrchestrator).first)
        {
            newVirtualPosition = getActionInput().printingOrchestrator->getPreviousVirtualPosition(getActionInput().windowUUID, getActionInput().storage, newVirtualPosition);
        }
        return RemoveUntilFileEndActionOutput{newVirtualPosition};
    }
} // namespace feather::action