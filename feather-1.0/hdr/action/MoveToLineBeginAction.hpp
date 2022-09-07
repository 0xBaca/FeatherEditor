#pragma once

#include "action/Action.hpp"
#include "action/input/MoveToLineBeginActionInput.hpp"
#include "action/output/MoveToLineBeginActionOutput.hpp"

namespace feather::action
{
class MoveToLineBeginAction : public Action<MoveToLineBeginActionInput, MoveToLineBeginActionOutput>
{
public:
    MoveToLineBeginAction(MoveToLineBeginActionInput &&);
    MoveToLineBeginActionOutput execute();
};
} // namespace feather::action
