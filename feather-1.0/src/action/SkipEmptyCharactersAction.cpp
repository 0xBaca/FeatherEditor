#include "action/SkipEmptyCharactersAction.hpp"
#include "utils/helpers/Lambda.hpp"

namespace feather::action
{
    SkipEmptyCharactersAction::SkipEmptyCharactersAction(SkipEmptyCharactersActionInput &&input)
        : Action<SkipEmptyCharactersActionInput, SkipEmptyCharactersActionOutput>(input, SkipEmptyCharactersActionOutput())
    {
    }

    SkipEmptyCharactersActionOutput SkipEmptyCharactersAction::execute()
    {
        auto cursorPosition = getActionInput().window->getCursorRealPosition(getActionInput().printingOrchestrator.get());
        pair lastValidPosition = getActionInput().printingOrchestrator->getLastValidPosition(getActionInput().window->getUUID(), getActionInput().window->getStorage());
        cursorPosition = utils::helpers::Lambda::findNext(cursorPosition, utils::helpers::Lambda::isNotWhiteChar, getActionInput().printingOrchestrator, getActionInput().window->getUUID(), getActionInput().window->getStorage(), getActionInput().windowsManager, false, true).first;
        if (cursorPosition > lastValidPosition)
        {
            cursorPosition = getActionInput().printingOrchestrator->getPreviousVirtualPosition(getActionInput().window->getUUID(), getActionInput().window->getStorage(), cursorPosition);
        }
        return SkipEmptyCharactersActionOutput{cursorPosition};
    }
} // namespace feather::action