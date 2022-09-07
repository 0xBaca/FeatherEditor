#include "action/input/ShiftRightActionInput.hpp"

namespace feather::action
{
    ShiftRightActionInput::ShiftRightActionInput(std::pair<pair, pair> rangeArg, pair currCursorPosArg, utils::datatypes::Uuid const &windowUUIDArg, std::shared_ptr<utils::storage::AbstractStorage> storageArg, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestartorArg, std::shared_ptr<windows::WindowsManager> windowsManagerArg, std::unique_ptr<printer::PrintingVisitorInterface> &currFrameVisitorArg)
        : range(rangeArg), currCursorPos(currCursorPosArg), printingOrchestrator(printingOrchestartorArg), windowUUID(windowUUIDArg), storage(storageArg), windowsManager(windowsManagerArg), currFrameVisitor(currFrameVisitorArg)
    {
    }
} // namespace feather::actio