#pragma once

#include "utils/datatypes/Global.hpp"

#include <map>

namespace feather::action
{
    struct EditLineBelowActionOutput
    {
        std::pair<pair, size_t> positionToSkip;
        pair result;
        pair getResult();
    };
} // namespace feather::action