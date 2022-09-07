#include "action/input/GotoLineActionInput.hpp"

namespace feather::action
{
    GotoLineActionInput::GotoLineActionInput(size_t lineNumberArg, std::shared_ptr<utils::helpers::KeyReader> keyReaderArg, utils::datatypes::Uuid const &windowUUIDArg, std::shared_ptr<utils::storage::AbstractStorage> storageArg, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestartorArg, std::shared_ptr<windows::WindowsManager> windowsManagerArg, std::unique_ptr<printer::PrintingVisitorInterface> &currFrameVisitorArg)
        : lineNumber(lineNumberArg), keyReader(keyReaderArg), printingOrchestrator(printingOrchestartorArg), windowUUID(windowUUIDArg), storage(storageArg), windowsManager(windowsManagerArg), currFrameVisitor(currFrameVisitorArg)
    {
    }
} // namespace feather::action