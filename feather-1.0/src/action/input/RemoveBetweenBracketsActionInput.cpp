#include "action/input/RemoveBetweenBracketsActionInput.hpp"

namespace feather::action
{
    RemoveBetweenBracketsActionInput::RemoveBetweenBracketsActionInput(utils::datatypes::Uuid const &windowUUIDArg, std::shared_ptr<windows::WindowsManager> windowsManagerArg, pair cursorPosArg, char32_t openingBracketArg, std::shared_ptr<utils::storage::AbstractStorage> storageArg, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestartorArg)
        : windowUUID(windowUUIDArg), windowsManager(windowsManagerArg), cursorPos(cursorPosArg), openingBracket(openingBracketArg), storage(storageArg), printingOrchestrator(printingOrchestartorArg)
    {
    }
} // namespace feather::action