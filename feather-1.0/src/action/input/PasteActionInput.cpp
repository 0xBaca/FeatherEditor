#include "action/input/PasteActionInput.hpp"

namespace feather::action
{
    PasteActionInput::PasteActionInput(utils::datatypes::Uuid const &windowUUIDArg, std::pair<std::pair<utils::datatypes::Uuid, std::string>, std::pair<pair, pair>> sourceArg, std::shared_ptr<utils::storage::AbstractStorageFactory> storageFactoryArg, pair currCursorPosArg, std::shared_ptr<utils::storage::AbstractStorage> storageArg, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestartorArg, std::pair<std::pair<utils::datatypes::Uuid, std::string>, std::map<pair, pair>> textCopyPositionsToSkipArg, std::list<pair> &breakPointsArg, std::list<pair>::iterator &currentBreakPointArg, std::optional<std::pair<std::pair<utils::datatypes::Uuid, std::string>, utils::datatypes::DeletionsSnapshot>> deletionsSnapshotArg, std::set<pair> addedBreakPointsArg, std::shared_ptr<windows::WindowsManager> windowsManagerArg, std::unique_ptr<printer::PrintingVisitorInterface> &currFrameVisitorArg, bool isPastingNewLineArg)
        : windowUUID(windowUUIDArg), source(sourceArg), storageFactory(storageFactoryArg), currCursorPos(currCursorPosArg), storage(storageArg), printingOrchestrator(printingOrchestartorArg), windowsManager(windowsManagerArg), textCopyPositionsToSkip(textCopyPositionsToSkipArg), breakPoints(breakPointsArg), currentBreakPoint(currentBreakPointArg), deletionsSnapshot(deletionsSnapshotArg), addedBreakPoints(addedBreakPointsArg), currFrameVisitor(currFrameVisitorArg), isPastingNewLine(isPastingNewLineArg)
    {
    }
} // namespace feather::action