#pragma once

#include "utils/datatypes/Global.hpp"

namespace feather::action
{
    struct RemoveBetweenWhiteCharactersActionOutput
    {
        pair newCursorPosition;
        pair getResult();
    };
} // namespace feather::action