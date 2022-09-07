#pragma once

#include "printer/PrintingOrchestrator.hpp"

namespace feather::action
{
    struct RemoveBetweenBracketsActionInput
    {
        utils::datatypes::Uuid const &windowUUID;
        std::shared_ptr<windows::WindowsManager> windowsManager;
        pair cursorPos;
        char32_t openingBracket;
        std::shared_ptr<utils::storage::AbstractStorage> storage;
        std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator;
        RemoveBetweenBracketsActionInput(utils::datatypes::Uuid const &, std::shared_ptr<windows::WindowsManager>, pair, char32_t, std::shared_ptr<utils::storage::AbstractStorage>, std::shared_ptr<feather::printer::PrintingOrchestratorInterface>);
    };
} // namespace feather::action