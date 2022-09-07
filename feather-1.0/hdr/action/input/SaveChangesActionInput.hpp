#pragma once

#include "printer/ChangesStack.hpp"
#include "utils/storage/AbstractStorageFactory.hpp"
#include "windows/WindowsManager.hpp"

namespace feather::action
{
    struct SaveChangesActionInput
    {
        std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator;
        std::shared_ptr<utils::storage::AbstractStorageFactory> storageFactory;
        utils::datatypes::Uuid const &windowUUID;
        std::unique_ptr<utils::BufferFillerInterface> const &bufferFiller;
        std::shared_ptr<utils::storage::AbstractStorage> storage;
        std::shared_ptr<windows::WindowsManager> windowsManager;
        std::string savedFileName;
        std::unique_ptr<printer::PrintingVisitorInterface> &currFrameVisitor;
        std::shared_ptr<printer::ChangesStack> changesStack;
        std::u32string savedFileNameWideString;
        SaveChangesActionInput(std::shared_ptr<printer::PrintingOrchestratorInterface>, std::shared_ptr<utils::storage::AbstractStorageFactory>, utils::datatypes::Uuid const &, std::unique_ptr<utils::BufferFillerInterface> const &, std::shared_ptr<utils::storage::AbstractStorage>, std::shared_ptr<windows::WindowsManager>, std::string, std::unique_ptr<printer::PrintingVisitorInterface> &, std::shared_ptr<printer::ChangesStack>, std::u32string);
    };
} // namespace feather::action