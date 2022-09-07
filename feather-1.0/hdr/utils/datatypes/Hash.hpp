#pragma once

#include <functional>

namespace feather::utils::datatypes
{
    struct PairHash
    {
        std::size_t operator()(std::pair<size_t, size_t> pair) const
        {
            return std::hash<size_t>()(pair.first) ^ std::hash<size_t>()(pair.second);
        }
    };
} // namespace feather::utils::datatypes