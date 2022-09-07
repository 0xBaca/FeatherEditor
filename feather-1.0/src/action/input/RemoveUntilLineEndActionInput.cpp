#include "action/input/RemoveUntilLineEndActionInput.hpp"

namespace feather::action
{
    RemoveUntilLineEndActionInput::RemoveUntilLineEndActionInput(utils::datatypes::Uuid const &windowUUIDArg, std::shared_ptr<windows::WindowsManager> windowsManagerArg, pair cursorPosArg, std::shared_ptr<utils::storage::AbstractStorage> storageArg, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestartorArg)
        : windowUUID(windowUUIDArg), windowsManager(windowsManagerArg), cursorPos(cursorPosArg), storage(storageArg), printingOrchestrator(printingOrchestartorArg)
    {
    }
} // namespace feather::action