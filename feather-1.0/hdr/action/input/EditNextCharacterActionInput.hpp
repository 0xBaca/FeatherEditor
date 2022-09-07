#pragma once

#include "printer/PrintingOrchestratorInterface.hpp"

namespace feather::action
{
struct EditNextCharacterActionInput
{
    utils::datatypes::Uuid const &windowUUID;
    pair cursorPos;
    std::shared_ptr<utils::storage::AbstractStorage> storage;
    std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator;
    EditNextCharacterActionInput(utils::datatypes::Uuid const &, pair, std::shared_ptr<utils::storage::AbstractStorage>, std::shared_ptr<feather::printer::PrintingOrchestratorInterface>);
};
} // namespace feather::action
