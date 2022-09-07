#pragma once

#include "printer/PrintingOrchestratorInterface.hpp"
#include "windows/WindowsManager.hpp"

namespace feather::action
{
    struct ShiftRightActionInput
    {
        std::pair<pair, pair> range;
        pair currCursorPos;
        std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator;
        utils::datatypes::Uuid const &windowUUID;
        std::shared_ptr<utils::storage::AbstractStorage> storage;
        std::shared_ptr<windows::WindowsManager> windowsManager;
        std::unique_ptr<printer::PrintingVisitorInterface> &currFrameVisitor;
        ShiftRightActionInput(std::pair<pair, pair>, pair, utils::datatypes::Uuid const &, std::shared_ptr<utils::storage::AbstractStorage>, std::shared_ptr<feather::printer::PrintingOrchestratorInterface>, std::shared_ptr<windows::WindowsManager>, std::unique_ptr<printer::PrintingVisitorInterface> &);
    };
} // namespace feather::action