#pragma once

#include "action/Action.hpp"
#include "action/input/GotoLineActionInput.hpp"
#include "action/output/GotoLineActionOutput.hpp"

namespace feather::action
{
    class GotoLineAction : public Action<GotoLineActionInput, GotoLineActionOutput>
    {
    public:
        GotoLineAction(GotoLineActionInput &&);
        void setCursor(pair);
        GotoLineActionOutput execute();
    };
} // namespace feather::action