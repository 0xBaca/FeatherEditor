#pragma once

#include "action/Action.hpp"
#include "action/input/RemoveWordBackwardActionInput.hpp"
#include "action/output/RemoveWordBackwardActionOutput.hpp"

namespace feather::action
{
class RemoveWordBackwardAction : public Action<RemoveWordBackwardActionInput, RemoveWordBackwardActionOutput>
{
public:
  RemoveWordBackwardAction(RemoveWordBackwardActionInput &&);
  RemoveWordBackwardActionOutput execute();
};

} // namespace feather::action
