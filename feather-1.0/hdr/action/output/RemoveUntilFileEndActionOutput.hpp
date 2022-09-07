#pragma once

#include "utils/datatypes/Global.hpp"

namespace feather::action
{
    struct RemoveUntilFileEndActionOutput
    {
        pair newCursorPosition;
        pair getResult();
    };
} // namespace feather::action