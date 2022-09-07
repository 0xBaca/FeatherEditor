#include "action/EditNextCharacterAction.hpp"
#include "config/Config.hpp"
#include "utils/helpers/Lambda.hpp"

extern std::unique_ptr<const feather::config::Configuration> configuration;

namespace feather::action
{
    EditNextCharacterAction::EditNextCharacterAction(EditNextCharacterActionInput &&input)
        : Action<EditNextCharacterActionInput, EditNextCharacterActionOutput>(input, EditNextCharacterActionOutput())
    {
    }

    EditNextCharacterActionOutput EditNextCharacterAction::execute()
    {
        auto currCursorPosition = getActionInput().cursorPos;
        auto lastValidPosition = getActionInput().printingOrchestrator->getLastValidPosition(getActionInput().windowUUID, getActionInput().storage);
        if (!configuration->isHexMode() && utils::helpers::Lambda::isCharAtPos(currCursorPosition, utils::helpers::Lambda::isNewLineChar, getActionInput().windowUUID, getActionInput().storage, getActionInput().printingOrchestrator).first)
        {
            return EditNextCharacterActionOutput{currCursorPosition};
        }
        else if (!getActionInput().printingOrchestrator->isAnyChangesMade(getActionInput().windowUUID) && !getActionInput().storage->getSize())
        {
            return EditNextCharacterActionOutput{currCursorPosition};
        }

        auto nextCursorPosition = getActionInput().printingOrchestrator->getNextVirtualPosition(getActionInput().windowUUID, getActionInput().storage, currCursorPosition);
        if (currCursorPosition == lastValidPosition)
        {
            return EditNextCharacterActionOutput{nextCursorPosition};
        }
        return EditNextCharacterActionOutput{nextCursorPosition};
    }
} // namespace feather::action
