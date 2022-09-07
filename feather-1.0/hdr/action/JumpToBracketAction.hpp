#pragma once

#include "action/Action.hpp"
#include "action/input/JumpToBracketActionInput.hpp"
#include "action/output/JumpToBracketActionOutput.hpp"

namespace feather::action
{
    class JumpToBracketAction : public Action<JumpToBracketActionInput, JumpToBracketActionOutput>
    {
    public:
        JumpToBracketAction(JumpToBracketActionInput &&);
        JumpToBracketActionOutput execute();
    };
} // namespace feather::action