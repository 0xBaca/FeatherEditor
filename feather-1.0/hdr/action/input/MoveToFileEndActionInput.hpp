#pragma once

#include "windows/MainWindowInterface.hpp"

namespace feather::action
{
    struct MoveToFileEndActionInput
    {
        std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator;
        std::shared_ptr<windows::MainWindowInterface> window;
        MoveToFileEndActionInput(std::shared_ptr<windows::MainWindowInterface>, std::shared_ptr<feather::printer::PrintingOrchestratorInterface>);
    };
} // namespace feather::action
