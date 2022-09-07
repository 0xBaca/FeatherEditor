#include "action/input/RemoveLineActionInput.hpp"

namespace feather::action
{
    RemoveLineActionInput::RemoveLineActionInput(utils::datatypes::Uuid const &windowUUIDArg, std::shared_ptr<windows::WindowsManager> windowsManagerArg, pair cursorPosArg, size_t noLinesArg, std::shared_ptr<utils::storage::AbstractStorage> storageArg, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestartorArg)
        : windowUUID(windowUUIDArg), windowsManager(windowsManagerArg), cursorPos(cursorPosArg), noLines(noLinesArg), storage(storageArg), printingOrchestrator(printingOrchestartorArg)
    {
    }
} // namespace feather::action