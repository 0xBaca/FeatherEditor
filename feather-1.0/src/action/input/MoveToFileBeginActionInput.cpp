#include "action/input/MoveToFileBeginActionInput.hpp"

namespace feather::action
{
    MoveToFileBeginActionInput::MoveToFileBeginActionInput(std::shared_ptr<windows::MainWindowInterface> windowArg, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestartorArg)
        : printingOrchestrator(printingOrchestartorArg), window(windowArg)
    {
    }
} // namespace feather::action
