#include "action/CopyLineAction.hpp"
#include "utils/helpers/Lambda.hpp"

namespace feather::action
{
    CopyLineAction::CopyLineAction(CopyLineActionInput &&input)
        : Action<CopyLineActionInput, CopyLineActionOutput>(input, CopyLineActionOutput())
    {
    }

    CopyLineActionOutput CopyLineAction::execute()
    {
        auto initialCursorRealPosition = getActionInput().cursorPos;
        pair lastValidPosition = getActionInput().printingOrchestrator->getLastValidPosition(getActionInput().windowUUID, getActionInput().storage);

        if (utils::helpers::Lambda::isCharAtPos(initialCursorRealPosition, utils::helpers::Lambda::isNewLineChar, getActionInput().windowUUID, getActionInput().storage, getActionInput().printingOrchestrator).first)
        {
            return CopyLineActionOutput{std::make_pair(initialCursorRealPosition, getActionInput().printingOrchestrator->getNextVirtualPosition(getActionInput().windowUUID, getActionInput().storage, initialCursorRealPosition))};
        }
        else if (!getActionInput().printingOrchestrator->isAnyChangesMade(getActionInput().windowUUID) && !getActionInput().storage->getSize())
        {
            return CopyLineActionOutput{std::nullopt};
        }

        //LEFT side
        auto leftSideCursorRealPos = utils::helpers::Lambda::findPrevious(initialCursorRealPosition, utils::helpers::Lambda::isNewLineChar, getActionInput().printingOrchestrator, getActionInput().windowUUID, getActionInput().storage, getActionInput().windowsManager, true);
        if (leftSideCursorRealPos.second.has_value() && utils::helpers::Lambda::isNewLineChar(leftSideCursorRealPos.second.value()))
        {
            leftSideCursorRealPos.first = getActionInput().printingOrchestrator->getNextVirtualPosition(getActionInput().windowUUID, getActionInput().storage, leftSideCursorRealPos.first);
        }
        //RIGHT side
        auto rightSideCursorRealPos = utils::helpers::Lambda::findNext(initialCursorRealPosition, utils::helpers::Lambda::isNewLineChar, getActionInput().printingOrchestrator, getActionInput().windowUUID, getActionInput().storage, getActionInput().windowsManager, false, true);
        if (!rightSideCursorRealPos.second.has_value())
        {
            rightSideCursorRealPos.first = getActionInput().printingOrchestrator->getNextVirtualPosition(getActionInput().windowUUID, getActionInput().storage, lastValidPosition);
        }
        else
        {
            rightSideCursorRealPos.first = getActionInput().printingOrchestrator->getNextVirtualPosition(getActionInput().windowUUID, getActionInput().storage, rightSideCursorRealPos.first);
        }
        return CopyLineActionOutput{std::make_pair(leftSideCursorRealPos.first, rightSideCursorRealPos.first)};
    }
} // namespace feather::action
