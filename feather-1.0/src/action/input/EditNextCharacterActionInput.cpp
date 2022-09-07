#include "action/input/EditNextCharacterActionInput.hpp"

namespace feather::action
{
    EditNextCharacterActionInput::EditNextCharacterActionInput(utils::datatypes::Uuid const &windowUUIDArg, pair cursorPosArg, std::shared_ptr<utils::storage::AbstractStorage> storageArg, std::shared_ptr<feather::printer::PrintingOrchestratorInterface> printingOrchestartorArg)
        : windowUUID(windowUUIDArg), cursorPos(cursorPosArg), storage(storageArg), printingOrchestrator(printingOrchestartorArg)
    {
    }
} // namespace feather::action
