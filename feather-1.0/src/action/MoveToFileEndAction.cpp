#include "action/MoveToFileEndAction.hpp"
#include "config/Config.hpp"
#include "printer/PrintingOrchestratorInterface.hpp"
#include "utils/FeatherMode.hpp"
#include "utils/helpers/Lambda.hpp"

extern feather::utils::FEATHER_MODE currentFeatherMode;
extern std::unique_ptr<const feather::config::Configuration> configuration;

namespace feather::action
{
    MoveToFileEndAction::MoveToFileEndAction(MoveToFileEndActionInput &&input)
        : Action<MoveToFileEndActionInput, MoveToFileEndActionOutput>(input, MoveToFileEndActionOutput())
    {
    }

    MoveToFileEndActionOutput MoveToFileEndAction::execute()
    {
        auto lastValidPosition = getActionInput().printingOrchestrator->getLastValidPosition(getActionInput().window->getUUID(), getActionInput().window->getStorage());
        auto nextPosition = getActionInput().printingOrchestrator->getNextVirtualPosition(getActionInput().window->getUUID(), getActionInput().window->getStorage(), lastValidPosition);
        auto prevPosition = getActionInput().printingOrchestrator->getPreviousVirtualPosition(getActionInput().window->getUUID(), getActionInput().window->getStorage(), lastValidPosition);
        bool isLastValidCharacterNewLine = utils::helpers::Lambda::isCharAtPos(lastValidPosition, utils::helpers::Lambda::isNewLineChar, getActionInput().window->getUUID(), getActionInput().window->getStorage(), getActionInput().printingOrchestrator).first;
        if (!configuration->isHexMode() && isLastValidCharacterNewLine)
        {
            return (utils::FEATHER_MODE::READ_MODE_SELECT_TEXT == currentFeatherMode) ? MoveToFileEndActionOutput{lastValidPosition} : MoveToFileEndActionOutput{prevPosition};
        }
        return (utils::FEATHER_MODE::READ_MODE_SELECT_TEXT == currentFeatherMode) ? MoveToFileEndActionOutput{nextPosition} : MoveToFileEndActionOutput{lastValidPosition};
    }
} // namespace feather::action