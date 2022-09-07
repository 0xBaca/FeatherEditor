#pragma once

#include "action/Action.hpp"
#include "action/input/CutActionInput.hpp"
#include "action/output/CutActionOutput.hpp"

namespace feather::action
{
    class CutAction : public Action<CutActionInput, CutActionOutput>
    {
    public:
        CutAction(CutActionInput &&);
        CutActionOutput execute();
    };
} // namespace feather::action