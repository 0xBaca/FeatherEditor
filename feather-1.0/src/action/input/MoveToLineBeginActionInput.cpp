#include "action/input/MoveToLineBeginActionInput.hpp"

namespace feather::action
{
MoveToLineBeginActionInput::MoveToLineBeginActionInput(utils::datatypes::Uuid const &windowUUIDArg, pair cursorPosArg, std::shared_ptr<utils::storage::AbstractStorage> storageArg, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestartorArg, std::shared_ptr<windows::WindowsManager> windowsManagerArg)
    : windowUUID(windowUUIDArg), cursorPos(cursorPosArg), storage(storageArg), printingOrchestrator(printingOrchestartorArg), windowsManager(windowsManagerArg)
{
}
} // namespace feather::actio