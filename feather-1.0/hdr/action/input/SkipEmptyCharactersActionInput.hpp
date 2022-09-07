#pragma once

#include "windows/WindowsManager.hpp"

namespace feather::action
{
    struct SkipEmptyCharactersActionInput
    {
        std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator;
        std::shared_ptr<windows::MainWindowInterface> window;
        std::shared_ptr<windows::WindowsManager> windowsManager;
        SkipEmptyCharactersActionInput(std::shared_ptr<windows::MainWindowInterface>, std::shared_ptr<feather::printer::PrintingOrchestratorInterface>, std::shared_ptr<windows::WindowsManager>);
    };
} // namespace feather::action