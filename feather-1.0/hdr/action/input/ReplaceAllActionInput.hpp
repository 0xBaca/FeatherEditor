#pragma once

#include "action/input/RemoveAllActionInput.hpp"

namespace feather::action
{
    struct ReplaceAllActionInput
    {
        RemoveAllActionInput removeAllActionInput;
        utils::datatypes::Uuid const &windowUUID;
        std::shared_ptr<windows::WindowsManager> windowsManager;
        std::shared_ptr<utils::storage::AbstractStorageFactory> storageFactory;
        std::unique_ptr<utils::BufferFillerInterface> const &bufferFiller;
        std::unique_ptr<printer::PrintingVisitorInterface> &currFrameVisitor;
        pair cursorPos;
        std::shared_ptr<utils::storage::AbstractStorage> storage;
        std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator;
        std::u32string replaceFrom, replaceTo;
        ReplaceAllActionInput(RemoveAllActionInput, utils::datatypes::Uuid const &, std::shared_ptr<windows::WindowsManager>, std::shared_ptr<utils::storage::AbstractStorageFactory>, std::unique_ptr<utils::BufferFillerInterface> const &, std::unique_ptr<printer::PrintingVisitorInterface> &, pair, std::shared_ptr<utils::storage::AbstractStorage>, std::shared_ptr<feather::printer::PrintingOrchestratorInterface>, std::u32string, std::u32string);
    };
} // namespace feather::action