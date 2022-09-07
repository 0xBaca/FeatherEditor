#include "action/input/SkipEmptyCharactersActionInput.hpp"

namespace feather::action
{
    SkipEmptyCharactersActionInput::SkipEmptyCharactersActionInput(std::shared_ptr<windows::MainWindowInterface> windowArg, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestartorArg, std::shared_ptr<windows::WindowsManager> windowsManagerArg)
        : printingOrchestrator(printingOrchestartorArg), window(windowArg), windowsManager(windowsManagerArg)
    {
    }
} // namespace feather::action