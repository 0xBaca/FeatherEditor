#pragma once

#include "action/Action.hpp"
#include "action/input/AlignToMiddleOfScreenActionInput.hpp"
#include "action/output/AlignToMiddleOfScreenActionOutput.hpp"

namespace feather::action
{
class AlignToMiddleOfScreenAction : public Action<AlignToMiddleOfScreenActionInput, AlignToMiddleOfScreenActionOutput>
{
public:
  AlignToMiddleOfScreenAction(AlignToMiddleOfScreenActionInput &&);
  AlignToMiddleOfScreenActionOutput execute();
};

} // namespace feather::action
