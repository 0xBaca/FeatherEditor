#include "action/input/MoveToByteActionInput.hpp"

namespace feather::action
{
    MoveToByteActionInput::MoveToByteActionInput(size_t byteMoveToArg, utils::datatypes::Uuid const &windowUUIDArg, std::shared_ptr<utils::storage::AbstractStorage> storageArg, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestartorArg)
        : byteMoveTo(byteMoveToArg), printingOrchestrator(printingOrchestartorArg), windowUUID(windowUUIDArg), storage(storageArg)
    {
    }
} // namespace feather::action
