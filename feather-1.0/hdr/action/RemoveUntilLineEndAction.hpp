#pragma once

#include "action/Action.hpp"
#include "action/input/RemoveUntilLineEndActionInput.hpp"
#include "action/output/RemoveUntilLineEndActionOutput.hpp"

namespace feather::action
{
  class RemoveUntilLineEndAction : public Action<RemoveUntilLineEndActionInput, RemoveUntilLineEndActionOutput>
  {
  public:
    RemoveUntilLineEndAction(RemoveUntilLineEndActionInput &&);
    RemoveUntilLineEndActionOutput execute();
  };

} // namespace feather::action
