#include "action/RemoveWordForwardAction.hpp"
#include "utils/helpers/Lambda.hpp"
#include "windows/WindowsManager.hpp"

namespace feather::action
{
    RemoveWordForwardAction::RemoveWordForwardAction(RemoveWordForwardActionInput &&input)
        : Action<RemoveWordForwardActionInput, RemoveWordForwardActionOutput>(input, RemoveWordForwardActionOutput())
    {
    }

    RemoveWordForwardActionOutput RemoveWordForwardAction::execute()
    {
        auto initialCursorRealPosition = getActionInput().cursorPos;
        auto currCursorRealPosition = initialCursorRealPosition;

        if ((utils::helpers::Lambda::isCharAtPos(initialCursorRealPosition, utils::helpers::Lambda::isNewLineChar, getActionInput().windowUUID, getActionInput().storage, getActionInput().printingOrchestrator).first))
        {
            return RemoveWordForwardActionOutput{initialCursorRealPosition};
        }
        else if (!getActionInput().printingOrchestrator->isAnyChangesMade(getActionInput().windowUUID) && !getActionInput().storage->getSize())
        {
            return RemoveWordForwardActionOutput{initialCursorRealPosition};
        }
        currCursorRealPosition = utils::helpers::Lambda::findNext(currCursorRealPosition, utils::helpers::Lambda::characterDontBelongToWord, getActionInput().printingOrchestrator, getActionInput().windowUUID, getActionInput().storage, getActionInput().windowsManager, false, true).first;
        currCursorRealPosition = utils::helpers::Lambda::findNext(currCursorRealPosition, utils::helpers::Lambda::isNotWhiteChar, getActionInput().printingOrchestrator, getActionInput().windowUUID, getActionInput().storage, getActionInput().windowsManager, false, false).first;

        if (currCursorRealPosition != initialCursorRealPosition)
        {
            return RemoveWordForwardActionOutput{getActionInput().printingOrchestrator->removeCharacters(getActionInput().windowUUID, getActionInput().storage, initialCursorRealPosition, currCursorRealPosition)};
        }

        auto firstBelongToWord = utils::helpers::Lambda::findNext(currCursorRealPosition, utils::helpers::Lambda::characterBelongToWord, getActionInput().printingOrchestrator, getActionInput().windowUUID, getActionInput().storage, getActionInput().windowsManager, false, false).first;
        auto firstNewLine = utils::helpers::Lambda::findNext(currCursorRealPosition, utils::helpers::Lambda::isNewLineChar, getActionInput().printingOrchestrator, getActionInput().windowUUID, getActionInput().storage, getActionInput().windowsManager, false, false).first;
        currCursorRealPosition = std::min(firstBelongToWord, firstNewLine);
        return RemoveWordForwardActionOutput{utils::helpers::Lambda::alignToProperPosition(getActionInput().printingOrchestrator->removeCharacters(getActionInput().windowUUID, getActionInput().storage, initialCursorRealPosition, currCursorRealPosition), getActionInput().storage, getActionInput().printingOrchestrator, getActionInput().windowUUID)};
    }
} // namespace feather::action