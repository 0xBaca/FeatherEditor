#pragma once

#include "action/Action.hpp"
#include "action/input/EditLineAboveActionInput.hpp"
#include "action/output/EditLineAboveActionOutput.hpp"

namespace feather::action
{
    class EditLineAboveAction : public Action<EditLineAboveActionInput, EditLineAboveActionOutput>
    {
    public:
        EditLineAboveAction(EditLineAboveActionInput &&);
        EditLineAboveActionOutput execute();
    };
} // namespace feather::action