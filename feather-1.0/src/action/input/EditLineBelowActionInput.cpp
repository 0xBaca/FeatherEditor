#include "action/input/EditLineBelowActionInput.hpp"

namespace feather::action
{
    EditLineBelowActionInput::EditLineBelowActionInput(utils::datatypes::Uuid const &windowUUIDArg, pair cursorPosArg, std::shared_ptr<utils::storage::AbstractStorage> storageArg, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestartorArg, reader::KeyStroke *keyStrokeArg, std::shared_ptr<windows::WindowsManager> windowsManagerArg)
        : windowUUID(windowUUIDArg), cursorPos(cursorPosArg), storage(storageArg), printingOrchestrator(printingOrchestartorArg), keyStroke(keyStrokeArg), windowsManager(windowsManagerArg)
    {
    }
} // namespace feather::action