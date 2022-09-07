#pragma once

#include <utility>

namespace feather::action
{
  template <typename Input, typename Output>
  class Action
  {
  protected:
    Action() = default;
    Action(Input, Output);
    Input actionInput;
    Output actionOutput;

  public:
    Input &getActionInput();
    Output &getActionOutput();
  };

  template <typename Input, typename Output>
  Action<Input, Output>::Action(Input input, Output output)
      : actionInput(std::move(input)), actionOutput(std::move(output))
  {
  }

  template <typename Input, typename Output>
  Input &Action<Input, Output>::getActionInput()
  {
    return actionInput;
  }

  template <typename Input, typename Output>
  Output &Action<Input, Output>::getActionOutput()
  {
    return actionOutput;
  }
} // namespace feather::action
