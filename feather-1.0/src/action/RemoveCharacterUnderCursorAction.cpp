#include "action/RemoveCharacterUnderCursorAction.hpp"
#include "utils/helpers/Lambda.hpp"

extern std::unique_ptr<const feather::config::Configuration> configuration;

namespace feather::action
{
    RemoveCharacterUnderCursorAction::RemoveCharacterUnderCursorAction(RemoveCharacterUnderCursorActionInput &&input)
        : Action<RemoveCharacterUnderCursorActionInput, RemoveCharacterUnderCursorActionOutput>(input, RemoveCharacterUnderCursorActionOutput())
    {
    }

    RemoveCharacterUnderCursorActionOutput RemoveCharacterUnderCursorAction::execute()
    {
        auto initialCursorRealPosition = getActionInput().cursorPos;
        if (getActionInput().printingOrchestrator->isAllCharactersRemoved(getActionInput().windowUUID, getActionInput().storage->getSize()) || (!getActionInput().printingOrchestrator->isAnyChangesMade(getActionInput().windowUUID) && !getActionInput().storage->getSize()))
        {
            return RemoveCharacterUnderCursorActionOutput{initialCursorRealPosition};
        }
        else if (!configuration->isHexMode() && utils::helpers::Lambda::isCharAtPos(initialCursorRealPosition, utils::helpers::Lambda::isNewLineChar, getActionInput().windowUUID, getActionInput().storage, getActionInput().printingOrchestrator).first)
        {
            return RemoveCharacterUnderCursorActionOutput{initialCursorRealPosition};
        }

        pair nextPos = getActionInput().printingOrchestrator->getNextVirtualPosition(getActionInput().windowUUID, getActionInput().storage, initialCursorRealPosition);
        return RemoveCharacterUnderCursorActionOutput{utils::helpers::Lambda::alignToProperPosition(getActionInput().printingOrchestrator->removeCharacters(getActionInput().windowUUID, getActionInput().storage, initialCursorRealPosition, nextPos), getActionInput().storage, getActionInput().printingOrchestrator, getActionInput().windowUUID)};
    }
} // namespace feather::action