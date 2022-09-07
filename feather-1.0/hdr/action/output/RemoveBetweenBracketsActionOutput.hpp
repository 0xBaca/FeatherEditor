#pragma once

#include "utils/datatypes/Global.hpp"

#include <optional>

namespace feather::action
{
    struct RemoveBetweenBracketsActionOutput
    {
        std::optional<pair> newCursorPosition;
    };
} // namespace feather::action