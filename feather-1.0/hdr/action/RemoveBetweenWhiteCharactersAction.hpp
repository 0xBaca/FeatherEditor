#pragma once

#include "action/Action.hpp"
#include "action/input/RemoveBetweenWhiteCharactersActionInput.hpp"
#include "action/output/RemoveBetweenWhiteCharactersActionOutput.hpp"

namespace feather::action
{
  class RemoveBetweenWhiteCharactersAction : public Action<RemoveBetweenWhiteCharactersActionInput, RemoveBetweenWhiteCharactersActionOutput>
  {
  public:
    RemoveBetweenWhiteCharactersAction(RemoveBetweenWhiteCharactersActionInput &&);
    RemoveBetweenWhiteCharactersActionOutput execute();
  };
} // namespace feather::action