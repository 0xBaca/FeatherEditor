#pragma once

#include "action/Action.hpp"
#include "action/input/MoveToNextWordActionInput.hpp"
#include "action/output/MoveToNextWordActionOutput.hpp"

namespace feather::action
{
class MoveToNextWordAction : public Action<MoveToNextWordActionInput, MoveToNextWordActionOutput>
{
public:
    MoveToNextWordAction(MoveToNextWordActionInput &&);
    MoveToNextWordActionOutput execute();
};
} // namespace feather::action
