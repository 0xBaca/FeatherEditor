#pragma once

#include "action/Action.hpp"
#include "action/input/EditLineBelowActionInput.hpp"
#include "action/output/EditLineBelowActionOutput.hpp"

namespace feather::action
{
    class EditLineBelowAction : public Action<EditLineBelowActionInput, EditLineBelowActionOutput>
    {
    public:
        EditLineBelowAction(EditLineBelowActionInput &&);
        EditLineBelowActionOutput execute();
    };
} // namespace feather::action