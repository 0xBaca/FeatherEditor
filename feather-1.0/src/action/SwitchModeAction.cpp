#include "action/SwitchModeAction.hpp"
#include "printer/PrintingOrchestratorInterface.hpp"
#include "utils/helpers/Lambda.hpp"
#include "utils/Utf8Util.hpp"

extern std::unique_ptr<const feather::config::Configuration> configuration;

namespace feather::action
{
SwitchModeAction::SwitchModeAction(SwitchModeActionInput &&input)
    : Action<SwitchModeActionInput, SwitchModeActionOutput>(input, SwitchModeActionOutput())
{
}

SwitchModeActionOutput SwitchModeAction::execute()
{
    auto fileName = getActionInput().storage->getName();
    auto cursorPosition = getActionInput().cursorPosition;
    auto probe = getActionInput().printingOrchestrator->getContinousCharacters(cursorPosition, 1UL, getActionInput().storage, getActionInput().windowUUID);
    configuration->isHexMode() ? configuration->forceTextMode(fileName) : configuration->forceHexMode(fileName);
    if (!probe.first.empty() && utils::Utf8Util::isMiddleOfCharacter(probe.first[0]))
    {
        do
        {
            if (cursorPosition.second)
            {
                --cursorPosition.second;
            }
            else
            {
                --cursorPosition.first;
            }
        } while (utils::Utf8Util::isMiddleOfCharacter(getActionInput().printingOrchestrator->getContinousCharacters(cursorPosition, 1UL, getActionInput().storage, getActionInput().windowUUID).first[0]));
    }
    return SwitchModeActionOutput{cursorPosition};
}
} // namespace feather::action
