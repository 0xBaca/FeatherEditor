#pragma once

#include "utils/datatypes/Global.hpp"

namespace feather::action
{
    struct RemoveLineActionOutput
    {
        pair newCursorPosition;
        pair getResult();
    };
} // namespace feather::action