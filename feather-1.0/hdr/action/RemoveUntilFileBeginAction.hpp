#pragma once

#include "action/Action.hpp"
#include "action/input/RemoveUntilFileBeginActionInput.hpp"
#include "action/output/RemoveUntilFileBeginActionOutput.hpp"

namespace feather::action
{
  class RemoveUntilFileBeginAction : public Action<RemoveUntilFileBeginActionInput, RemoveUntilFileBeginActionOutput>
  {
  public:
    RemoveUntilFileBeginAction(RemoveUntilFileBeginActionInput &&);
    RemoveUntilFileBeginActionOutput execute();
  };
} // namespace feather::action