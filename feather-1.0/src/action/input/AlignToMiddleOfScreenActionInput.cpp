#include "action/input/AlignToMiddleOfScreenActionInput.hpp"

namespace feather::action
{
    AlignToMiddleOfScreenActionInput::AlignToMiddleOfScreenActionInput(std::shared_ptr<windows::MainWindowInterface> windowArg, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestartorArg, std::shared_ptr<windows::WindowsManager> windowsManagerArg, std::unique_ptr<printer::PrintingVisitorInterface> &lineUpVisitorArg, std::unique_ptr<printer::PrintingVisitorInterface> &lineDownVisitorArg)
        : printingOrchestrator(printingOrchestartorArg), window(windowArg), windowsManager(windowsManagerArg), lineUpVisitor(lineUpVisitorArg), lineDownVisitor(lineDownVisitorArg)
    {
    }
} // namespace feather::action
