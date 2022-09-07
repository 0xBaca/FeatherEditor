#pragma once

#include "utils/datatypes/Global.hpp"

namespace feather::action
{
    struct RemoveUntilFileBeginActionOutput
    {
        pair newCursorPosition;
        pair getResult();
    };
} // namespace feather::action