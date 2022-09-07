#pragma once

#include "action/Action.hpp"
#include "action/input/RemoveCharacterUnderCursorActionInput.hpp"
#include "action/output/RemoveCharacterUnderCursorActionOutput.hpp"

namespace feather::action
{
class RemoveCharacterUnderCursorAction : public Action<RemoveCharacterUnderCursorActionInput, RemoveCharacterUnderCursorActionOutput>
{
public:
  RemoveCharacterUnderCursorAction(RemoveCharacterUnderCursorActionInput &&);
  RemoveCharacterUnderCursorActionOutput execute();
};

} // namespace feather::action
