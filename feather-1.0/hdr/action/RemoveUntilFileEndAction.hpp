#pragma once

#include "action/Action.hpp"
#include "action/input/RemoveUntilFileEndActionInput.hpp"
#include "action/output/RemoveUntilFileEndActionOutput.hpp"

namespace feather::action
{
  class RemoveUntilFileEndAction : public Action<RemoveUntilFileEndActionInput, RemoveUntilFileEndActionOutput>
  {
  public:
    RemoveUntilFileEndAction(RemoveUntilFileEndActionInput &&);
    RemoveUntilFileEndActionOutput execute();
  };
} // namespace feather::action