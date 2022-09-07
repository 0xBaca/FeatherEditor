#pragma once

#include "utils/datatypes/Global.hpp"

#include <map>

namespace feather::action
{
    struct RemoveAllActionOutput
    {
        std::map<pair, pair> deleted;
    };
} // namespace feather::actio