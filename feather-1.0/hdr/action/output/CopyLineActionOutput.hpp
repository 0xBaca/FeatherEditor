#pragma once

#include "utils/datatypes/Global.hpp"

#include <optional>

namespace feather::action
{
    struct CopyLineActionOutput
    {
        std::optional<std::pair<pair, pair>> newCursorPosition;
        std::optional<std::pair<pair, pair>> getResult();
    };
} // namespace feather::action