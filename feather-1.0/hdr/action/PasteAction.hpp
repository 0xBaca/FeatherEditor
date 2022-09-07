#pragma once

#include "action/Action.hpp"
#include "action/input/PasteActionInput.hpp"
#include "action/output/PasteActionOutput.hpp"

namespace feather::action
{
  class PasteAction : public Action<PasteActionInput, PasteActionOutput>
  {
  public:
    PasteAction(PasteActionInput &&);
    PasteActionOutput execute();
    pair getChunkToSkip(std::map<pair, pair> const &, pair const &);
  };
} // namespace feather::action