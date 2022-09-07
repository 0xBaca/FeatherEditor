#include "action/MoveToLineEndAction.hpp"
#include "config/Config.hpp"
#include "utils/FeatherMode.hpp"
#include "utils/helpers/Lambda.hpp"

extern feather::utils::FEATHER_MODE currentFeatherMode;
extern std::unique_ptr<const feather::config::Configuration> configuration;

namespace feather::action
{
    MoveToLineEndAction::MoveToLineEndAction(MoveToLineEndActionInput &&input)
        : Action<MoveToLineEndActionInput, MoveToLineEndActionOutput>(input, MoveToLineEndActionOutput())
    {
    }

    MoveToLineEndActionOutput MoveToLineEndAction::execute()
    {
        auto cursorPosition = getActionInput().cursorPos;
        auto lastValidPosition = getActionInput().printingOrchestrator->getLastValidPosition(getActionInput().windowUUID, getActionInput().storage);
        if (utils::helpers::Lambda::isCharAtPos(cursorPosition, utils::helpers::Lambda::isNewLineChar, getActionInput().windowUUID, getActionInput().storage, getActionInput().printingOrchestrator).first || cursorPosition == lastValidPosition)
        {
            return MoveToLineEndActionOutput{cursorPosition};
        }
        cursorPosition = utils::helpers::Lambda::findNext(cursorPosition, utils::helpers::Lambda::isNewLineChar, getActionInput().printingOrchestrator, getActionInput().windowUUID, getActionInput().storage, getActionInput().windowsManager, false, true).first;
        return (utils::FEATHER_MODE::READ_MODE_SELECT_TEXT == currentFeatherMode) ? MoveToLineEndActionOutput{cursorPosition} : MoveToLineEndActionOutput{getActionInput().printingOrchestrator->getPreviousVirtualPosition(getActionInput().windowUUID, getActionInput().storage, cursorPosition)};
    }
} // namespace feather::action