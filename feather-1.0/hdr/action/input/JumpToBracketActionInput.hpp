#pragma once

#include "printer/PrintingOrchestrator.hpp"

namespace feather::action
{
    struct JumpToBracketActionInput
    {
        utils::datatypes::Uuid const &windowUUID;
        pair cursorPos;
        std::shared_ptr<utils::storage::AbstractStorage> storage;
        std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator;
        std::shared_ptr<windows::WindowsManager> windowsManager;
        char32_t searchedBracket;
        JumpToBracketActionInput(utils::datatypes::Uuid const &, pair, std::shared_ptr<utils::storage::AbstractStorage>, std::shared_ptr<feather::printer::PrintingOrchestratorInterface>, std::shared_ptr<windows::WindowsManager>, char32_t);
    };
} // namespace feather::action