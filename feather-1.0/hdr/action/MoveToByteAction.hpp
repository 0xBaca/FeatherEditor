#pragma once

#include "action/Action.hpp"
#include "action/input/MoveToByteActionInput.hpp"
#include "action/output/MoveToByteActionOutput.hpp"

namespace feather::action
{
    class MoveToByteAction : public Action<MoveToByteActionInput, MoveToByteActionOutput>
    {
    public:
        MoveToByteAction(MoveToByteActionInput &&);
        void setCursor(pair);
        MoveToByteActionOutput execute();
    };
} // namespace feather::action