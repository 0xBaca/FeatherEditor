#include "action/input/JumpToBracketActionInput.hpp"

namespace feather::action
{
    JumpToBracketActionInput::JumpToBracketActionInput(utils::datatypes::Uuid const &windowUUIDArg, pair cursorPosArg, std::shared_ptr<utils::storage::AbstractStorage> storageArg, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestartorArg, std::shared_ptr<windows::WindowsManager> windowsManagerArg, char32_t searchedBracketArg)
        : windowUUID(windowUUIDArg), cursorPos(cursorPosArg), storage(storageArg), printingOrchestrator(printingOrchestartorArg), windowsManager(windowsManagerArg), searchedBracket(searchedBracketArg)
    {
    }
} // namespace feather::action