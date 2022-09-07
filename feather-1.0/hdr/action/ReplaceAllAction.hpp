#pragma once

#include "action/Action.hpp"
#include "action/input/ReplaceAllActionInput.hpp"
#include "action/output/ReplaceAllActionOutput.hpp"

namespace feather::action
{
  class ReplaceAllAction : public Action<ReplaceAllActionInput, ReplaceAllActionOutput>
  {
  public:
    ReplaceAllAction(ReplaceAllActionInput &&);
    ReplaceAllActionOutput execute();
  };
} // namespace feather::action