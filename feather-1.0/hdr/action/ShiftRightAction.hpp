#pragma once

#include "action/Action.hpp"
#include "action/input/ShiftRightActionInput.hpp"
#include "action/output/ShiftRightActionOutput.hpp"

namespace feather::action
{
    class ShiftRightAction : public Action<ShiftRightActionInput, ShiftRightActionOutput>
    {
    public:
        ShiftRightAction(ShiftRightActionInput &&);
        void setCursor(pair);
        ShiftRightActionOutput execute();
    };
} // namespace feather::actio