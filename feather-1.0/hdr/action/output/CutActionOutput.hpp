#pragma once

#include "utils/datatypes/Global.hpp"

#include <optional>

namespace feather::action
{
    struct CutActionOutput
    {
        std::optional<std::pair<pair, pair>> newCursorPosition;
        std::optional<std::pair<pair, pair>> getResult();
    };
} // namespace feather::action