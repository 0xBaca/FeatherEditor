#pragma once

#include "utils/datatypes/Global.hpp"

#include <optional>

namespace feather::action
{
    struct GotoLineActionOutput
    {
        std::optional<pair> result;
    };
} // namespace feather::action