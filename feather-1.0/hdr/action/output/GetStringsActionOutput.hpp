#pragma once

#include "utils/datatypes/Global.hpp"

#include <vector>

namespace feather::action
{
    struct GetStringsActionOutput
    {
        std::vector<std::pair<pair, pair>> result;
    };
} // namespace feather::actio