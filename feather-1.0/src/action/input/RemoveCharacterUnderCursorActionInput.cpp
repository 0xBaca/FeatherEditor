#include "action/input/RemoveCharacterUnderCursorActionInput.hpp"

namespace feather::action
{
    RemoveCharacterUnderCursorActionInput::RemoveCharacterUnderCursorActionInput(utils::datatypes::Uuid const &windowUUIDArg, pair cursorPosArg, std::shared_ptr<utils::storage::AbstractStorage> storageArg, std::shared_ptr<feather::printer::PrintingOrchestratorInterface> printingOrchestartorArg, std::shared_ptr<windows::WindowsManager> windowsManagerArg)
        : windowUUID(windowUUIDArg), cursorPos(cursorPosArg), printingOrchestrator(printingOrchestartorArg), storage(storageArg), windowsManager(windowsManagerArg)
    {
    }
} // namespace feather::action
