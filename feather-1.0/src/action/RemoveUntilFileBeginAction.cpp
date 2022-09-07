#include "action/RemoveUntilFileBeginAction.hpp"
#include "config/Config.hpp"
#include "utils/helpers/Lambda.hpp"
#include "windows/WindowsManager.hpp"

extern std::unique_ptr<const feather::config::Configuration> configuration;

namespace feather::action
{
    RemoveUntilFileBeginAction::RemoveUntilFileBeginAction(RemoveUntilFileBeginActionInput &&input)
        : Action<RemoveUntilFileBeginActionInput, RemoveUntilFileBeginActionOutput>(input, RemoveUntilFileBeginActionOutput())
    {
    }

    RemoveUntilFileBeginActionOutput RemoveUntilFileBeginAction::execute()
    {
        auto initialCursorPosition = getActionInput().cursorPos;
        auto firstValidPosition = getActionInput().printingOrchestrator->getFirstValidPosition(getActionInput().windowUUID);

        if (getActionInput().printingOrchestrator->isAllCharactersRemoved(getActionInput().windowUUID, getActionInput().storage->getSize()))
        {
            return RemoveUntilFileBeginActionOutput{initialCursorPosition};
        }
        else if (!getActionInput().printingOrchestrator->isAnyChangesMade(getActionInput().windowUUID) && !getActionInput().storage->getSize())
        {
            return RemoveUntilFileBeginActionOutput{initialCursorPosition};
        }
        return RemoveUntilFileBeginActionOutput{getActionInput().printingOrchestrator->removeCharacters(getActionInput().windowUUID, getActionInput().storage, firstValidPosition, initialCursorPosition)};
    }
} // namespace feather::action