#pragma once

#include "printer/PrintingOrchestrator.hpp"
#include "windows/WindowsManager.hpp"

namespace feather::reader
{
    class KeyStroke;
};

namespace feather::action
{
    struct EditLineBelowActionInput
    {
        utils::datatypes::Uuid const &windowUUID;
        pair cursorPos;
        std::shared_ptr<utils::storage::AbstractStorage> storage;
        std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator;
        reader::KeyStroke *keyStroke;
        std::shared_ptr<windows::WindowsManager> windowsManager;
        EditLineBelowActionInput(utils::datatypes::Uuid const &, pair, std::shared_ptr<utils::storage::AbstractStorage>, std::shared_ptr<feather::printer::PrintingOrchestratorInterface>, reader::KeyStroke *, std::shared_ptr<windows::WindowsManager>);
    };
} // namespace feather::action