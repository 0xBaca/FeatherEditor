#include "action/RemoveWordBackwardAction.hpp"
#include "utils/helpers/Lambda.hpp"
#include "windows/WindowsManager.hpp"

namespace feather::action
{
    RemoveWordBackwardAction::RemoveWordBackwardAction(RemoveWordBackwardActionInput &&input)
        : Action<RemoveWordBackwardActionInput, RemoveWordBackwardActionOutput>(input, RemoveWordBackwardActionOutput())
    {
    }

    RemoveWordBackwardActionOutput RemoveWordBackwardAction::execute()
    {
        auto const initialCursorRealPosition = getActionInput().cursorPos;
        auto currCursorRealPosition = initialCursorRealPosition;
        bool cursorWasBehindLastValidPos = false;
        if (currCursorRealPosition == getActionInput().printingOrchestrator->getFirstValidPosition(getActionInput().windowUUID))
        {
            return RemoveWordBackwardActionOutput{initialCursorRealPosition};
        }

        currCursorRealPosition = getActionInput().printingOrchestrator->getPreviousVirtualPosition(getActionInput().windowUUID, getActionInput().storage, currCursorRealPosition);
        if (initialCursorRealPosition > getActionInput().printingOrchestrator->getLastValidPosition(getActionInput().windowUUID, getActionInput().storage))
        {
            cursorWasBehindLastValidPos = true;
        }
        if (utils::helpers::Lambda::isCharAtPos(currCursorRealPosition, utils::helpers::Lambda::characterBelongToWord, getActionInput().windowUUID, getActionInput().storage, getActionInput().printingOrchestrator).first)
        {
            auto firstNotBelongingToWord = utils::helpers::Lambda::findPrevious(currCursorRealPosition, utils::helpers::Lambda::characterDontBelongToWord, getActionInput().printingOrchestrator, getActionInput().windowUUID, getActionInput().storage, getActionInput().windowsManager, true);
            auto firstWhiteCharacter = utils::helpers::Lambda::findPrevious(currCursorRealPosition, utils::helpers::Lambda::isWhiteChar, getActionInput().printingOrchestrator, getActionInput().windowUUID, getActionInput().storage, getActionInput().windowsManager, false);
            currCursorRealPosition = std::max(firstNotBelongingToWord.first, firstWhiteCharacter.first);
            if (!utils::helpers::Lambda::isCharAtPos(currCursorRealPosition, utils::helpers::Lambda::characterBelongToWord, getActionInput().windowUUID, getActionInput().storage, getActionInput().printingOrchestrator).first)
            {
                currCursorRealPosition = getActionInput().printingOrchestrator->getNextVirtualPosition(getActionInput().windowUUID, getActionInput().storage, currCursorRealPosition);
            }
        }
        else if (utils::helpers::Lambda::isCharAtPos(currCursorRealPosition, utils::helpers::Lambda::isWhiteChar, getActionInput().windowUUID, getActionInput().storage, getActionInput().printingOrchestrator).first)
        {
            currCursorRealPosition = utils::helpers::Lambda::findPrevious(currCursorRealPosition, utils::helpers::Lambda::isNotWhiteChar, getActionInput().printingOrchestrator, getActionInput().windowUUID, getActionInput().storage, getActionInput().windowsManager, true).first;
            if (!utils::helpers::Lambda::isCharAtPos(currCursorRealPosition, utils::helpers::Lambda::isWhiteChar, getActionInput().windowUUID, getActionInput().storage, getActionInput().printingOrchestrator).first)
            {
                currCursorRealPosition = getActionInput().printingOrchestrator->getNextVirtualPosition(getActionInput().windowUUID, getActionInput().storage, currCursorRealPosition);
            }
        }
        else
        {
            auto firstBelongingToWord = utils::helpers::Lambda::findPrevious(currCursorRealPosition, utils::helpers::Lambda::characterBelongToWord, getActionInput().printingOrchestrator, getActionInput().windowUUID, getActionInput().storage, getActionInput().windowsManager, true);
            auto firstWhiteCharacter = utils::helpers::Lambda::findPrevious(currCursorRealPosition, utils::helpers::Lambda::isWhiteChar, getActionInput().printingOrchestrator, getActionInput().windowUUID, getActionInput().storage, getActionInput().windowsManager, false);
            currCursorRealPosition = std::max(firstBelongingToWord.first, firstWhiteCharacter.first);
            if (!utils::helpers::Lambda::isCharAtPos(currCursorRealPosition, utils::helpers::Lambda::characterDontBelongToWord, getActionInput().windowUUID, getActionInput().storage, getActionInput().printingOrchestrator).first)
            {
                currCursorRealPosition = getActionInput().printingOrchestrator->getNextVirtualPosition(getActionInput().windowUUID, getActionInput().storage, currCursorRealPosition);
            }
        }
        auto posToReturn = utils::helpers::Lambda::alignToProperPosition(getActionInput().printingOrchestrator->removeCharacters(getActionInput().windowUUID, getActionInput().storage, currCursorRealPosition, initialCursorRealPosition), getActionInput().storage, getActionInput().printingOrchestrator, getActionInput().windowUUID);
        if (cursorWasBehindLastValidPos && posToReturn > getActionInput().printingOrchestrator->getFirstValidPosition(getActionInput().windowUUID))
        {
            return RemoveWordBackwardActionOutput{getActionInput().printingOrchestrator->getNextVirtualPosition(getActionInput().windowUUID, getActionInput().storage, posToReturn)};
        }
        return RemoveWordBackwardActionOutput{posToReturn};
    }
} // namespace feather::action