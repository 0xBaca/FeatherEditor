#pragma once

#include "action/Action.hpp"
#include "action/input/MoveToLineEndActionInput.hpp"
#include "action/output/MoveToLineEndActionOutput.hpp"

namespace feather::action
{
class MoveToLineEndAction : public Action<MoveToLineEndActionInput, MoveToLineEndActionOutput>
{
public:
    MoveToLineEndAction(MoveToLineEndActionInput &&);
    MoveToLineEndActionOutput execute();
}; // namespace feather::action

} // namespace feather::action
