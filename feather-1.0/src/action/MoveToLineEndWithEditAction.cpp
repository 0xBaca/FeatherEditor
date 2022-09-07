#include "action/MoveToLineEndWithEditAction.hpp"
#include "utils/helpers/Lambda.hpp"

namespace feather::action
{
    MoveToLineEndWithEditAction::MoveToLineEndWithEditAction(MoveToLineEndWithEditActionInput &&input)
        : Action<MoveToLineEndWithEditActionInput, MoveToLineEndWithEditActionOutput>(input, MoveToLineEndWithEditActionOutput())
    {
    }

    MoveToLineEndWithEditActionOutput MoveToLineEndWithEditAction::execute()
    {
        auto cursorPosition = getActionInput().cursorPos;
        cursorPosition = utils::helpers::Lambda::findNext(cursorPosition, utils::helpers::Lambda::isNewLineChar, getActionInput().printingOrchestrator, getActionInput().windowUUID, getActionInput().storage, getActionInput().windowsManager, false, true).first;
        return MoveToLineEndWithEditActionOutput{cursorPosition};
    }
} // namespace feather::action
