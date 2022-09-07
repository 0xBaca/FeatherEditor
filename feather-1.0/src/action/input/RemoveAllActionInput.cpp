#include "action/input/RemoveAllActionInput.hpp"

namespace feather::action
{
    RemoveAllActionInput::RemoveAllActionInput(SearchSubstringActionInput searchSubstringActionInputArg, utils::datatypes::Uuid const &windowUUIDArg, std::shared_ptr<windows::WindowsManager> windowsManagerArg, pair cursorPosArg, std::shared_ptr<utils::storage::AbstractStorage> storageArg, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestartorArg, size_t bytesToRemoveArg)
        : searchSubstringActionInput(searchSubstringActionInputArg), windowUUID(windowUUIDArg), windowsManager(windowsManagerArg), cursorPos(cursorPosArg), storage(storageArg), printingOrchestrator(printingOrchestartorArg), bytesToRemove(bytesToRemoveArg)
    {
    }
} // namespace feather::action