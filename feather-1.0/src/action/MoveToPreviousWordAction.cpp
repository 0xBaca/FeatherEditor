#include "action/MoveToPreviousWordAction.hpp"
#include "utils/helpers/Lambda.hpp"

namespace feather::action
{
    MoveToPreviousWordAction::MoveToPreviousWordAction(MoveToPreviousWordActionInput &&input)
        : Action<MoveToPreviousWordActionInput, MoveToPreviousWordActionOutput>(input, MoveToPreviousWordActionOutput())
    {
    }

    MoveToPreviousWordActionOutput MoveToPreviousWordAction::execute()
    {
        auto initialCursorPosition = getActionInput().window->getCursorRealPosition(getActionInput().printingOrchestrator.get());
        auto cursorPosition = initialCursorPosition;
        if (utils::helpers::Lambda::isCharAtPos(cursorPosition, utils::helpers::Lambda::isSpace, getActionInput().window->getUUID(), getActionInput().window->getStorage(), getActionInput().printingOrchestrator).first ||
            utils::helpers::Lambda::isCharAtPos(cursorPosition, utils::helpers::Lambda::isTabChar, getActionInput().window->getUUID(), getActionInput().window->getStorage(), getActionInput().printingOrchestrator).first)
        {
            return MoveToPreviousWordActionOutput{utils::helpers::Lambda::findPrevious(cursorPosition, utils::helpers::Lambda::characterIsNotWhiteCharacter, getActionInput().printingOrchestrator, getActionInput().window->getUUID(), getActionInput().window->getStorage(), getActionInput().windowsManager, true).first};
        }

        if (utils::helpers::Lambda::isCharAtPos(cursorPosition, utils::helpers::Lambda::characterBelongToWord, getActionInput().window->getUUID(), getActionInput().window->getStorage(), getActionInput().printingOrchestrator).first)
        {
            return MoveToPreviousWordActionOutput{utils::helpers::Lambda::findPrevious(cursorPosition, utils::helpers::Lambda::characterDontBelongToWord, getActionInput().printingOrchestrator, getActionInput().window->getUUID(), getActionInput().window->getStorage(), getActionInput().windowsManager, true).first};
        }
        return MoveToPreviousWordActionOutput{utils::helpers::Lambda::findPrevious(cursorPosition, utils::helpers::Lambda::characterBelongToWord, getActionInput().printingOrchestrator, getActionInput().window->getUUID(), getActionInput().window->getStorage(), getActionInput().windowsManager, true).first};
    }
} // namespace feather::action