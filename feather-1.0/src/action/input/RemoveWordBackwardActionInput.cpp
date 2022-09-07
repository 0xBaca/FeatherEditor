#include "action/input/RemoveWordBackwardActionInput.hpp"

namespace feather::action
{
    RemoveWordBackwardActionInput::RemoveWordBackwardActionInput(utils::datatypes::Uuid const &windowUUIDArg, std::shared_ptr<windows::WindowsManager> windowsManagerArg, pair cursorPosArg, std::shared_ptr<utils::storage::AbstractStorage> storageArg, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestartorArg)
        : windowUUID(windowUUIDArg), cursorPos(cursorPosArg), storage(storageArg), printingOrchestrator(printingOrchestartorArg), windowsManager(windowsManagerArg)
    {
    }
} // namespace feather::action