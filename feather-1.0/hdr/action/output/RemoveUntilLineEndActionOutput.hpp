#pragma once

#include "utils/datatypes/Global.hpp"

namespace feather::action
{
    struct RemoveUntilLineEndActionOutput
    {
        pair newCursorPosition;
        pair getResult();
    };
} // namespace feather::action
