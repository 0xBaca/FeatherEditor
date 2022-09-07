#pragma once

#include "action/Action.hpp"
#include "action/input/SkipEmptyCharactersActionInput.hpp"
#include "action/output/SkipEmptyCharactersActionOutput.hpp"

namespace feather::action
{
    class SkipEmptyCharactersAction : public Action<SkipEmptyCharactersActionInput, SkipEmptyCharactersActionOutput>
    {
    public:
        SkipEmptyCharactersAction(SkipEmptyCharactersActionInput &&);
        SkipEmptyCharactersActionOutput execute();
    };
} // namespace feather::action