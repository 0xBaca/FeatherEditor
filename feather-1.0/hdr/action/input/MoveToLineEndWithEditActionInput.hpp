#pragma once

#include "printer/PrintingOrchestratorInterface.hpp"
#include "windows/WindowsManager.hpp"

namespace feather::action
{
struct MoveToLineEndWithEditActionInput
{
    utils::datatypes::Uuid const &windowUUID;
    pair cursorPos;
    std::shared_ptr<utils::storage::AbstractStorage> storage;
    std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator;
    std::shared_ptr<windows::WindowsManager> windowsManager;
    MoveToLineEndWithEditActionInput(utils::datatypes::Uuid const &, pair, std::shared_ptr<utils::storage::AbstractStorage>, std::shared_ptr<feather::printer::PrintingOrchestratorInterface>, std::shared_ptr<windows::WindowsManager>);
};
} // namespace feather::actio