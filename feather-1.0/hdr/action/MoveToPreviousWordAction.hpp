#pragma once

#include "action/Action.hpp"
#include "action/input/MoveToPreviousWordActionInput.hpp"
#include "action/output/MoveToPreviousWordActionOutput.hpp"

namespace feather::action
{
class MoveToPreviousWordAction : public Action<MoveToPreviousWordActionInput, MoveToPreviousWordActionOutput>
{
public:
    MoveToPreviousWordAction(MoveToPreviousWordActionInput &&);
    MoveToPreviousWordActionOutput execute();
};
} // namespace feather::action
