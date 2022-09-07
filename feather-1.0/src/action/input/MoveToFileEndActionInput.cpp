#include "action/input/MoveToFileEndActionInput.hpp"

namespace feather::action
{
    MoveToFileEndActionInput::MoveToFileEndActionInput(std::shared_ptr<windows::MainWindowInterface> windowArg, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestartorArg)
        : printingOrchestrator(printingOrchestartorArg), window(windowArg)
    {
    }
} // namespace feather::action