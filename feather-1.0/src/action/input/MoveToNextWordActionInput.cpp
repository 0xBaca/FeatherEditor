#include "action/input/MoveToNextWordActionInput.hpp"

namespace feather::action
{
MoveToNextWordActionInput::MoveToNextWordActionInput(utils::datatypes::Uuid const &windowUUIDArg, pair cursorPosArg, std::shared_ptr<utils::storage::AbstractStorage> storageArg, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestartorArg, std::shared_ptr<windows::WindowsManager> windowsManagerArg)
    : windowUUID(windowUUIDArg), cursorPos(cursorPosArg), storage(storageArg), printingOrchestrator(printingOrchestartorArg), windowsManager(windowsManagerArg)
{
}
} // namespace feather::action