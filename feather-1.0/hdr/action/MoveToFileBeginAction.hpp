#pragma once

#include "action/Action.hpp"
#include "action/input/MoveToFileBeginActionInput.hpp"
#include "action/output/MoveToFileBeginActionOutput.hpp"

namespace feather::action
{
class MoveToFileBeginAction : public Action<MoveToFileBeginActionInput, MoveToFileBeginActionOutput>
{
public:
    MoveToFileBeginAction(MoveToFileBeginActionInput &&);
    MoveToFileBeginActionOutput execute();
};
} // namespace feather::action
