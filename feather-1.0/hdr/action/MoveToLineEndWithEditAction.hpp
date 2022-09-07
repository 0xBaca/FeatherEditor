#pragma once

#include "action/Action.hpp"
#include "action/input/MoveToLineEndWithEditActionInput.hpp"
#include "action/output/MoveToLineEndWithEditActionOutput.hpp"

namespace feather::action
{
class MoveToLineEndWithEditAction : public Action<MoveToLineEndWithEditActionInput, MoveToLineEndWithEditActionOutput>
{
public:
    MoveToLineEndWithEditAction(MoveToLineEndWithEditActionInput &&);
    MoveToLineEndWithEditActionOutput execute();
}; // namespace feather::action

} // namespace feather::action
