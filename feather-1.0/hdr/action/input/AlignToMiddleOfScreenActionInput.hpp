#pragma once

#include "windows/MainWindowInterface.hpp"
#include "windows/WindowsManager.hpp"

namespace feather::action
{
    struct AlignToMiddleOfScreenActionInput
    {
        std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator;
        std::shared_ptr<windows::MainWindowInterface> window;
        std::shared_ptr<windows::WindowsManager> windowsManager;
        std::unique_ptr<printer::PrintingVisitorInterface> &lineUpVisitor, &lineDownVisitor;
        AlignToMiddleOfScreenActionInput(std::shared_ptr<windows::MainWindowInterface>, std::shared_ptr<feather::printer::PrintingOrchestratorInterface>, std::shared_ptr<windows::WindowsManager>, std::unique_ptr<printer::PrintingVisitorInterface> &, std::unique_ptr<printer::PrintingVisitorInterface> &);
    };
} // namespace feather::action
