#include "action/MoveToLineBeginAction.hpp"
#include "printer/CurrFrameVisitor.hpp"
#include "utils/helpers/Lambda.hpp"

namespace feather::action
{
    MoveToLineBeginAction::MoveToLineBeginAction(MoveToLineBeginActionInput &&input)
        : Action<MoveToLineBeginActionInput, MoveToLineBeginActionOutput>(input, MoveToLineBeginActionOutput())
    {
    }

    MoveToLineBeginActionOutput MoveToLineBeginAction::execute()
    {
        auto cursorPosition = getActionInput().cursorPos;
        if (utils::helpers::Lambda::isCharAtPos(cursorPosition, utils::helpers::Lambda::isNewLineChar, getActionInput().windowUUID, getActionInput().storage, getActionInput().printingOrchestrator).first)
        {
            return MoveToLineBeginActionOutput{cursorPosition};
        }
        auto previousNewLine = utils::helpers::Lambda::findPrevious(cursorPosition, utils::helpers::Lambda::isNewLineChar, getActionInput().printingOrchestrator, getActionInput().windowUUID, getActionInput().storage, getActionInput().windowsManager, false);
        if (previousNewLine.second.has_value() && utils::helpers::Lambda::isNewLineChar(previousNewLine.second.value()))
        {
            cursorPosition = getActionInput().printingOrchestrator->getNextVirtualPosition(getActionInput().windowUUID, getActionInput().storage, previousNewLine.first);
        }
        else
        {
            cursorPosition = previousNewLine.first;
        }
        
        return MoveToLineBeginActionOutput{cursorPosition};
    }
} // namespace feather::action