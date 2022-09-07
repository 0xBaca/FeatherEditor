#pragma once

#include "action/Action.hpp"
#include "action/input/RemoveWordForwardActionInput.hpp"
#include "action/output/RemoveWordForwardActionOutput.hpp"

namespace feather::action
{
class RemoveWordForwardAction : public Action<RemoveWordForwardActionInput, RemoveWordForwardActionOutput>
{
public:
  RemoveWordForwardAction(RemoveWordForwardActionInput &&);
  RemoveWordForwardActionOutput execute();
};

} // namespace feather::action
