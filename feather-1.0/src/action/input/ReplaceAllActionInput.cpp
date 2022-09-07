#include "action/input/ReplaceAllActionInput.hpp"

namespace feather::action
{
    ReplaceAllActionInput::ReplaceAllActionInput(RemoveAllActionInput removeAllActionInputArg, utils::datatypes::Uuid const &windowUUIDArg, std::shared_ptr<windows::WindowsManager> windowsManagerArg, std::shared_ptr<utils::storage::AbstractStorageFactory> storageFactoryArg, std::unique_ptr<utils::BufferFillerInterface> const &bufferFillerArg, std::unique_ptr<printer::PrintingVisitorInterface> &currFrameVisitorArg, pair cursorPosArg, std::shared_ptr<utils::storage::AbstractStorage> storageArg, std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestartorArg, std::u32string replaceFromArg, std::u32string replaceToArg)
        : removeAllActionInput(removeAllActionInputArg), windowUUID(windowUUIDArg), windowsManager(windowsManagerArg), storageFactory(storageFactoryArg), bufferFiller(bufferFillerArg), currFrameVisitor(currFrameVisitorArg), cursorPos(cursorPosArg), storage(storageArg), printingOrchestrator(printingOrchestartorArg), replaceFrom(replaceFromArg), replaceTo(replaceToArg)
    {
    }
} // namespace feather::action