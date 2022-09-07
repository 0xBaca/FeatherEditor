#pragma once

#include "action/Action.hpp"
#include "action/input/RemoveAllActionInput.hpp"
#include "action/output/RemoveAllActionOutput.hpp"

namespace feather::action
{
  class RemoveAllAction : public Action<RemoveAllActionInput, RemoveAllActionOutput>
  {
  public:
    static void sigBusHandler(int);
    static void sigIntHandler(int);
 
    RemoveAllAction(RemoveAllActionInput &&);
    RemoveAllActionOutput execute();
    pair getShiftedPos(pair, size_t);
    void unblockSignals();
  };
} // namespace feather::action