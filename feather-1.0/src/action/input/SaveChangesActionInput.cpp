#include "action/input/SaveChangesActionInput.hpp"

namespace feather::action
{
    SaveChangesActionInput::SaveChangesActionInput(std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestratorArg, std::shared_ptr<utils::storage::AbstractStorageFactory> storageFactoryArg, utils::datatypes::Uuid const &windowUUIDArg, std::unique_ptr<utils::BufferFillerInterface> const &bufferFillerArg, std::shared_ptr<utils::storage::AbstractStorage> storageArg, std::shared_ptr<windows::WindowsManager> windowsManagerArg, std::string saveFileNameArg, std::unique_ptr<printer::PrintingVisitorInterface> &currFrameVisitorArg, std::shared_ptr<printer::ChangesStack> changesStackArg, std::u32string savedFileNameWideStringArg)
        : printingOrchestrator(printingOrchestratorArg), storageFactory(storageFactoryArg), windowUUID(windowUUIDArg), bufferFiller(bufferFillerArg), storage(storageArg), windowsManager(windowsManagerArg), savedFileName(saveFileNameArg), currFrameVisitor(currFrameVisitorArg), changesStack(changesStackArg), savedFileNameWideString(savedFileNameWideStringArg)
    {
    }
} // namespace feather::action