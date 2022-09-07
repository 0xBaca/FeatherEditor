#pragma once

#include "action/Action.hpp"
#include "action/input/SwitchModeActionInput.hpp"
#include "action/output/SwitchModeActionOutput.hpp"

namespace feather::action
{
class SwitchModeAction : public Action<SwitchModeActionInput, SwitchModeActionOutput>
{
public:
    SwitchModeAction(SwitchModeActionInput &&);
    void setCursor(pair);
    SwitchModeActionOutput execute();
};
} // namespace feather::action