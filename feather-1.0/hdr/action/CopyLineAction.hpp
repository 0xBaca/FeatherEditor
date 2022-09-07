#pragma once

#include "action/Action.hpp"
#include "action/input/CopyLineActionInput.hpp"
#include "action/output/CopyLineActionOutput.hpp"

namespace feather::action
{
  class CopyLineAction : public Action<CopyLineActionInput, CopyLineActionOutput>
  {
  public:
    CopyLineAction(CopyLineActionInput &&);
    CopyLineActionOutput execute();
  };
} // namespace feather::action