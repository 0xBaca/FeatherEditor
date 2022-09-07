#pragma once

#include "printer/ChangesStack.hpp"
#include "windows/MainWindowInterface.hpp"
#include "windows/WindowsManager.hpp"

namespace feather::action
{
    struct RemoveCharacterUnderCursorActionInput
    {
        utils::datatypes::Uuid const &windowUUID;
        pair cursorPos;
        std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator;
        std::shared_ptr<utils::storage::AbstractStorage> storage;
        std::shared_ptr<windows::WindowsManager> windowsManager;
        RemoveCharacterUnderCursorActionInput(utils::datatypes::Uuid const &, pair, std::shared_ptr<utils::storage::AbstractStorage>, std::shared_ptr<feather::printer::PrintingOrchestratorInterface>, std::shared_ptr<windows::WindowsManager>);
    };
} // namespace feather::action