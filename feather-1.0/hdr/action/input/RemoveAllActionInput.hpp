#pragma once

#include "action/input/SearchSubstringActionInput.hpp"

namespace feather::action
{
    struct RemoveAllActionInput
    {
        SearchSubstringActionInput searchSubstringActionInput;
        utils::datatypes::Uuid const &windowUUID;
        std::shared_ptr<windows::WindowsManager> windowsManager;
        pair cursorPos;
        std::shared_ptr<utils::storage::AbstractStorage> storage;
        std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator;
        size_t bytesToRemove;
        RemoveAllActionInput(SearchSubstringActionInput, utils::datatypes::Uuid const &, std::shared_ptr<windows::WindowsManager>, pair, std::shared_ptr<utils::storage::AbstractStorage>, std::shared_ptr<feather::printer::PrintingOrchestratorInterface>, size_t);
    };
} // namespace feather::action