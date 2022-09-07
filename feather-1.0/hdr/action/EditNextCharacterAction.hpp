#pragma once

#include "action/Action.hpp"
#include "action/input/EditNextCharacterActionInput.hpp"
#include "action/output/EditNextCharacterActionOutput.hpp"

namespace feather::action
{
    class EditNextCharacterAction : public Action<EditNextCharacterActionInput, EditNextCharacterActionOutput>
    {
    public:
        EditNextCharacterAction(EditNextCharacterActionInput &&);
        EditNextCharacterActionOutput execute();
    };
} // namespace feather::action
