#pragma once

#include "printer/PrintingOrchestratorInterface.hpp"
#include "utils/helpers/KeyReader.hpp"
#include "windows/WindowsManager.hpp"

namespace feather::action
{
    struct GotoLineActionInput
    {
        size_t lineNumber;
        std::shared_ptr<utils::helpers::KeyReader> keyReader;
        std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator;
        utils::datatypes::Uuid const &windowUUID;
        std::shared_ptr<utils::storage::AbstractStorage> storage;
        std::shared_ptr<windows::WindowsManager> windowsManager;
        std::unique_ptr<printer::PrintingVisitorInterface> &currFrameVisitor;
        GotoLineActionInput(size_t, std::shared_ptr<utils::helpers::KeyReader>, utils::datatypes::Uuid const &, std::shared_ptr<utils::storage::AbstractStorage>, std::shared_ptr<feather::printer::PrintingOrchestratorInterface>, std::shared_ptr<windows::WindowsManager>, std::unique_ptr<printer::PrintingVisitorInterface> &);
    };
} // namespace feather::action