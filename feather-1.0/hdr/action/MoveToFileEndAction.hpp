#pragma once

#include "action/Action.hpp"
#include "action/input/MoveToFileEndActionInput.hpp"
#include "action/output/MoveToFileEndActionOutput.hpp"

namespace feather::action
{
class MoveToFileEndAction : public Action<MoveToFileEndActionInput, MoveToFileEndActionOutput>
{
public:
    MoveToFileEndAction(MoveToFileEndActionInput &&);
    MoveToFileEndActionOutput execute();
};
} // namespace feather::action
