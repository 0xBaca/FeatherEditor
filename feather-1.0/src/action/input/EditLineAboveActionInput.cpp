#include "action/input/EditLineAboveActionInput.hpp"

namespace feather::action
{
    EditLineAboveActionInput::EditLineAboveActionInput(utils::datatypes::Uuid const &windowUUIDArg, pair cursorPosArg, std::shared_ptr<utils::storage::AbstractStorage> storageArg, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestartorArg, reader::KeyStroke *keyStrokeArg, std::shared_ptr<windows::WindowsManager> windowsManagerArg)
        : windowUUID(windowUUIDArg), cursorPos(cursorPosArg), storage(storageArg), printingOrchestrator(printingOrchestartorArg), windowsManager(windowsManagerArg), keyStroke(keyStrokeArg)
    {
    }
} // namespace feather::action