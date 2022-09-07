#include "action/MoveToNextWordAction.hpp"
#include "config/Config.hpp"
#include "printer/PrintingOrchestratorInterface.hpp"
#include "utils/helpers/Lambda.hpp"

extern feather::utils::FEATHER_MODE currentFeatherMode;
extern std::unique_ptr<const feather::config::Configuration> configuration;

namespace feather::action
{
    MoveToNextWordAction::MoveToNextWordAction(MoveToNextWordActionInput &&input)
        : Action<MoveToNextWordActionInput, MoveToNextWordActionOutput>(input, MoveToNextWordActionOutput())
    {
    }

    MoveToNextWordActionOutput MoveToNextWordAction::execute()
    {
        auto cursorPosition = getActionInput().cursorPos;
        auto lastValidPosition = getActionInput().printingOrchestrator->getLastValidPosition(getActionInput().windowUUID, getActionInput().storage);
        bool isLastValidCharacterNewLine = utils::helpers::Lambda::isCharAtPos(lastValidPosition, utils::helpers::Lambda::isNewLineChar, getActionInput().windowUUID, getActionInput().storage, getActionInput().printingOrchestrator).first;
        // Skip white characters
        pair movedCursorPosition = utils::helpers::Lambda::findNext(cursorPosition, utils::helpers::Lambda::characterIsNotWhiteCharacter, getActionInput().printingOrchestrator, getActionInput().windowUUID, getActionInput().storage, getActionInput().windowsManager, false, true).first;
        if (cursorPosition >= lastValidPosition)
        {
            return (utils::FEATHER_MODE::READ_MODE_SELECT_TEXT == currentFeatherMode) ? MoveToNextWordActionOutput{cursorPosition} : MoveToNextWordActionOutput{getActionInput().printingOrchestrator->getPreviousVirtualPosition(getActionInput().windowUUID, getActionInput().storage, cursorPosition)};
        }
        else if (movedCursorPosition != cursorPosition)
        {
            return MoveToNextWordActionOutput{movedCursorPosition};
        }
        cursorPosition = movedCursorPosition;

        // Skip what don't belong to word
        if ((utils::helpers::Lambda::isCharAtPos(cursorPosition, utils::helpers::Lambda::characterBelongToWord, getActionInput().windowUUID, getActionInput().storage, getActionInput().printingOrchestrator).first))
        {
            cursorPosition = utils::helpers::Lambda::findNext(cursorPosition, utils::helpers::Lambda::characterDontBelongToWord, getActionInput().printingOrchestrator, getActionInput().windowUUID, getActionInput().storage, getActionInput().windowsManager, false, false).first;
        }
        else
        {
            // Skip characters belonging to word
            cursorPosition = utils::helpers::Lambda::findNext(cursorPosition, utils::helpers::Lambda::characterBelongToWord, getActionInput().printingOrchestrator, getActionInput().windowUUID, getActionInput().storage, getActionInput().windowsManager, false, false).first;
        }

        cursorPosition = utils::helpers::Lambda::findNext(cursorPosition, utils::helpers::Lambda::characterIsNotWhiteCharacter, getActionInput().printingOrchestrator, getActionInput().windowUUID, getActionInput().storage, getActionInput().windowsManager, false, false).first;
        if (cursorPosition >= lastValidPosition)
        {
            return MoveToNextWordActionOutput{utils::helpers::Lambda::getLastValidPosInReadMode(lastValidPosition, isLastValidCharacterNewLine, configuration->isHexMode(), currentFeatherMode, getActionInput().windowUUID, getActionInput().printingOrchestrator, getActionInput().storage)};
        }
        return MoveToNextWordActionOutput{cursorPosition};
    }
} // namespace feather::action