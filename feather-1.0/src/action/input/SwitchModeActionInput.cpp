#include "action/input/SwitchModeActionInput.hpp"

namespace feather::action
{
SwitchModeActionInput::SwitchModeActionInput(pair cursorPositionArg, utils::datatypes::Uuid const &windowUUIDArg, std::shared_ptr<utils::storage::AbstractStorage> storageArg, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestartorArg)
    : cursorPosition(cursorPositionArg), printingOrchestrator(printingOrchestartorArg), windowUUID(windowUUIDArg), storage(storageArg)
{
}
} // namespace feather::acti