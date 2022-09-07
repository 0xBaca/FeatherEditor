#pragma once

#include "printer/PrintingOrchestrator.hpp"

namespace feather::action
{
    struct RemoveUntilLineEndActionInput
    {
        utils::datatypes::Uuid const &windowUUID;
        std::shared_ptr<windows::WindowsManager> windowsManager;
        pair cursorPos;
        std::shared_ptr<utils::storage::AbstractStorage> storage;
        std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator;
        RemoveUntilLineEndActionInput(utils::datatypes::Uuid const &, std::shared_ptr<windows::WindowsManager>, pair, std::shared_ptr<utils::storage::AbstractStorage>, std::shared_ptr<feather::printer::PrintingOrchestratorInterface>);
    };
} // namespace feather::action