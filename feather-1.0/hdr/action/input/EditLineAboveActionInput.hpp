#pragma once

#include "printer/PrintingOrchestrator.hpp"
#include "windows/WindowsManager.hpp"

namespace feather::reader
{
    class KeyStroke;
};

namespace feather::action
{
    struct EditLineAboveActionInput
    {
        utils::datatypes::Uuid const &windowUUID;
        pair cursorPos;
        std::shared_ptr<utils::storage::AbstractStorage> storage;
        std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator;
        std::shared_ptr<windows::WindowsManager> windowsManager;
        reader::KeyStroke *keyStroke;
        EditLineAboveActionInput(utils::datatypes::Uuid const &, pair, std::shared_ptr<utils::storage::AbstractStorage>, std::shared_ptr<feather::printer::PrintingOrchestratorInterface>, reader::KeyStroke *, std::shared_ptr<windows::WindowsManager>);
    };
} // namespace feather::action