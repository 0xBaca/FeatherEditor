#pragma once

#include "printer/PrintingOrchestratorInterface.hpp"

namespace feather::action
{
    struct MoveToByteActionInput
    {
        size_t byteMoveTo;
        std::shared_ptr<printer::PrintingOrchestratorInterface> printingOrchestrator;
        utils::datatypes::Uuid const &windowUUID;
        std::shared_ptr<utils::storage::AbstractStorage> storage;
        MoveToByteActionInput(size_t, utils::datatypes::Uuid const &, std::shared_ptr<utils::storage::AbstractStorage>, std::shared_ptr<feather::printer::PrintingOrchestratorInterface>);
    };
} // namespace feather::action