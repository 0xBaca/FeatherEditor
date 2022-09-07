#pragma once

#include "action/Action.hpp"
#include "action/input/RemoveLineActionInput.hpp"
#include "action/output/RemoveLineActionOutput.hpp"

namespace feather::action
{
  class RemoveLineAction : public Action<RemoveLineActionInput, RemoveLineActionOutput>
  {
  public:
    RemoveLineAction(RemoveLineActionInput &&);
    RemoveLineActionOutput execute();
  };
} // namespace feather::action