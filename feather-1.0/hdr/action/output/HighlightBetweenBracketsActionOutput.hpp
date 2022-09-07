#pragma once

#include "utils/datatypes/Global.hpp"

#include <optional>

namespace feather::action
{
    struct HighlightBetweenBracketsActionOutput
    {
        std::optional<std::pair<pair, pair>> cursorPos;
    };
} // namespace feather::action